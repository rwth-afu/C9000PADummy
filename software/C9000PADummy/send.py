
import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=38400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    xonxoff=serial.XOFF,
    rtscts=False,
    dsrdtr=False
)

#ser.open()
ser.isOpen()

print("Initializing the device ..")

data = "\x09"
ser.write(data)

print('Done')
