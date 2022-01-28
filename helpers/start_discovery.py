from serial import Serial

from serialcom import Router


with Serial('/dev/ttyUSB0', 9600, timeout=3) as s:
    router = Router(s)
    router.start_discovery()

