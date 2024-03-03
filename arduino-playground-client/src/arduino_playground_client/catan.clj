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
        (remove port-ids ocean-tiles)
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

(defn setup-game [graph num-players]
  {:pre [(pos-int? num-players)]}
  (let [{ocean-tiles true
         land-tiles  false} (group-by #(boolean (some nil? (:neighbors %))) graph)]
    {:current-player 0
     :setup-phase? true
     :board          (concat (setup-land land-tiles)
                             (setup-ocean ocean-tiles land-tiles))}))
