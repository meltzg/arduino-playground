import logging
from time import sleep
import typing as t
import serial

__LOGGER = logging.getLogger(__name__)


START_CODE = bytes([0xAC])
ID_SIZE = 2
HARD_PORT = b"\xff" * ID_SIZE


class Router(object):
    def __init__(self, conn: serial.Serial):
        self._conn = conn

    def send_message(self, dest: bytes, command: int, payload: bytes):
        message = (
            START_CODE
            + HARD_PORT
            + dest
            + to_bytes(len(payload), 2)
            + to_bytes(command, 1)
            + payload
        )
        print("sending message: ", message)
        self._conn.write(message)

    @property
    def id(self):
        self.send_message(HARD_PORT, 0x01, b"")
        sleep(0.25)
        resp = self._conn.read_all()
        print(resp)
        return resp[-ID_SIZE:]

    @property
    def neighbors(self):
        return self.get_neighbors(self.id)

    def get_neighbors(self, dest):
        self.send_message(dest, 0x02, b"")
        resp = self._conn.read(10000)
        print(resp)
        idstr = resp[-(6 * ID_SIZE) :]
        return [idstr[i : i + ID_SIZE] for i in range(0, len(idstr), ID_SIZE)]

    def start_discovery(self):
        self.send_message(self.id, 0x08, b"")
        sleep(0.25)
        whole = b""
        resp = self._conn.read_all()
        while resp:
            whole += resp
            resp = self._conn.read_all()

        print(resp.split(b"\r\n"))


def to_bytes(val: int, num_bytes: int):
    parts = []

    for _ in range(num_bytes):
        parts.append(val & 0xFF)
        val = val >> 8

    return bytes(parts)
