import threading
import serial
import sys
import redis
import time

REDIS_DB = 0
REDIS_QUEUE = "queueme"
SERIALPORT = "/dev/ttyAMA0"
BAUDRATE = 115200

try:
    ser = serial.Serial(SERIALPORT, BAUDRATE, timeout=10)
    red = redis.StrictRedis(db=REDIS_DB)
except OSError:
    print "No serial connection"
    quit()
except Exception as e:
    print "Failed to init: %s" % e
    quit()


def send_serial_data(data):
    if (data_is_valid(data)):
        print "Sending: %s" % data
        ser.write(data)


def main():
    while True:
        # Pop the redis queue to see if we have something
        send_serial_data(red.lpop(REDIS_QUEUE))


def read_from_serial():
    # Wait for data to come in from the serial and handle it
    print "Waiting for serial..."
    while True:
        line = ser.readline().strip()
        if line == "": continue
        print "Raw from serial: %s" % line

        process_serial_data(line)


def process_serial_data(line):
    data = line.split(':')
    if len(data) == 2:
        command = data[0]
        payload = data[1]
        print "Command: %s, Data: %s" % (command, payload)
    else:
        print "Serial data is incorrect: %s" % line


def data_is_valid(data):
    # Should be in the format x c:d where x is a node number, and c and d are
    # strings.
    if data is None: return False

    (node_id, payload) = data.split(' ')
    if is_number(node_id):
        (cmd, data) = payload.split(':')
        if (len(cmd) > 0 and len(data) > 0):
            return True
    return False


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


thread = threading.Thread(target=read_from_serial)
thread.daemon = True
thread.start()

main()
