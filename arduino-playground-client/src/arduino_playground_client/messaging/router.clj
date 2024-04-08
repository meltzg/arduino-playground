(ns arduino-playground-client.messaging.router
  (:require [arduino-playground-client.messaging.serial :as ser]))

;; Hardware serial has a special case address
(def PORT_H (unchecked-byte 0xff))
(def EMPTY (unchecked-byte 0x00))

;; System options
(def ROUTER_SYS_COMMAND (unchecked-byte 0x01))
(def ROUTER_USE_CACHE (unchecked-byte 0x02))
(def ROUTER_BROADCAST (unchecked-byte 0x04))
(def ROUTER_CLEAR_TOPOLOGY (unchecked-byte 0x08))
(def ROUTER_ENABLE_DISCOVERY_UPDATES (unchecked-byte 0x10))
(def ROUTER_HARDWARE_PROXY_REQUEST (unchecked-byte 0x20))

;; System commands
(def ROUTER_GET_ID (unchecked-byte 0x01))
(def ROUTER_RESPONSE_ID (unchecked-byte 0x02))
(def ROUTER_GET_NEIGHBORS (unchecked-byte 0x03))
(def ROUTER_ADD_NODE (unchecked-byte 0x04))
(def ROUTER_START_DISCOVERY (unchecked-byte 0x05))
(def ROUTER_GET_DISCOVERY_STATUS (unchecked-byte 0x06))
(def ROUTER_RESPONSE_DISCOVERY_STATUS (unchecked-byte 0x07))
(def ROUTER_GET_NEIGHBOR_TOPOLOGY (unchecked-byte 0x08))

(defn parse-discovery-stats [bytes]
  (let [discovery-complete (pos? (first bytes))
        [nodes edges] (map ser/bytes->length (partition 2 (rest bytes)))]
    {:discovery-complete? discovery-complete
     :nodes               nodes
     :edges               edges}))

(defn get-id! [port]
  (ser/write-message! port {:source  PORT_H
                            :dest    PORT_H
                            :command ROUTER_GET_ID
                            :options (bit-or ROUTER_SYS_COMMAND ROUTER_USE_CACHE)})
  (-> port
      ser/read-message!
      :payload
      first))

(defn get-neighbors!
  ([port node-id]
   (get-neighbors! port node-id true))
  ([port node-id use-cache?]
   (ser/write-message! port {:source  PORT_H
                             :dest    node-id
                             :command ROUTER_GET_NEIGHBORS
                             :options (bit-or ROUTER_SYS_COMMAND (if use-cache? ROUTER_USE_CACHE 0x00))})
   (->> port
        ser/read-message!
        :payload
        rest
        (map #(if (zero? %) nil %)))))

(defn get-neighbor-topology!
  ([port node-id]
   (get-neighbor-topology! port node-id true))
  ([port node-id use-cache?]
   (ser/write-message! port {:source  PORT_H
                             :dest    node-id
                             :command ROUTER_GET_NEIGHBOR_TOPOLOGY
                             :options (bit-or ROUTER_SYS_COMMAND (if use-cache? ROUTER_USE_CACHE 0x00))})
   (->> port
        ser/read-message!
        :payload
        rest
        (map #(if (zero? %) nil %)))))

(defn get-discovery-stats!
  ([port]
   (get-discovery-stats! port (get-id! port)))
  ([port node-id]
   (ser/write-message! port {:source  PORT_H
                             :dest    node-id
                             :command ROUTER_GET_DISCOVERY_STATUS
                             :options ROUTER_SYS_COMMAND})
   (->> port
        ser/read-message!
        :payload
        parse-discovery-stats)))

(defn get-topology! [port]
  (loop [visited #{}
         queue (conj clojure.lang.PersistentQueue/EMPTY
                     (get-id! port))
         graph '()]
    (if (seq queue)
      (let [curr-id (first queue)
            neighbors (get-neighbor-topology! port curr-id)]
        (println "node:" (ser/format-bytes [curr-id]) "neighbors:" (ser/format-bytes neighbors))
        (recur (conj visited curr-id)
               (pop (apply (partial conj queue)
                           (remove #(or (nil? %)
                                        (visited %)
                                        (some #{%} queue))
                                   neighbors)))
               (conj graph {:id        (int curr-id)
                            :neighbors (mapv #(when (some? %) (int %))
                                             neighbors)})))
      graph)))
