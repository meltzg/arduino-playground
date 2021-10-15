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

    def send_message(self, dest: bytes, command: int, payload: bytes, ignore_actor: bool = True):
        options = 0
        if ignore_actor:
            options |= 0x40
        message = (
            START_CODE
            + HARD_PORT
            + dest
            + len(payload).to_bytes(2, 'little')
            + options.to_bytes(1, 'little')
            + command.to_bytes(1, 'little')
            + payload
        )
        print("sending message: ", message)
        while self._conn.read() != b'\xab':
            self._conn.write(b'\xaa')
        self._conn.write(message)

    def read_response(self) -> bytes:
        b = 0
        while b != b'\xaa':
            b = self._conn.read()
        self._conn.write(b'\xab')
        while b != b'\xac':
            b = self._conn.read()
        print("read message")
        message = self._conn.read(10)
        payload_size = int.from_bytes(message[4:6], 'little')
        message += self._conn.read(payload_size)
        print("payload size {}".format(payload_size))
        return message


    @property
    def id(self):
        self.send_message(HARD_PORT, 0x01, b"")
        resp = self.read_response()
        print(resp)
        return resp[-ID_SIZE:]

    @property
    def neighbors(self):
        return self.get_neighbors(self.id)

    @property
    def discover_stats(self):
        self.send_message(self.id, 0x06, b"")
        sleep(0.25)
        resp = self._conn.read_all()
        print(resp)
        return resp[-ID_SIZE:]

    def get_neighbors(self, dest):
        self.send_message(dest, 0x03, b"")
        resp = self.read_response()
        print(resp)
        idstr = resp[-(6 * ID_SIZE) :]
        return [idstr[i : i + ID_SIZE] for i in range(0, len(idstr), ID_SIZE)]

    def start_discovery(self):
        self.send_message(self.id, 0x05, b"")
        sleep(0.25)
        whole = b""
        resp = self._conn.read_all()
        while resp:
            whole += resp
            resp = self._conn.read_all()

        print(resp.split(b"\r\n"))


if __name__ == '__main__':
    router = Router(serial.Serial('/dev/ttyUSB0', 9600, timeout=3))
    print(router.neighbors)
