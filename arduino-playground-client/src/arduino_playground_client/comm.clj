(ns arduino-playground-client.comm
  (:import [java.nio ByteBuffer]
           [java.nio ByteOrder]
           [com.fazecast.jSerialComm SerialPort]))

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

(defn length->bytes [length]
  (-> (ByteBuffer/allocate 4)
      (.order ByteOrder/LITTLE_ENDIAN)
      (.putInt length)
      .array
      vec
      (#(take 2 %))))

(defn bytes->length [[low high]]
  (-> (ByteBuffer/allocate 4)
      (.order ByteOrder/LITTLE_ENDIAN)
      (.put (byte-array 2 [low high]))
      (.rewind)
      (.getInt)))

(defn message->bytes [{:keys [dest command payload ignore-actor? use-cache?]
                       :or   {dest 0x00 command 0x00}}]
  (let [options (unchecked-byte (cond->> 0x00
                                         ignore-actor? (bit-or ROUTER_SYS_COMMAND)
                                         use-cache? (bit-or ROUTER_USE_CACHE)))
        payload-length (length->bytes (count payload))]
    (println payload-length)
    (concat [PORT_H
             dest]
            payload-length
            [options
             command
             0x00
             0x00]
            payload)))

(defn bytes->message [[source dest _ _ options command _ _ & payload]]
  {:source  source
   :dest    dest
   :command command
   :options options
   :payload payload})

(defn get-payload-size [[_ _ low high & _]]
  (bytes->length [low high]))

(defn get-port [path]
  (SerialPort/getCommPort path))

(defn open-port! [port]
  (.openPort port)
  port)

(defn close-port! [port]
  (.closePort port))

(defn read-message! [port]
  (let [ping-buffer (byte-array 1 [0x00])
        header-buffer (byte-array 8)]
    (while (not= (first ping-buffer) PING_BYTE)
      (.readBytes port ping-buffer 1))
    (.writeBytes port (byte-array 1 [ACK_BYTE]) 1)
    (while (not= (first ping-buffer) START_CODE)
      (.readBytes port ping-buffer 1))
    (.readBytes port header-buffer 8)
    (let [payload-size (get-payload-size header-buffer)
          payload-buffer (byte-array payload-size)]
      (.readBytes port payload-buffer payload-size)
      (bytes->message (concat header-buffer payload-buffer)))))
(defn write-message! [port message]
  (let [ping-buffer (byte-array 1, [0x00])
        msg-bytes (message->bytes message)]
    (while (not= (first ping-buffer) ACK_BYTE)
      (println "ping")
      (.writeBytes port (byte-array 1 [PING_BYTE]) 1)
      (.readBytes port ping-buffer 1))
    (println "recieved ack")
    (.writeBytes port (byte-array 1 [START_CODE]) 1)
    (.writeBytes port (byte-array (count msg-bytes) msg-bytes) (count msg-bytes))
    (println "message sent")))