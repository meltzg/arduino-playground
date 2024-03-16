(ns arduino-playground-client.draw
  (:require [arduino-playground-client.hex-graph :as hex]
            [mindra.diagrams :as md]))

(def hex-side-len 5.0)

(def land-colors {:ocean  {:r 0x13 :g 0x38 :b 0xbe}
                  :desert {:r 0xd2 :g 0xb4 :b 0x8c}
                  :brick  {:r 0xcb :g 0x41 :b 0x54}
                  :sheep  {:r 0xb0 :g 0xfc :b 0x38}
                  :wood   {:r 0x03 :g 0xac :b 0x13}
                  :stone  {:r 0x80 :g 0x80 :b 0x80}
                  :wheat  {:r 0xff :g 0xff :b 0x00}
                  :wild   {:r 0xff :g 0xff :b 0xff}})

(def player-colors [{:r 0xff :g 0x00 :b 0x00}
                    {:r 0xff :g 0xff :b 0x00}
                    {:r 0x00 :g 0xff :b 0x00}
                    {:r 0x00 :g 0x00 :b 0xff}
                    {:r 0xff :g 0xff :b 0xff}
                    {:r 0x96 :g 0x4b :b 0x00}])

(def active-color {:r 0x66 :g 0xb2 :b 0xb2})

(defn draw-port [{:keys [port]}]
  (when port
    (let [{port-type :type :keys [side]} port
          {:keys [r g b]} (port-type land-colors)
          side-angle (* -1 (/ side 6) 2 Math/PI)
          side-radius (* -1 hex-side-len (/ (Math/sqrt 3) 3))]
      (md/translate
        (* side-radius (Math/cos side-angle))
        (* side-radius (Math/sin side-angle))
        (md/rotate (+ 30 (* 60 side))
                   (md/superimpose'
                     (md/vsep'
                       1
                       (md/text (name port-type))
                       (md/text (str (if (= port-type :wild) 3 2) ":1")))
                     (md/fill-color
                       r g b 255
                       (md/triangle hex-side-len))))))))

(defn draw-robber []
  (let [{:keys [r g b]} (:stone land-colors)]
    (md/fill-color
      r g b 255
      (md/superimpose'
        (md/translate 0 1.5 (md/circle 0.6))
        (md/translate 0 -1.5 (md/rounded-rectangle 2 0.6 0.6))
        (md/ellipse-xy 0.6 1.5)))))

(defn draw-settlement [{:keys [player-num city?]}]
  (let [{:keys [r g b]} (or (get player-colors player-num) active-color)
        settlement-len 1.0
        cross-len (/ settlement-len (Math/sqrt 2))]
    (md/fill-color
      r g b 255
      (md/superimpose'
        (if city?
          (md/translate (/ settlement-len 2) 0 (md/rectangle (* 2 settlement-len) settlement-len))
          (md/square settlement-len))
        (md/translate 0 (/ settlement-len 2) (md/rotate 45 (md/square cross-len)))))))

(defn draw-placed-settlement [{:keys [side] :as settlement}]
  (let [side-angle (* -1 (+ (/ 1.0 12) (/ side 6)) 2 Math/PI)]
    (md/translate
      (* -1 hex-side-len (Math/cos side-angle))
      (* -1 hex-side-len (Math/sin side-angle))
      (draw-settlement settlement))))

(defn draw-settlements [{:keys [settlements]}]
  (md/superimpose (map draw-placed-settlement settlements)))

(defn draw-road [{:keys [side player-num]}]
  (let [{:keys [r g b]} (or (get player-colors player-num) active-color)
        road-width 0.5]
    (md/fill-color
      r g b 255
      (md/rotate
        (* -60 side)
        (md/rectangle road-width (dec hex-side-len))))))

(defn draw-placed-road [{:keys [side] :as road}]
  (let [side-angle (* -1 (/ side 6) 2 Math/PI)
        side-radius (* -1 hex-side-len (/ (Math/sqrt 3) 2))]
    (md/translate
      (* side-radius (Math/cos side-angle))
      (* side-radius (Math/sin side-angle))
      (draw-road road))))

