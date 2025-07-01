#include <WiFi.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <math.h>
#include <Wire.h>
#include <MPU6050.h>

// ======= CONFIGURACIÓN WIFI ======= //
const char* ssid = "Obi_WAN_Kenobi";        // Tu red WiFi
const char* password = "quesadilla"; 

// ======= CONFIGURACIÓN TELEGRAM ======= //
const String botToken = "7946556001:AAE-fiNMfhvAJXQ-8sGfhG7u5pwc-imoIKI";  // Token de tu bot de Telegram
const String chatId = "1152170598";       // ID de tu chat

// ======= CONFIGURACIÓN GPS ======= //
#define GPS_RX 16
#define GPS_TX 17
#define PI 3.14159265

// ======= CONFIGURACIÓN MPU6050 ======= //
MPU6050 mpu;
const float ACCEL_THRESHOLD = 1.0;      // Umbral de aceleración para detección (1g)
bool vehicleOn = false;                 // Estado del vehículo
bool vehicleMoving = false;             // Estado de movimiento
const float SPEED_THRESHOLD = 30.0;     // 30 km/h para alerta de movimiento

// Variables GPS
String gpsData = "";
String latStr = "";
String lonStr = "";
String utcTime = "";
char latDir, lonDir;
bool newGPSData = false;
float latitude = 0.0;
float longitude = 0.0;

// Variables Velocidad GPS
float speedKnots = 0.0;
float speedKmh = 0.0;
bool newSpeedData = false;

// ======= CONFIGURACIÓN GEOCERCA ======= //
float ref_lat = 22.772429;    // Latitud de referencia inicial
float ref_lon = -102.644426;  // Longitud de referencia inicial
float fence_radius = 200.0;   // Radio en metros inicial

// ======= TEMPORIZADORES ======= //
unsigned long lastTelegramCheck = 0;
const long telegramCheckInterval = 5000; // Verificar comandos cada 5 seg
unsigned long lastAutoReport = 0;
const long autoReportInterval = 3600000; // 1 hora = 3600000 ms
unsigned long lastAccelCheck = 0;
const long accelCheckInterval = 1000;    // Verificar acelerómetro cada 1 seg

// ======= CONTROL DE COMANDOS ======= //
unsigned long lastUpdateId = 0; // Para seguimiento de mensajes procesados

void setup() {
    Serial.begin(9600);
    delay(1500);
    
    // Conectar al WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando al WiFi...");
    }
    Serial.println("WiFi conectado");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    
    // Inicializar GPS
    Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("Inicializando GPS...");
    
    // Inicializar MPU6050
    Wire.begin();
    mpu.initialize();
    if (!mpu.testConnection()) {
        Serial.println("Error: MPU6050 no conectado!");
    } else {
        Serial.println("MPU6050 inicializado");
        mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // Rango ±2g
    }
    
    // Mostrar configuración inicial
    Serial.println("\nConfiguración Inicial:");
    Serial.print("Geocerca: ");
    Serial.print(ref_lat, 6);
    Serial.print(", ");
    Serial.print(ref_lon, 6);
    Serial.print(" | Radio: ");
    Serial.print(fence_radius);
    Serial.println(" m");
    
    // Enviar mensaje de inicio
    sendTelegramMessage("Sistema iniciado. Comandos disponibles:\n"
                        "/ubicacion - Obtener ubicación actual\n"
                        "/radio NUM - Cambir radio de geocerca\n"
                        "/base LAT,LON - Cambiar ubicación de referencia");
}

void loop() {
    // Leer datos GPS
    readGPS();
    
    // Procesar nuevos datos GPS
    if (newGPSData) {
        processGPSData();
        newGPSData = false;
    }
    
    // Procesar datos de velocidad
    if (newSpeedData) {
        checkVehicleState();
        newSpeedData = false;
    }
    
    // Verificar acelerómetro
    if (millis() - lastAccelCheck >= accelCheckInterval) {
        checkAccelerometer();
        lastAccelCheck = millis();
    }
    
    // Verificar comandos de Telegram
    if (millis() - lastTelegramCheck >= telegramCheckInterval) {
        checkTelegramCommands();
        lastTelegramCheck = millis();
    }
    
    // Enviar reporte automático cada hora
    if (millis() - lastAutoReport >= autoReportInterval) {
        if (isValidGPSData()) {
            sendLocationReport();
        }
        lastAutoReport = millis();
    }
    
    delay(50); // Pequeña pausa
}

// ======= DETECCIÓN DE ESTADO DEL VEHÍCULO ======= //

