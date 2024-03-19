(ns arduino-playground-client.core
  (:gen-class)
  (:require [mindra.core :refer [diagram->svg]]
            [arduino-playground-client.draw :as d]
            [arduino-playground-client.hex-graph :as hex]
            [arduino-playground-client.catan :as c]))

(defn -main
  "I don't do a whole lot ... yet."
  [& _]
  (loop []
    (let [turns (c/play-game (c/setup-board (hex/create-graph 4 8) 6))
          last-turn (last turns)
          winner-map (-> last-turn
                         (update :board (fn [graph]
                                          (update-vals
                                            graph
                                            (fn [tile]
                                              (update tile :settlements
                                                      (partial filter #(= (:player-num %)
                                                                          (:winner last-turn))))))))
                         (update :board (fn [graph]
                                          (update-vals
                                            graph
                                            (fn [tile]
                                              (update tile :roads
                                                      (partial filter #(= (:player-num %)
                                                                          (:winner last-turn)))))))))]
      (mapv #(do (spit "tile-test.svg" (diagram->svg (d/draw-board %)))
                 (Thread/sleep 750))
            turns)
      (Thread/sleep 2000)
      (spit "tile-test.svg" (diagram->svg (d/draw-board winner-map)))
      (Thread/sleep 10000)
      (recur))))
