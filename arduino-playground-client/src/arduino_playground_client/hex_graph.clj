(ns arduino-playground-client.hex-graph
  (:require [mindra.diagrams :as md]))

(def hex-side-len 5.0)

(defn valid-widths [min-width max-width]
  (< min-width max-width))

(defn num-rows [min-width max-width]
  {:pre [(valid-widths min-width max-width)]}
  (inc (* 2 (- max-width min-width))))


(defn distance-from-max-row [min-width max-width row-num]
  {:pre [(valid-widths min-width max-width)]}
  (Math/abs (- (Math/floor (/ (num-rows min-width max-width) 2))
               row-num)))

(defn row-length [min-width max-width row-num]
  {:pre [(valid-widths min-width max-width)]}
  (- max-width
     (distance-from-max-row min-width max-width row-num)))

(defn num-vertices [min-width max-width]
  {:pre [(valid-widths min-width max-width)]}
  (apply + (map (partial row-length min-width max-width)
                (range (num-rows min-width max-width)))))

(defn id-offset [min-width max-width row-num]
  {:pre [(valid-widths min-width max-width)]}
  (apply + (map #(row-length min-width max-width %) (range 0 row-num))))

(defn vertex-row [min-width max-width vertex-id]
  {:pre [(valid-widths min-width max-width)]}
  (->> (num-rows min-width max-width)
       range
       (map-indexed #(vec [%1 (map (partial + (id-offset min-width max-width %2))
                                   [0, (dec (row-length min-width max-width %2))])]))
       (filter #(<= (first (last %)) vertex-id (last (last %))))
       ffirst))

(defn calculate-neighbors [min-width max-width vertex-id]
  {:pre [(valid-widths min-width max-width)]}
  (let [row-num (vertex-row min-width max-width vertex-id)
        row-offset (- vertex-id (id-offset min-width max-width row-num))
        current-row-len (row-length min-width max-width row-num)
        max-row (Math/floor (/ (num-rows min-width max-width) 2))]
    [(when (> row-offset 0) (dec vertex-id))
     (when (and (> row-num 0)
                (or (> row-offset 0)
                    (> row-num max-row)))
       (+ (id-offset min-width max-width (dec row-num))
          row-offset
          (if (<= row-num max-row) -1 0)))
     (when (and (> row-num 0)
                (or (< row-offset (dec current-row-len))
                    (> row-num max-row)))
       (+ (id-offset min-width max-width (dec row-num))
          row-offset
          (if (<= row-num max-row) 0 1)))
     (when (< row-offset (dec current-row-len)) (inc vertex-id))
     (when (and (< row-num (dec (num-rows min-width max-width)))
                (or (< row-offset (dec current-row-len))
                    (< row-num max-row)))
       (+ (id-offset min-width max-width (inc row-num))
          row-offset
          (if (>= row-num max-row) 0 1)))
     (when (and (< row-num (dec (num-rows min-width max-width)))
                (or (> row-offset 0)
                    (< row-num max-row)))
       (+ (id-offset min-width max-width (inc row-num))
          row-offset
          (if (>= row-num max-row) -1 0)))]))

(defn create-graph [min-width max-width]
  {:pre (valid-widths min-width max-width)}
  (map (fn [vertex-id] {:id        vertex-id
                        :neighbors (mapv #(when-not (nil? %) (int %))
                                         (calculate-neighbors min-width max-width vertex-id))})
       (range (num-vertices min-width max-width))))

(defn diagram-row [min-width max-width row-num]
  {:pre [(valid-widths min-width max-width)]}
  (md/translate (* (distance-from-max-row min-width max-width row-num) hex-side-len (/ (Math/sqrt 3) 2))
                0
                (md/hsep 0 (repeat (row-length min-width max-width row-num) (md/rotate 30 (md/polygon-regular 6 hex-side-len))))))

(defn diagram-hex-grid [min-width max-width]
  (md/vsep (/ hex-side-len -2)
           (map (partial diagram-row min-width max-width) (range (num-rows min-width max-width)))))