void checkAccelerometer() {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    // Convertir a g (rango ±2g, 16384 LSB/g)
    float accelX = ax / 16384.0;
    float accelY = ay / 16384.0;
    float accelZ = az / 16384.0;
    
    // Calcular magnitud de la aceleración
    float magnitude = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
    
    // Detectar encendido del vehículo
    if (magnitude > ACCEL_THRESHOLD && !vehicleOn) {
        vehicleOn = true;
        sendTelegramMessage("Alerta: Vehículo encendido (Aceleración: " + String(magnitude, 2) + "g)");
    }
}

void checkVehicleState() {
    // Detectar si el vehículo está en movimiento
    if (speedKmh > SPEED_THRESHOLD && !vehicleMoving) {
        vehicleMoving = true;
        sendTelegramMessage("Alerta: Vehículo en movimiento (Velocidad: " + String(speedKmh, 1) + " km/h)");
    } 
    // Resetear estados cuando se detiene
    else if (speedKmh < 5.0) {
        vehicleOn = false;
        vehicleMoving = false;
    }
}

// ======= FUNCIONES PRINCIPALES ======= //

void readGPS() {
    while (Serial2.available()) {
        char c = Serial2.read();
        if (c == '\n') {
            if (gpsData.startsWith("$GPGLL")) {
                parseGPGLL();
            }
            if (gpsData.startsWith("$GPRMC")) {
                parseGPRMC();
            }
            gpsData = "";
        } else if (c != '\r') {
            gpsData += c;
        }
    }
}

void processGPSData() {
    // Calcular distancia a referencia
    float distance = calculateDistance(latitude, longitude, ref_lat, ref_lon);
    
    // Mostrar en consola
    Serial.println("----------------------------------");
    Serial.print("UTC: ");
    Serial.println(utcTime);
    Serial.print("Ubicación: ");
    Serial.print(latitude, 6);
    Serial.print(", ");
    Serial.println(longitude, 6);
    Serial.print("Velocidad: ");
    Serial.print(speedKmh, 1);
    Serial.println(" km/h");
    Serial.print("Distancia a referencia: ");
    Serial.print(distance);
    Serial.print(" m | Estado: ");
    Serial.println(distance <= fence_radius ? "DENTRO" : "FUERA");
    Serial.println("----------------------------------");
}

void checkTelegramCommands() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/getUpdates?offset=" + String(lastUpdateId + 1);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        http.end();
        
        // Buscar mensajes nuevos
        int updateStart = payload.indexOf("\"update_id\"");
        while (updateStart != -1) {
            // Obtener update_id
            int idStart = payload.indexOf(":", updateStart) + 1;
            int idEnd = payload.indexOf(",", idStart);
            unsigned long updateId = payload.substring(idStart, idEnd).toInt();
            lastUpdateId = updateId;
            
            // Buscar texto del mensaje
            int textStart = payload.indexOf("\"text\"", updateStart);
            if (textStart != -1) {
                textStart = payload.indexOf("\"", textStart + 7) + 1;
                int textEnd = payload.indexOf("\"", textStart);
                String command = payload.substring(textStart, textEnd);
                
                // Procesar comando
                if (command.startsWith("/ubicacion")) {
                    handleLocationCommand();
                }
                else if (command.startsWith("/radio")) {
                    handleRadiusCommand(command);
                }
                else if (command.startsWith("/base")) {
                    handleBaseCommand(command);
                }
            }
            updateStart = payload.indexOf("\"update_id\"", updateStart + 1);
        }
    } else {
        http.end();
        Serial.print("Error en getUpdates: ");
        Serial.println(httpCode);
    }
}

// ======= MANEJO DE COMANDOS ======= //

void handleLocationCommand() {
    if (isValidGPSData()) {
        sendLocationReport();
    } else {
        sendTelegramMessage("Esperando datos GPS válidos...");
    }
}

void handleRadiusCommand(String command) {
    String value = command.substring(7); // Saltar "/radio "
    float newRadius = value.toFloat();
    
    if (newRadius > 0) {
        fence_radius = newRadius;
        sendTelegramMessage("Radio actualizado: " + String(fence_radius) + " metros");
        Serial.println("Radio geocerca actualizado: " + String(fence_radius) + " m");
    } else {
        sendTelegramMessage("Error: Radio inválido. Uso: /radio 300");
    }
}

void handleBaseCommand(String command) {
    String coords = command.substring(6); // Saltar "/base "
    int commaPos = coords.indexOf(',');
    
    if (commaPos != -1) {
        String latStr = coords.substring(0, commaPos);
        String lonStr = coords.substring(commaPos + 1);
        
        float newLat = latStr.toFloat();
        float newLon = lonStr.toFloat();
        
        if (newLat >= -90.0 && newLat <= 90.0 && newLon >= -180.0 && newLon <= 180.0) {
            ref_lat = newLat;
            ref_lon = newLon;
            String msg = "Base actualizada: " + String(ref_lat, 6) + ", " + String(ref_lon, 6);
            sendTelegramMessage(msg);
            Serial.println(msg);
        } else {
            sendTelegramMessage("Error: Coordenadas inválidas. Rango válido: Lat[-90,90] Lon[-180,180]");
        }
    } else {
        sendTelegramMessage("Error: Formato inválido. Uso: /base 22.123456,-102.123456");
    }
}

