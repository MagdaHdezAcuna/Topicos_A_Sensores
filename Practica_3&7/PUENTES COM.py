import serial

def transfer_data(com_read='COM13', com_write='COM7', baudrate=9600):
    try:
        with serial.Serial(com_read, baudrate, timeout=1) as ser_read, \
             serial.Serial(com_write, baudrate, timeout=1) as ser_write:
            print(f"Transfiriendo datos de {com_read} a {com_write}...")
            while True:
                if ser_read.in_waiting:
                    data = ser_read.read(ser_read.in_waiting)
                    decoded_data = data.decode('utf-8', errors='ignore')
                    ser_write.write(decoded_data.encode('utf-8'))
                    print(f"Transferido: {decoded_data}")
    except serial.SerialException as e:
        print(f"Error de conexión: {e}")
    except KeyboardInterrupt:
        print("Transferencia detenida.")

if __name__ == "__main__":
    transfer_data()