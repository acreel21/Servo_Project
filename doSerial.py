import serial
import sys
import time

ser = serial.Serial(
        port='/dev/ttyS0',\
        baudrate=9600,\
        parity=serial.PARITY_NONE,\
        stopbits=serial.STOPBITS_ONE,\
        bytesize=serial.EIGHTBITS,\
            timeout=1)

print("connected to: " + ser.portstr)
while True:
    userinput1 = raw_input("Enter direction of servo (Either 0 for positive or 1 for negative or E to exit)")
    if (('E') == (userinput1)):
        ser.close()
        exit()
    else:
        userinput2 = (raw_input("The angle to servo: "))
        if (int(userinput2) > 255):
            usernum = int(userinput2) >> 4
            userinput2 = str(usernum)
            shift = str(0)
        else:
            shift = str(1)
        checksum = (255%(int(int(userinput1) + int(userinput2) + int(shift))))
        ser.write(str(chr(0xFF) + chr(0xFF) + chr(int(userinput1)) + chr(int(userinput2)) + chr(int(shift))  + chr(int(checksum))))
        buff = ser.read(25)
        if (len(buff)>0):
            sys.stdout.write(buff)
