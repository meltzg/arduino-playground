(ns arduino-playground-client.draw
  (:require [mindra.diagrams :as md]))

(def hex-side-len 5.0)

(defn draw-tile [{tile-id :id :keys [row col]}]
  (md/translate
    (+ (* col hex-side-len (Math/sqrt 3))
       (if (odd? row)
         (* hex-side-len (/ (Math/sqrt 3) 2))
         0))
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
             :keys   [neighbors row col]
             :as     tile} (first queue)]
        (println curr-id neighbors [col row])
        (recur (conj visited curr-id)
               (pop (apply (partial conj queue)
                           (remove #(or (nil? (:id %))
                                        (visited (:id %))
                                        (some #{(:id %)} (map :id queue)))
                                   [(assoc (get graph-map (get neighbors 0))
                                      :row row :col (dec col))
                                    (assoc (get graph-map (get neighbors 1))
                                      :row (dec row) :col (dec col))
                                    (assoc (get graph-map (get neighbors 2))
                                      :row (dec row) :col col)
                                    (assoc (get graph-map (get neighbors 3))
                                      :row row :col (inc col))
                                    (assoc (get graph-map (get neighbors 4))
                                      :row (inc row) :col col)
                                    (assoc (get graph-map (get neighbors 5))
                                      :row (inc row) :col (dec col))])))
               graph-map
               (conj diags (draw-tile tile))))
      diags)))
