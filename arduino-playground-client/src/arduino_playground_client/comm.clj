(ns arduino-playground-client.comm)

(def BAUD 9600)
(def PING_BYTE (unchecked-byte 0xAA))
(def ACK_BYTE (unchecked-byte 0xAB))
(def START_CODE (unchecked-byte 0xAC))

;; Hardware serial has a special case address
(def PORT_H (unchecked-byte 0xff))
(def Empty (unchecked-byte 0x00))

;; System options
(def ROUTER_SYS_COMMAND (unchecked-byte 0x01))
(def ROUTER_USE_CACHE (unchecked-byte 0x02))
(def ROUTER_BROADCAST (unchecked-byte 0x04))
(def ROUTER_CLEAR_TOPOLOGY (unchecked-byte 0x08))
(def ROUTER_ENABLE_DISCOVERY_UPDATES (unchecked-byte 0x10))

;; System commands
(def ROUTER_GET_ID (unchecked-byte 0x01))
(def ROUTER_RESPONSE_ID (unchecked-byte 0x02))
(def ROUTER_GET_NEIGHBORS (unchecked-byte 0x03))
(def ROUTER_ADD_NODE (unchecked-byte 0x04))
(def ROUTER_START_DISCOVERY (unchecked-byte 0x05))
(def ROUTER_GET_DISCOVERY_STATUS (unchecked-byte 0x06))
(def ROUTER_RESPONSE_DISCOVERY_STATUS (unchecked-byte 0x07))
