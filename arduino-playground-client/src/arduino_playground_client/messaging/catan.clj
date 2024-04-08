(ns arduino-playground-client.messaging.catan
  (:require [arduino-playground-client.messaging.router :as r]
            [arduino-playground-client.messaging.serial :as ser]))

(def MODE-ID 2)
(def COMMANDS {:set-state   6
               :acknowledge 7})
(def EMPTY-NODE (unchecked-byte 0x00))
(def UNOWNED 0xff)

(defn type->byte [type]
  (case type
    :ocean 1
    :desert 2
    :brick 3
    :sheep 4
    :wood 5
    :stone 6
    :wheat 7
    0))

(defn tile-state->bytes [{:keys [id settlements roads port neighbors type robber? roll]} current-player]
  (concat [id
           (type->byte type)
           (type->byte (if (= (:type port) :wild) :desert (:type port)))
           (or (get neighbors (:side port)) EMPTY-NODE)
           (or roll 0)
           (if robber? 1 0)]
          (map #(or (:player-num (first (filter (comp (partial = %) :side) roads)))
                    UNOWNED)
               (range 6))
          (map #(or (:player-num (first (filter (comp (partial = %) :side) settlements)))
                    UNOWNED)
               (range 2))
          (map #(if (:city? (first (filter (comp (partial = %) :side) settlements)))
                  1
                  0)
               (range 2))
          [1]
          (replace {nil EMPTY-NODE} neighbors)
          [current-player]))

(defn set-tile! [port tile-state current-player]
  (ser/write-message! port
                      {:source  r/PORT_H
                       :dest    (:id tile-state)
                       :payload (concat [MODE-ID
                                         (:set-state COMMANDS)]
                                        (tile-state->bytes tile-state current-player)
                                        [0])}))
