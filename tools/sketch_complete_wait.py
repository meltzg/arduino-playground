import argparse
import serial


def __main__(conn: serial.Serial):
    read = b''
    while b'Complete' not in read:
        read += conn.read_all()

    for line in read.splitlines():
        print(line)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Wait for sketch to complete')
    parser.add_argument('--port', default='/dev/ttyUSB0')
    parser.add_argument('--baud', default=9600, type=int)
    args = parser.parse_args()
    print(args)

    with serial.Serial(port=args.port, baudrate=args.baud) as conn:
        __main__(conn)