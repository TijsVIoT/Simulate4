import pyodbc
import serial
import time

# Verbinding met de Access database
def get_db_connection():
    try:
        conn = pyodbc.connect(r'DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\Users\User\OneDrive\Documenten\Database1.accdb;')
        return conn
    except pyodbc.Error as e:
        print(f"Error connecting to database: {e}")
        return None

# Gewicht opslaan in de Access database
def save_weight_to_db(weight):
    conn = get_db_connection()
    if conn:
        try:
            cursor = conn.cursor()
            cursor.execute('INSERT INTO [waardes sensor] (gewicht) VALUES (?)', (weight,))
            conn.commit()
            cursor.close()
            conn.close()
            print(f"Weight {weight} g saved to database")
        except pyodbc.Error as e:
            print(f"Error inserting into database: {e}")

# Seriële poort configureren
try:
    ser = serial.Serial('COM3', 115200)  # Pas COM9 aan naar de juiste poort
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None

# Lees seriële gegevens en sla ze op in de database
if ser:
    while True:
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip()
                print(f"Received line: {line}")  # Debug output
                if line.startswith("weight is:"):
                    try:
                        weight_str = line.split(": ")[1].split(" ")[0]
                        weight = float(weight_str)
                        save_weight_to_db(weight)
                    except ValueError as e:
                        print(f"Error parsing weight: {e}, line: {line}")
        except serial.SerialException as e:
            print(f"Error reading from serial port: {e}")
        time.sleep(1)  # Kleine vertraging om CPU-gebruik te minimaliseren
else:
    print("Serial port is not available.")



