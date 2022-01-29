from email.mime import base
import logging
from pydoc import cli
from time import sleep
import typing as t

import click

import serial

__LOGGER = logging.getLogger(__name__)


START_CODE = bytes([0xAC])
ID_SIZE = 2
HARD_PORT = b"\xff" * ID_SIZE


class Message(object):
    def __init__(self, header_bytes: bytes, conn: serial.Serial) -> None:
        self.source = int.from_bytes(header_bytes[0:2], "little")
        self.dest = int.from_bytes(header_bytes[2:4], "little")
        self.payload_size = int.from_bytes(header_bytes[4:6], "little")
        self.sys_options = header_bytes[6]
        self.sys_command = header_bytes[7]
        self.payload = conn.read(self.payload_size)


class DiscoveryStats(object):
    def __init__(self, resp_bytes):
        self.done = bool(resp_bytes[0])
        self.nodes = int.from_bytes(resp_bytes[1:3], "little")
        self.edges = int.from_bytes(resp_bytes[3:], "little")


@click.group()
@click.option("--port", default="/dev/ttyUSB0", type=str)
@click.option("--baud", default=9600, type=int)
@click.option("--timeout", default=3, type=int)
@click.pass_context
def router(ctx, port, baud, timeout):
    ctx.ensure_object(dict)
    ctx.obj["PORT"] = port
    ctx.obj["BAUD"] = baud
    ctx.obj["TIMEOUT"] = timeout


@router.command()
@click.pass_context
def id(ctx):
    with serial.Serial(
        port=ctx.obj["PORT"], baudrate=ctx.obj["BAUD"], timeout=ctx.obj["TIMEOUT"]
    ) as conn:
        id_ = get_id(conn)
        click.echo(f"My ID: {hex(id_)}")


@router.command()
@click.option("--node", default=None, type=str)
@click.option("--cache/--no-cache", default=True)
@click.pass_context
def neighbors(ctx, node, cache):
    with serial.Serial(
        port=ctx.obj["PORT"], baudrate=ctx.obj["BAUD"], timeout=ctx.obj["TIMEOUT"]
    ) as conn:
        requested_id = get_id(conn) if node is None else int(node, base=16)
        send_message(
            conn, requested_id.to_bytes(ID_SIZE, "little"), 0x03, b"", use_cache=cache
        )
        resp = read_response(conn)
        neighbor_ids = [
            int.from_bytes(resp.payload[i : i + ID_SIZE], "little")
            for i in range(ID_SIZE, len(resp.payload), ID_SIZE)
        ]
        click.echo(f"Neighbors for {requested_id}")
        for n in neighbor_ids:
            click.echo(hex(n))


@router.command()
@click.pass_context
def discovery_stats(ctx):
    with serial.Serial(
        port=ctx.obj["PORT"], baudrate=ctx.obj["BAUD"], timeout=ctx.obj["TIMEOUT"]
    ) as conn:
        id_ = get_id(conn)
        send_message(conn, id_.to_bytes(ID_SIZE, "little"), 0x06, b"")
        resp = read_response(conn)
        stats = DiscoveryStats(resp.payload)
        click.echo(f"Discover done: {stats.done}")
        click.echo(f"Num nodes: {stats.nodes}")
        click.echo(f"Num edges: {stats.edges}")


@router.command()
@click.pass_context
def start_discovery(ctx):
    with serial.Serial(
        port=ctx.obj["PORT"], baudrate=ctx.obj["BAUD"], timeout=ctx.obj["TIMEOUT"]
    ) as conn:
        id_ = get_id(conn)
        send_message(conn, id_.to_bytes(ID_SIZE, "little"), 0x05, b"")


def get_id(conn: serial.Serial) -> int:
    send_message(conn, HARD_PORT, 0x01, b"")
    resp = read_response(conn)
    id_ = int.from_bytes(resp.payload, "little")
    return id_


def send_message(
    conn: serial.Serial,
    dest: bytes,
    command: int,
    payload: bytes,
    ignore_actor: bool = True,
    use_cache: bool = True,
):
    options = 0
    if ignore_actor:
        options |= 0x40
    if use_cache:
        options |= 0x02
    message = (
        START_CODE
        + HARD_PORT
        + dest
        + len(payload).to_bytes(2, "little")
        + options.to_bytes(1, "little")
        + command.to_bytes(1, "little")
        + payload
    )
    while conn.read() != b"\xab":
        conn.write(b"\xaa")
    conn.write(message)


def read_response(conn: serial.Serial) -> Message:
    b = 0
    while b != b"\xaa":
        b = conn.read()
    conn.write(b"\xab")
    while b != b"\xac":
        b = conn.read()
    header = conn.read(10)
    message = Message(header, conn)
    return message


if __name__ == "__main__":
    router()
