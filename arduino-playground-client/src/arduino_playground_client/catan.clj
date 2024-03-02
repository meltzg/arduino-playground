(ns arduino-playground-client.catan
  (:require [arduino-playground-client.utils :refer [take-rand']]))

(def port-weights {:wild  5
                   :brick 1
                   :sheep 2
                   :wood  1
                   :stone 1
                   :wheat 1})

(defn weights->selection-order [weights]
  (shuffle (apply concat (map #(repeat (second %) (first %)) weights))))

(defn setup-land [land-tiles]
  land-tiles)

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
        (map #(-> %1
                  (assoc-in [:port :type] %2)
                  (assoc-in [:port :side] (first (rand-nth (filter (comp land-ids second)
                                                                   (map-indexed list (:neighbors %1)))))))
             ports
             (take (count ports) (weights->selection-order port-weights)))))))

(defn setup-board [graph]
  (let [{ocean-tiles true
         land-tiles  false :as m} (group-by #(boolean (some nil? (:neighbors %))) graph)]
    (concat (setup-land land-tiles)
            (setup-ocean ocean-tiles land-tiles))))
