PORT = /dev/ttyUSB0
ISP_PORT = /dev/ttyACM0

compile-catan:
	arduino-cli compile -b arduino:avr:uno catan_node/ -v

upload-catan: compile-catan
	arduino-cli upload -p ${PORT} -b arduino:avr:uno catan_node/ -v

compile-router:
	arduino-cli compile -b arduino:avr:uno router/ -v

upload-router: compile-router
	arduino-cli upload -p ${PORT} -b arduino:avr:uno router/ -v

compile-pathfinder:
	arduino-cli compile -b arduino:avr:uno pathfinder-i2c/ -v

upload-pathfinder: compile-pathfinder
	arduino-cli upload -p ${PORT} -b arduino:avr:uno pathfinder-i2c/ -v

compile-pathfinder-tester:
	arduino-cli compile -b arduino:avr:uno pathfinder_tester/ -v

upload-pathfinder-tester: compile-pathfinder-tester
	arduino-cli upload -p ${PORT} -b arduino:avr:uno pathfinder_tester/ -v

compile-eeprom-clear:
	arduino-cli compile -b arduino:avr:uno eeprom-clear/ -v

upload-eeprom-clear: compile-eeprom-clear
	arduino-cli upload -p ${PORT} -b arduino:avr:uno eeprom-clear/ -v

burn-bootloader:
	arduino-cli burn-bootloader -b arduino:avr:uno -P arduinoasisp -p ${ISP_PORT} -v
