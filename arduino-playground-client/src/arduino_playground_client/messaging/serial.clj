(ns arduino-playground-client.messaging.serial
  (:import (java.nio ByteBuffer)
           (com.fazecast.jSerialComm SerialPort)
           (java.nio ByteOrder)))

(def PING_BYTE (unchecked-byte 0xAA))
(def ACK_BYTE (unchecked-byte 0xAB))
(def START_CODE (unchecked-byte 0xAC))

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

(defn format-bytes [bytes]
  (map (partial format "0x%02X") bytes))

(defn message->bytes [{:keys [source dest command payload options]
                       :or   {dest 0x00 command 0x00}}]
  (let [payload-length (length->bytes (count payload))]
    (concat [source
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

(defn list-ports []
  (map #(.getSystemPortName %) (SerialPort/getCommPorts)))

(defn open-port! [port]
  (.openPort port)
  port)

(defn close-port! [port]
  (.closePort port))

(defn read-buffer! [port]
  (let [buffer (byte-array (.bytesAvailable port))]
    (.readBytes port buffer (count buffer))
    (String. buffer "UTF-8")))

(defn read-message! [port]
  (let [ping-buffer (byte-array 1 [0x00])
        header-buffer (byte-array 8)]
    (while (not= (first ping-buffer) PING_BYTE)
      (when (> (.readBytes port ping-buffer 1) 0)
        (print (String. ping-buffer "UTF-8"))))
    (.writeBytes port (byte-array 1 [ACK_BYTE]) 1)
    (while (not= (first ping-buffer) START_CODE)
      (.readBytes port ping-buffer 1))
    (while (< (.bytesAvailable port) 8))
    (println "header read" (.readBytes port header-buffer 8))
    (println "header" (format-bytes header-buffer))
    (let [payload-size (get-payload-size header-buffer)
          payload-buffer (byte-array payload-size)]
      (while (< (.bytesAvailable port) payload-size))
      (println "payload read" (.readBytes port payload-buffer payload-size))
      (bytes->message (concat header-buffer payload-buffer)))))

(defn write-message! [port message]
  (let [ping-buffer (byte-array 1, [0x00])
        msg-bytes (message->bytes message)]
    (println "bytes to write" (format-bytes msg-bytes))
    (while (not= (first ping-buffer) ACK_BYTE)
      (.writeBytes port (byte-array 1 [PING_BYTE]) 1)
      (.readBytes port ping-buffer 1))
    (println "recieved ack")
    (.writeBytes port (byte-array 1 [START_CODE]) 1)
    (.writeBytes port (byte-array (count msg-bytes) msg-bytes) (count msg-bytes))
    (println "message sent")))