// ======= FUNCIONES DE REPORTE ======= //

void sendLocationReport() {
    float distance = calculateDistance(latitude, longitude, ref_lat, ref_lon);
    bool inside = (distance <= fence_radius);
    
    String report = "Reporte GPS:\n";
    report += "Estado Geocerca: " + String(inside ? "DENTRO" : "FUERA") + "\n";
    report += "Ubicacion: " + String(latitude, 6) + ", " + String(longitude, 6) + "\n";
    report += "Velocidad: " + String(speedKmh, 1) + " km/h\n";
    report += "UTC: " + utcTime + "\n";
    report += "Config: " + String(ref_lat, 6) + "," + String(ref_lon, 6);
    report += " | Radio: " + String(fence_radius) + " m";
    
    sendTelegramMessage(report);
}

bool sendTelegramMessage(String message) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    String payload = "chat_id=" + chatId + "&text=" + urlEncode(message);
    
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(payload);
    
    bool success = (httpCode == 200);
    http.end();
    return success;
}

// ======= FUNCIONES AUXILIARES ======= //

String urlEncode(String str) {
    String encoded = "";
    for (unsigned int i = 0; i < str.length(); i++) {
        char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += "%20";
        } else {
            char hex[4];
            sprintf(hex, "%%%02X", c);
            encoded += hex;
        }
    }
    return encoded;
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371000; // Radio de la Tierra en metros
    float phi1 = lat1 * PI / 180.0;
    float phi2 = lat2 * PI / 180.0;
    float deltaPhi = (lat2 - lat1) * PI / 180.0;
    float deltaLambda = (lon2 - lon1) * PI / 180.0;

    float a = sin(deltaPhi / 2.0) * sin(deltaPhi / 2.0) +
              cos(phi1) * cos(phi2) *
              sin(deltaLambda / 2.0) * sin(deltaLambda / 2.0);
    
    float c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R * c;
}

bool isValidGPSData() {
    return (latitude != 0.0 && longitude != 0.0 && utcTime != "");
}

void parseGPGLL() {
    int firstComma = gpsData.indexOf(',');
    int secondComma = gpsData.indexOf(',', firstComma + 1);
    int thirdComma = gpsData.indexOf(',', secondComma + 1);
    int fourthComma = gpsData.indexOf(',', thirdComma + 1);
    int fifthComma = gpsData.indexOf(',', fourthComma + 1);
    int sixthComma = gpsData.indexOf(',', fifthComma + 1);

    if (sixthComma != -1) {
        latStr = gpsData.substring(firstComma + 1, secondComma);
        latDir = gpsData.charAt(secondComma + 1);
        lonStr = gpsData.substring(thirdComma + 1, fourthComma);
        lonDir = gpsData.charAt(fourthComma + 1);
        utcTime = gpsData.substring(fifthComma + 1, sixthComma);
        
        latitude = convertCoords(latStr);
        longitude = convertCoords(lonStr);
        
        if (latDir == 'S') latitude = -latitude;
        if (lonDir == 'W') longitude = -longitude;
        
        newGPSData = true;
    }
}

void parseGPRMC() {
    int firstComma = gpsData.indexOf(',');
    int secondComma = gpsData.indexOf(',', firstComma + 1);
    int thirdComma = gpsData.indexOf(',', secondComma + 1);
    int fourthComma = gpsData.indexOf(',', thirdComma + 1);
    int fifthComma = gpsData.indexOf(',', fourthComma + 1);
    int sixthComma = gpsData.indexOf(',', fifthComma + 1);
    int seventhComma = gpsData.indexOf(',', sixthComma + 1);
    int eighthComma = gpsData.indexOf(',', seventhComma + 1);
    
    if (eighthComma == -1) return;
    
    String status = gpsData.substring(secondComma + 1, thirdComma);
    if (status != "A") return;
    
    String speedStr = gpsData.substring(seventhComma + 1, eighthComma);
    if (speedStr.length() > 0) {
        speedKnots = speedStr.toFloat();
        speedKmh = speedKnots * 1.852;
        newSpeedData = true;
    }
}

float convertCoords(String coord) {
    if (coord.length() < 4) return 0.0;
    int dotIndex = coord.indexOf('.');
    if (dotIndex < 2) return 0.0;
    float degrees = coord.substring(0, dotIndex - 2).toFloat();
    float minutes = coord.substring(dotIndex - 2).toFloat();
    return degrees + (minutes / 60.0);
}