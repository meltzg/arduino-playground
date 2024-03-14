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

(def structure-costs {:road       {:wood 1 :brick 1}
                      :settlement {:wood 1 :brick 1 :wheat 1 :sheep 1}
                      :city       {:wheat 2 :stone 3}})

(def VALID-SETTLEMENTS #{0 1})
(def VALID-ROADS #{0 1 2})

(def roads-to-check [[{:neighbor 0 :side 2}
                      {:neighbor 5 :side 1}
                      {:neighbor 5 :side 2}]
                     [{:neighbor 0 :side 2}
                      {:neighbor 2 :side 0}]
                     [{:neighbor 2 :side 0}
                      {:neighbor 3 :side 0}
                      {:neighbor 3 :side 1}]])

(def settlement-roads [[{:neighbor -1 :side 0}
                        {:neighbor 0 :side 2}
                        {:neighbor -1 :side 1}]
                       [{:neighbor -1 :side 1}
                        {:neighbor 2 :side 0}
                        {:neighbor -1 :side 2}]])

(defn weights->items [weights]
  (apply concat (map #(repeat (second %) (first %)) weights)))

(defn weights->selection-order [weights]
  (shuffle (weights->items weights)))

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

(defn get-tile-settlements [{:keys [board]} {:keys [settlements neighbors]}]
  (concat settlements
          (filter #(#{0} (:side %)) (get-in board [(get neighbors 3) :settlements]))
          (filter #(#{0 1} (:side %)) (get-in board [(get neighbors 4) :settlements]))
          (filter #(#{1} (:side %)) (get-in board [(get neighbors 5) :settlements]))))

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
    (when (and (VALID-SETTLEMENTS side)                     ; only side 0 and 1 are valid
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
  ([game-state]
   (get-available-settlements game-state false))
  ([{:keys [board current-player] :as game-state} city?]
   (mapcat
     (fn [[tile-id settlements]]
       (map #(do [tile-id %]) settlements))
     (filter #(seq (second %))
             (map #(do [(:id %)
                        (remove nil? (map (fn [side]
                                            (valid-settlement?
                                              game-state (:id %)
                                              {:player-num current-player
                                               :city?      city?
                                               :side       side}))
                                          (range 2)))])
                  (vals board))))))

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
    (when (and (VALID-ROADS side)
               (not (seq (filter #(= side (:side %)) roads)))
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
                                (map #(get-road game-state (get-in tile [:neighbors (:neighbor %)]) (:side %))
                                     (get roads-to-check side))))))
      road)))

(defn set-road [game-state tile-id road]
  (if (valid-road? game-state tile-id road)
    (update-in game-state [:board tile-id :roads] conj road)
    game-state))

(defn get-available-roads
  [{:keys [board current-player] :as game-state}]
  (mapcat
    (fn [[tile-id roads]]
      (map #(do [tile-id %]) roads))
    (filter #(seq (second %))
            (map #(do [(:id %)
                       (remove nil? (map (fn [side]
                                           (valid-road?
                                             game-state (:id %)
                                             {:player-num current-player
                                              :side       side}))
                                         (range 3)))])
                 (vals board)))))

(defn setup-board [graph num-players]
  {:pre [(pos-int? num-players)]}
  (let [{ocean-tiles true
         land-tiles  false} (group-by #(boolean (some nil? (:neighbors %))) graph)]
    {:current-player -1
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

(defn select-initial-settlement [{:keys [board current-player] :as game-state}]
  (let [num-settlements (count (filter #(= (:player-num %) current-player) (mapcat :settlements (vals board))))
        [tile-id settlement] (rand-nth (get-available-settlements game-state))
        resource-tiles (remove #(#{:ocean :desert} (:type %))
                               (get-settlement-tiles game-state tile-id settlement))]
    (as-> game-state gs
          (set-settlement gs tile-id settlement)
          (apply (partial set-road gs)
                 (rand-nth (filter #(valid-road? gs (first %) (second %))
                                   (map #(do [(if (neg? (:neighbor %))
                                                tile-id
                                                (get-in gs [:board tile-id :neighbors (:neighbor %)]))
                                              {:player-num current-player :side (:side %)}])
                                        (get settlement-roads (:side settlement))))))
          (reduce #(update-in %1 [:player-stats current-player (:type %2)] (if (>= num-settlements 1) inc identity))
                  gs
                  resource-tiles))))

(defn update-setup-phase [{:keys [board num-players setup-phase?] :as game-state}]
  (if (and setup-phase?
           (= (count (mapcat :settlements (vals board))) (* num-players 2)))
    (assoc game-state :setup-phase? false :current-player -1)
    game-state))

(defn update-current-player [{:keys [board setup-phase? num-players] :as game-state}]
  (let [num-settlements (count (mapcat :settlements (vals board)))
        updater (cond (or (not setup-phase?)
                          (< num-settlements num-players)) inc
                      (> num-settlements num-players) dec
                      :else identity)]
    (update game-state :current-player #(mod (updater %) num-players))))

(defn roll-dice [game-state n d]
  (assoc game-state :dice-roll (repeatedly n #(rand-int d))))
(defn collect-resources [{:keys [dice-roll board] :as game-state}]
  (->> board
       vals
       (filter #(and (not (:robber? %))
                     (= (:roll %) (apply + dice-roll))))
       (map #(do [(:type %) (get-tile-settlements game-state %)]))
       (mapcat #(map (fn [settlement] (assoc settlement :type (first %))) (second %)))
       (reduce #(update-in %1 [:player-stats (:player-num %2) (:type %2)] + (if (:city? %2) 2 1)) game-state)))

(defn move-robber [{:keys [board] :as game-state}]
  (let [current-robber-id (->> board
                               vals
                               (filter :robber?)
                               first
                               :id)]
    (-> game-state
        (assoc-in [:board current-robber-id :robber?] false)
        (assoc-in [:board
                   (:id (first (filter #(and (not= (:type %) :ocean)
                                             (not= (:id %) current-robber-id))
                                       (repeatedly #(rand-nth (vals board))))))
                   :robber?] true))))

(defn discard-resources [{:keys [player-stats] :as game-state}]
  (->> player-stats
       (map-indexed vector)
       (filter #(> (apply + (vals (second %))) 7))
       (reduce (fn [gs [player-num resources]]
                 (->> resources
                      weights->selection-order
                      (take (Math/floor (/ (apply + (vals resources)) 2)))
                      (reduce #(update-in %1 [:player-stats player-num %2] dec) gs)))
               game-state)))

(defn handle-roll [{:keys [dice-roll] :as game-state}]
  (if (= (apply + dice-roll) 7)
    (-> game-state
        discard-resources
        move-robber)
    (collect-resources game-state)))

(defn get-available-structures [game-state]
  (into {} (filter (comp seq second) {:road       (get-available-roads game-state)
                                      :settlement (get-available-settlements game-state)
                                      :city       (get-available-settlements game-state true)})))

(defn make-payment [{:keys [current-player player-stats]} structure]
  (loop [player-stats (get player-stats current-player)
         payment {}
         cost (weights->items (structure structure-costs))]
    (let [resource (first cost)]
      (cond (not (seq cost))
            payment
            (< (resource player-stats) 1)
            nil
            :else
            (recur (update player-stats resource dec)
                   (update payment resource (fnil inc 0))
                   (rest cost))))))

(defn affordable-actions [game-state]
  (->> structure-costs
       keys
       (map #(do [% (make-payment game-state %)]))
       (remove #(nil? (second %)))
       (into {})))

(defn build-structure [{:keys [current-player] :as game-state} structure-type [tile-id structure]]
  (println "build" structure-type)
  (let [payment (make-payment game-state structure-type)]
    (reduce #(update-in %1 [:player-stats current-player %2] dec)
            (case structure-type
              (:city :settlement) (set-settlement game-state tile-id structure)
              :road (set-road game-state tile-id structure))
            (weights->items payment))))

(defn take-actions [game-state]
  (loop [game-state game-state]
    (let [possible-actions (affordable-actions game-state)
          available-structures (get-available-structures game-state)
          available-affordable-actions (clojure.set/intersection (set (keys possible-actions))
                                                                 (set (keys available-structures)))
          type-to-build (first (filter available-affordable-actions [:city :settlement :road]))]
      (println possible-actions)
      (if type-to-build
        (recur (-> game-state
                   (build-structure type-to-build (rand-nth (type-to-build available-structures)))))
        game-state))))

(defn do-turn [game-state]
  (cond-> (update-current-player game-state)
          (:setup-phase? game-state) (#(-> %
                                           select-initial-settlement
                                           update-setup-phase))
          (not (:setup-phase? game-state)) (#(-> %
                                                 (roll-dice 2 6)
                                                 handle-roll
                                                 take-actions))))

(defn play-game [game-state]
  (lazy-seq (cons game-state (play-game (do-turn game-state)))))

#_(require '[mindra.core :refer [diagram->svg]]
           '[arduino-playground-client.draw :as d]
           '[arduino-playground-client.hex-graph :as hex]
           '[arduino-playground-client.catan :as c])
#_(map-indexed #(spit (str "game/turn-" (format "%03d" %1) ".svg")
                      (diagram->svg
                        (d/draw-board %2)))
               (take 100 (c/play-game (c/setup-board (hex/create-graph 4 8) 6))))
