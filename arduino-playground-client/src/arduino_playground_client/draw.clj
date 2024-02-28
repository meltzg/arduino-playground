(ns arduino-playground-client.draw
  (:require [arduino-playground-client.hex-graph :as hex]
            [mindra.diagrams :as md]))

(def hex-side-len 5.0)

(def colors {:ocean  {:r 0x13 :g 0x38 :b 0xbe}
             :desert {:r 0xd2 :g 0xb4 :b 0x8c}
             :brick  {:r 0xcb :g 0x41 :b 0x54}
             :sheep  {:r 0xb0 :g 0xfc :b 0x38}
             :wood   {:r 0x03 :g 0xac :b 0x13}
             :stone  {:r 0x80 :g 0x80 :b 0x80}
             :wheat  {:r 0xff :g 0xff :b 0x00}
             :wild   {:r 0xff :g 0xff :b 0xff}})

(defn draw-tile [{land-type :type :keys [row col roll robber?]}]
  (let [{:keys [r g b]} (if land-type (land-type colors)
                                      (into {} (map #(vec [% (rand-int 255)]) [:r :g :b])))]
    (md/translate
      (* col hex-side-len (/ (Math/sqrt 3) 2))
      (* row -1.5 hex-side-len)
      (md/superimpose'
        (md/text (cond
                   robber? "Robber"
                   (nil? roll) ""
                   :else (format "%02d" roll)))
        (md/rotate
          30
          (md/fill-color
            r g b 255
            (md/polygon-regular 6 hex-side-len)))))))

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
