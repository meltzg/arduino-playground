(ns arduino-playground-client.core
  (:gen-class))

(defn num-rows [min-width max-width]
  (inc (* 2 (- max-width min-width))))

(defn distance-from-max-row [min-width max-width row-num]
  (Math/abs (- (Math/floor (/ (num-rows min-width max-width) 2))
               row-num)))

(defn row-length [min-width max-width row-num]
  (- max-width
     (distance-from-max-row min-width max-width row-num)))

(defn id-offset [min-width max-width row-num]
  (apply + (map #(row-length min-width max-width %) (range 0 row-num))))

(defn vertex-row [min-width max-width vertex-id]
  (->> (num-rows min-width max-width)
       range
       (map-indexed #(vec [%1 (map (partial + (id-offset min-width max-width %2))
                                   [0, (dec (row-length min-width max-width %2))])]))
       (filter #(<= (first (last %)) vertex-id (last (last %))))
       ffirst))

(defn calculate-neighbors [min-width max-width vertex-id]
  (let [row-num (vertex-row min-width max-width vertex-id)
        row-offset (- vertex-id (id-offset min-width max-width row-num))
        current-row-len (row-length min-width max-width row-num)
        increasing? (or (= row-num 0)
                        (and (> current-row-len (row-length min-width max-width (dec row-num)))
                             (not (zero? (- current-row-len max-width)))))]
    [(when (> row-offset 0) (dec vertex-id))
     (when (and (pos? row-offset) (> row-num 0))
       (+ row-offset
          (id-offset min-width max-width (dec row-num))
          (if increasing? -1 0)))
     (when (and (not= row-offset (dec current-row-len)) (> row-num 0))
       (+ row-offset
          (id-offset min-width max-width (dec row-num))
          (if increasing? 0 1)))
     (when (< row-offset (dec current-row-len)) (inc vertex-id))
     (when (or (< current-row-len (row-length min-width max-width (inc row-num)))
               (not= row-offset (dec current-row-len)))
       (+ row-offset
          (id-offset min-width max-width (inc row-num))
          (if increasing? 1 0)))
     (when (or (< current-row-len (row-length min-width max-width (inc row-num)))
               (pos? row-offset))
       (+ row-offset
          (id-offset min-width max-width (inc row-num))
          (if increasing? 0 -1)))]))

(defn draw-row [min-width max-width row-num]
  (str (apply str (repeat (distance-from-max-row min-width max-width row-num) "  "))
       (apply str (map #(format "%02d  " (int (+ % (id-offset min-width max-width row-num))))
                       (range (row-length min-width max-width row-num))))))

(defn -main
  "I don't do a whole lot ... yet."
  [& _]
  (println "Hello, World!"))
