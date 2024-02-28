(ns arduino-playground-client.draw
  (:require [arduino-playground-client.hex-graph :as hex]
            [mindra.diagrams :as md]))

(def hex-side-len 5.0)

(defn draw-tile [{tile-id :id :keys [row col]}]
  (md/translate
    (* col hex-side-len (/ (Math/sqrt 3) 2))
    (* row -1.5 hex-side-len)
    (md/superimpose'
      (md/text (str tile-id))
      (md/rotate 30 (md/polygon-regular 6 hex-side-len)))))

(defn draw-board [graph]
  (loop [visited #{}
         queue (conj clojure.lang.PersistentQueue/EMPTY
                     (assoc (first graph) :row 0 :col 0))
         graph-map (into {} (map #(vec [(:id %) %]) graph))
         diags []]
    (if (seq queue)
      (let [{curr-id :id
             :keys   [neighbors]
             :as     tile} (first queue)]
        (recur (conj visited curr-id)
               (pop (apply (partial conj queue)
                           (remove #(or (nil? (:id %))
                                        (visited (:id %))
                                        (some #{(:id %)} (map :id queue)))
                                   (map-indexed #(merge (get graph-map (get neighbors %1)) %2)
                                                (hex/neighbor-coordinates tile)))))
               graph-map
               (conj diags (draw-tile tile))))
      diags)))
