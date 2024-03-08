(ns arduino-playground-client.catan
  (:require [arduino-playground-client.utils :refer [take-rand']]))

(def port-weights {:wild  5
                   :brick 1
                   :sheep 2
                   :wood  1
                   :stone 1
                   :wheat 1})

(def land-weights {:desert 2
                   :brick  5
                   :sheep  6
                   :wood   6
                   :stone  5
                   :wheat  6})

(def roads-to-check [[{:neighbor 0 :side 2}
                      {:neighbor 5 :side 1}
                      {:neighbor 5 :side 2}]
                     [{:neighbor 0 :side 2}
                      {:neighbor 2 :side 0}]
                     [{:neighbor 2 :side 0}
                      {:neighbor 3 :side 0}
                      {:neighbor 3 :side 1}]])

(defn weights->selection-order [weights]
  (shuffle (apply concat (map #(repeat (second %) (first %)) weights))))

(defn setup-land [land-tiles]
  (loop [remaining-tiles land-tiles
         current-weights land-weights
         final-lands '()
         robber-assigned? false]
    (if (seq remaining-tiles)
      (let [land-type (rand-nth (weights->selection-order current-weights))
            updated-weights (update current-weights land-type dec)]
        (recur (rest remaining-tiles)
               (if (zero? (apply + (vals updated-weights)))
                 land-weights
                 updated-weights)
               (conj final-lands
                     (-> remaining-tiles
                         first
                         (assoc :type land-type
                                :robber? (and (not robber-assigned?) (= land-type :desert))
                                :roll (when-not
                                        (= land-type :desert)
                                        (first
                                          (filter #(not= 7 %)
                                                  (repeatedly #(+ 2 (apply + (repeatedly 2 (partial rand-int 6)))))))))))
               (or robber-assigned? (= land-type :desert))))
      final-lands)))

(defn num-ports [num-ocean num-land]
  (min (/ num-ocean 3) (+ 9 (* 2 (/ num-land 30)))))

(defn setup-ocean [ocean-tiles land-tiles]
  (let [land-ids (set (map :id land-tiles))
        num-ports (num-ports (count ocean-tiles) (count land-tiles))
        ports (take-rand' num-ports
                          (filter #(some land-ids (:neighbors %)) ocean-tiles))
        port-ids (set (map :id ports))]
    (map
      #(assoc % :type :ocean)
      (concat
        (remove #(port-ids (:id %)) ocean-tiles)
        (loop [remaining-ports ports
               current-weights port-weights
               final-ports '()]
          (if (seq remaining-ports)
            (let [port-type (rand-nth (weights->selection-order current-weights))
                  updated-weights (update current-weights port-type dec)]
              (recur (rest remaining-ports)
                     (if (zero? (apply + (vals updated-weights)))
                       port-weights
                       updated-weights)
                     (conj final-ports
                           (-> remaining-ports
                               first
                               (assoc-in [:port :type] port-type)
                               (assoc-in [:port :side]
                                         (first (rand-nth (filter (comp land-ids second)
                                                                  (map-indexed list (:neighbors (first remaining-ports)))))))))))
            final-ports))))))

(defn get-settlement [{:keys [board]} tile-id side]
  (->> (get-in board [tile-id :settlements])
       (filter #(= side (:side %)))
       first))

(defn get-settlement-tiles [{:keys [board]} tile-id {:keys [side]}]
  [(get board tile-id)
   (get board (get (:neighbors (get board tile-id)) side))
   (get board (get (:neighbors (get board tile-id)) (inc side)))])

(defn valid-settlement?
  "Validates settlement/city placement. Only positions 0 and 1 are valid to simplify logic"
  [{:keys [setup-phase? board] :as game-state} tile-id {:keys [player-num city? side] :as settlement}]
  (let [tile (get board tile-id)
        current-settlement (get-settlement game-state tile-id side)
        neighboring-settlements [(get-settlement game-state tile-id (mod (inc side) 2))
                                 (get-settlement game-state
                                                 (get (:neighbors tile) (+ (* side -3) 5))
                                                 (mod (inc side) 2))
                                 (get-settlement game-state
                                                 (get (:neighbors tile) (* side 3))
                                                 (mod (inc side) 2))]
        possible-land-tiles (get-settlement-tiles game-state tile-id settlement)]
    (when (and (#{0 1} side)                                ; only side 0 and 1 are valid
               (if city?
                 (and (= (:player-num current-settlement) player-num) ; settlement must be owned by the player-num to be a city
                      (false? (boolean (:city? current-settlement)))) ; settlement can't already be city
                 (not current-settlement))                  ; settlement can't already be claimed
               (every? nil? neighboring-settlements)        ; no immediate neighbors
               (some #(and % (not= :ocean (:type %))) possible-land-tiles) ; one of the relevant tiles is land
               (or setup-phase?                             ; setup phase does not require roads
                   false))                                  ; settlement needs to be connected to a valid road
      settlement)))

(defn set-settlement [game-state tile-id settlement]
  (if (valid-settlement? game-state tile-id settlement)
    (update-in game-state [:board tile-id :settlements] conj settlement)
    game-state))

(defn get-available-settlements
  ([game-state player-num]
   (get-available-settlements game-state player-num false))
  ([game-state player-num city?]
   (mapcat
     (fn [[tile-id settlements]]
       (map #(do [tile-id %]) settlements))
     (filter #(seq (second %))
             (map #(do [(:id %)
                        (remove nil? (map (fn [side]
                                            (valid-settlement?
                                              game-state (:id %)
                                              {:player-num player-num
                                               :city?      city?
                                               :side       side}))
                                          (range 2)))])
                  (-> game-state :board vals))))))

(defn settlement->neighbor
  "Given a city side number [0-5], return the neighbor the city is on since we only use side 0 and 1. -1 for on tile"
  [side]
  (case side
    (0 1) -1
    2 3
    (3 4) 4
    5 5))

(defn get-road [{:keys [board]} tile-id side]
  (->> (get-in board [tile-id :roads])
       (filter #(= side (:side %)))
       first))

(defn valid-road?
  "Validates road placement. Only positions 0, 1, and 2 are valid to simplify logic"
  [{:keys [board] :as game-state} tile-id {:keys [player-num side] :as road}]
  (let [{:keys [roads] :as tile} (get board tile-id)
        settlement-behind (mod (+ side 5) 6)
        settlement-behind-neighbor (settlement->neighbor settlement-behind)
        settlement-ahead (mod side 6)
        settlement-ahead-neighbor (settlement->neighbor settlement-ahead)]
    (when (and (#{0 1 2} side)
               (or (not= :ocean (:type tile))
                   (not= :ocean (:type (get board (get (:neighbors tile) side))))) ; the road is on land
               (or (seq (filter #(and (= player-num (:player-num %))
                                      (#{(mod (inc side) 6) (mod (dec side) 6)} (:side %)))
                                roads))                     ; neighboring roads on this tile are owned by the player
                   (= player-num (:player-num (get-settlement
                                                game-state
                                                (if (neg? settlement-behind-neighbor) tile-id (get (:neighbors tile) settlement-behind-neighbor))
                                                (mod settlement-behind 2)))) ; the settlement behind is owned by the player
                   (= player-num (:player-num (get-settlement
                                                game-state
                                                (if (neg? settlement-ahead-neighbor) tile-id (get (:neighbors tile) settlement-ahead-neighbor))
                                                (mod settlement-ahead 2)))) ; the settlement ahead is owned by the player
                   (seq (filter #(= player-num (:player-num %)) ; a relevant road is owned by the player on a neighboring tile
                                (map #(get-road board (get-in tile [:neighbors (:neighbor %)]) (:side %))
                                     (get roads-to-check side))))))
      road)))

(defn set-road [game-state tile-id road]
  (if (valid-road? game-state tile-id road)
    (update-in game-state [:board tile-id :roads] conj road)
    game-state))

(defn get-available-roads
  [game-state player-num]
  (mapcat
    (fn [[tile-id roads]]
      (map #(do [tile-id %]) roads))
    (filter #(seq (second %))
            (map #(do [(:id %)
                       (remove nil? (map (fn [side]
                                           (valid-road?
                                             game-state (:id %)
                                             {:player-num player-num
                                              :side       side}))
                                         (range 3)))])
                 (-> game-state :board vals)))))

(defn setup-board [graph num-players]
  {:pre [(pos-int? num-players)]}
  (let [{ocean-tiles true
         land-tiles  false} (group-by #(boolean (some nil? (:neighbors %))) graph)]
    {:current-player 0
     :num-players    num-players
     :setup-phase?   true
     :player-stats   (vec (repeat num-players {:brick 0
                                               :sheep 0
                                               :stone 0
                                               :wheat 0
                                               :wood  0}))
     :board          (into {} (map #(do [(:id %) %])
                                   (concat (setup-land land-tiles)
                                           (setup-ocean ocean-tiles land-tiles))))}))

(defn select-initial-settlements [{:keys [num-players] :as game-state}]
  (as-> game-state gs
        (reduce
          (fn [state player-num]
            (apply (partial set-settlement state)
                   (rand-nth (get-available-settlements state player-num))))
          gs
          (range num-players))
        (reduce
          (fn [state player-num]
            (apply (partial set-road state)
                   (rand-nth (get-available-roads state player-num))))
          gs
          (range num-players))
        (reduce
          (fn [state player-num]
            (let [[tile-id settlement] (rand-nth (get-available-settlements state player-num))
                  resource-tiles (remove #(#{:ocean :desert} (:type %))
                                         (get-settlement-tiles state tile-id settlement))]
              (reduce #(update-in %1 [:player-stats player-num (:type %2)] inc)
                      (set-settlement state tile-id settlement) resource-tiles)))
          gs
          (range (dec num-players) -1 -1))
        (reduce
          (fn [state player-num]
            (apply (partial set-road state)
                   (rand-nth (get-available-roads state player-num))))
          gs
          (range num-players))
        (assoc gs :setup-phase? false)))