(defn draw-roads [{:keys [roads]}]
  (md/superimpose (map #(draw-placed-road %) roads)))

(defn draw-base [{land-type :type :keys [id roll robber? active?]}]
  (let [{:keys [r g b]} (if land-type (land-type land-colors)
                                      (into {} (map #(vec [% (rand-int 255)]) [:r :g :b])))
        roll-color (if active? active-color (:desert land-colors))]
    (md/superimpose'
      (md/translate 0 2 (md/text (str "ID: " id)))
      (when robber? (draw-robber))
      (when-not (nil? roll)
        (md/superimpose'
          (md/text (format "%02d" roll))
          (md/fill-color
            (:r roll-color)
            (:g roll-color)
            (:b roll-color)
            255
            (md/circle 1))))
      (md/rotate
        30
        (md/fill-color
          r g b 255
          (md/polygon-regular 6 hex-side-len))))))

(defn draw-tile [{:keys [row col] :as tile}]
  (let [x (* col hex-side-len (/ (Math/sqrt 3) 2))
        y (* row -1.5 hex-side-len)]
    {:base   (md/translate
               x y
               (md/superimpose'
                 (draw-port tile)
                 (draw-base tile)))
     :pieces (md/translate
               x y
               (md/superimpose'
                 (draw-settlements tile)
                 (draw-roads tile)))}))

(defn draw-player-stats
  ([game-state]
   (draw-player-stats game-state nil))
  ([{:keys [player-stats board dice-roll]} player-num]
   (let [num-roads (count (mapcat #(filter (fn [{pn :player-num}]
                                             (or (nil? player-num) (= player-num pn)))
                                           (:roads %))
                                  (vals board)))
         num-settlements (count (mapcat #(filter (fn [{pn :player-num :keys [city?]}]
                                                   (and (or (nil? player-num) (= player-num pn))
                                                        (not city?)))
                                                 (:settlements %))
                                        (vals board)))
         num-cities (count (mapcat #(filter (fn [{pn :player-num :keys [city?]}]
                                              (and (or (nil? player-num) (= player-num pn))
                                                   city?))
                                            (:settlements %))
                                   (vals board)))
         {:keys [r g b]} (or (get player-colors player-num) active-color)]
     (md/background-frame
       1
       0xff 0xff 0xff
       (md/vsep' 2
                 (md/hsep' 2
                           (md/background-frame
                             1.5 0xff 0xff 0xff
                             (md/text (str (if player-num "Player " "Game Stats ") player-num)))
                           (md/superimpose'
                             (when (not player-num)
                               (md/text (str (apply + dice-roll))))
                             (md/fill-color r g b
                                            255
                                            (md/polygon-regular 6 1))))
                 (md/hsep' 2
                           (md/vsep 0 (map #(md/superimpose' (md/text (str (get-in player-stats [player-num %])))
                                                             (md/fill-color (-> land-colors % :r)
                                                                            (-> land-colors % :g)
                                                                            (-> land-colors % :b)
                                                                            255
                                                                            (md/polygon-regular 6 1)))
                                           [:brick :sheep :stone :wheat :wood]))
                           (md/vsep' 0.5
                                     (md/hsep' 0.75
                                               (md/scale 1 0.5 (draw-road {:player-num player-num :side 0}))
                                               (md/text (str num-roads)))
                                     (md/hsep' 0.75
                                               (draw-settlement {:player-num player-num})
                                               (md/text (str num-settlements)))
                                     (md/hsep' 0.75
                                               (draw-settlement {:player-num player-num :city? true})
                                               (md/text (str num-cities))))))))))

(defn draw-board [game-state]
  (loop [visited #{}
         queue (conj clojure.lang.PersistentQueue/EMPTY
                     (assoc (-> game-state :board vals first) :row 0 :col 0))
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
                                   (map-indexed #(merge (get-in game-state [:board (get neighbors %1)]) %2)
                                                (hex/neighbor-coordinates tile)))))
               (conj diags (draw-tile (assoc tile :active? (= (:roll tile) (apply + (:dice-roll game-state))))))))
      (md/vsep' 1
                (md/hsep 0 (conj (map (partial draw-player-stats game-state) (range (:num-players game-state)))
                                 (draw-player-stats game-state)))
                (md/superimpose
                  (concat (map :pieces diags)
                          (map :base diags)))))))
