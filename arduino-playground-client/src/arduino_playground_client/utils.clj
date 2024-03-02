(ns arduino-playground-client.utils)

(defn- shuffle'
  [seed coll]
  (let [rng (java.util.Random. seed)
        rnd #(do % (.nextInt rng))]
    (sort-by rnd coll)))

(defn take-rand'
  ([n coll]
   (->> coll shuffle (take n)))
  ([n coll seed]
   (->> coll (shuffle' seed) (take n))))
