from time import sleep
import typing as t
import serial


START_CODE = bytes([0xac])
HARD_PORT = b'\xff' * 4


def to_bytes(val: int, num_bytes: int):
    parts = []

    for _ in range(num_bytes):
        parts.append(val & 0xff)
        val = val >> 8

    return bytes(parts)


def send_message(ser: serial.Serial, dest: bytes, command: int, payload: bytes):
    message = START_CODE + HARD_PORT + dest + to_bytes(len(payload), 2) + to_bytes(command, 1) + payload
    print("sending message: ", message)
    ser.write(message)


def get_id(ser):
    send_message(ser, HARD_PORT, 0x01, b'')
    sleep(0.25)
    resp = ser.read_all()
    print(resp)
    return resp[-4:]


def get_neighbors(ser, dest):
    send_message(ser, dest, 0x04, b'')
    resp = ser.read(10000)
    print(resp)
    idstr = resp[-(6 * 4):]
    return [idstr[i:i + 4] for i in range(0, len(idstr), 4)]
