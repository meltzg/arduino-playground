(ns arduino-playground-client.messaging.catan
  (:require [arduino-playground-client.messaging.router :as r]
            [arduino-playground-client.messaging.serial :as ser]))

(def MODE-ID 2)
(def COMMANDS {:set-state           6
               :play-state-response 7
               :play-state-request  8})
(def EMPTY-TILE (unchecked-byte 0x00))
(def UNOWNED (unchecked-byte 0xff))

(def type->byte {:ocean  1
                 :desert 2
                 :brick  3
                 :sheep  4
                 :wood   5
                 :stone  6
                 :wheat  7})
(def byte->type (into {} (map #(vector (second %) (first %)) type->byte)))

(defn tile-state->bytes [{:keys [id settlements roads port neighbors type robber? roll]} current-player]
  (concat [id
           (type type->byte)
           (get type->byte
                (if (= (:type port) :wild) :desert (:type port))
                0)
           (or (get neighbors (:side port)) EMPTY-TILE)
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
          (replace {nil EMPTY-TILE} neighbors)
          [current-player]))

(defn bytes->tile-state [tile-bytes]
  (let [byte-vec (vec tile-bytes)
        neighbors (subvec byte-vec 17 23)
        road-owners (subvec byte-vec 6 12)
        settlement-owners (subvec byte-vec 12 14)
        cities? (subvec byte-vec 14 16)
        port-type (get byte->type (nth byte-vec 2))]
    {:id          (first byte-vec)
     :type        (get byte->type (second byte-vec))
     :port        (when port-type {:type (if (= port-type :desert) :wild port-type)
                                   :side (.indexOf neighbors (nth byte-vec 3))})
     :roll        (when (pos? (nth byte-vec 4)) (nth byte-vec 4))
     :robber?     (if (zero? (nth byte-vec 5)) false true)
     :roads       (remove nil? (map-indexed #(when (not= %2 UNOWNED) {:side %1 :player-num %2}) road-owners))
     :settlements (remove nil? (map #(when (not= %2 UNOWNED) {:side %1 :player-num %2 :city? (if (zero? %3) false true)})
                                    (range (count settlement-owners)) settlement-owners cities?))
     }))

(defn set-tile!
  ([port tile-state current-player]
   (set-tile! port tile-state, current-player, false))
  ([port tile-state current-player request-ack?]
   (ser/write-message! port
                       {:source  r/PORT_H
                        :dest    (:id tile-state)
                        :payload (concat [MODE-ID
                                          (:set-state COMMANDS)]
                                         (tile-state->bytes tile-state current-player)
                                         [(if request-ack? 1 0)])})
   (when request-ack?
     (-> port
         ser/read-message!
         :payload
         rest
         rest
         bytes->tile-state))))

(defn get-tile-play-state! [port id]
  (ser/write-message! port
                      {:source  r/PORT_H
                       :dest    id
                       :payload [MODE-ID
                                 (:play-state-request COMMANDS)]})
  (-> port
      ser/read-message!
      :payload
      rest
      rest
      bytes->tile-state))

(defn update-board! [port [{previous-board :board} {:keys [current-player] current-board :board}]]
  (let [previous-tileset (set (vals previous-board))
        updated-tiles (remove #(previous-tileset %) (vals current-board))]
    (doall (map #(do (set-tile! port % current-player)
                     (Thread/sleep 500))
                updated-tiles))))
