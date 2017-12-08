import serial
import requests
from time import sleep


bluetoothSerial = serial.Serial("/dev/rfcomm0", baudrate=9600 )
count = None

while True:
    batimentos = bluetoothSerial.readline()
    print(batimentos)
    content = {
        "beats": batimentos
    }
    print(content)
    requests.post('http://0.0.0.0:8000/api/v1/heartbeats/', json=content)
