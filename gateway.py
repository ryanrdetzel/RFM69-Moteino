import threading
import serial
import sys
import redis
import time

# 001 001 MSG:DATA

REDIS_DB = 0
REDIS_QUEUE = "queueme"
SERIALPORT = "/dev/ttyAMA0"
BAUDRATE = 115200

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

try:
    ser = serial.Serial(SERIALPORT, BAUDRATE, timeout=10)
except OSError:
    print "No serial connection"
    ser = None

def sendData(data):
    # Check to see if it's valid?
    print "Sending %s" % data
    if ser: ser.write(data)

def main():
    red = redis.StrictRedis(db=REDIS_DB)
    print "Loaded"
    while True:
        try:
            ## See if we have something in the queue to send
            pending = red.lpop(REDIS_QUEUE)
            if pending is not None:
                print "Got: %s" % pending
                sendData(pending)
        except KeyboardInterrupt:
            raise

def write_to_serial():
    ## Check serial for incoming data
    print "Waiting for serial"
    while True:
        if ser:
            line = ser.readline()
            if line == "": continue
            print "Raw: %s" % line
            data = line.rstrip().split(':')
            if len(data) == 2:
                command = data[0]
                payload = data[1]

                print "Command: %s, Data: %s" % (command, payload)

thread = threading.Thread(target=write_to_serial)
thread.start()

main()