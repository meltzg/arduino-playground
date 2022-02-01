PORT = /dev/ttyUSB0
ISP_PORT = /dev/ttyACM0
EEPROM_OFFSET = 'sizeof(NodeId_t)'
NODE_ID = 0

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

compile-controller-check:
	arduino-cli compile -b arduino:avr:uno controller-check/ -v

upload-controller-check: compile-controller-check
	arduino-cli upload -p ${PORT} -b arduino:avr:uno controller-check/ -v

compile-eeprom-clear:
	arduino-cli compile -b arduino:avr:uno eeprom-clear/ --build-property compiler.cpp.extra_flags="-DEEPROM_OFFSET=${EEPROM_OFFSET}" -v

upload-eeprom-clear: compile-eeprom-clear
	arduino-cli upload -p ${PORT} -b arduino:avr:uno eeprom-clear/ -v

compile-set-id:
	arduino-cli compile -b arduino:avr:uno set-id/ --build-property compiler.cpp.extra_flags="-DMY_ID=${NODE_ID}" -v

upload-set-id: compile-set-id
	arduino-cli upload -p ${PORT} -b arduino:avr:uno set-id/ -v

eeprom-clear: upload-eeprom-clear
	python tools/sketch_complete_wait.py --port ${PORT}

set-id: upload-set-id
	python tools/sketch_complete_wait.py --port ${PORT}

burn-bootloader:
	arduino-cli burn-bootloader -b arduino:avr:uno -P arduinoasisp -p ${ISP_PORT} -v
