(ns arduino-playground-client.core
  (:gen-class)
  (:require [arduino-playground-client.catan :as c]
            [arduino-playground-client.messaging.catan :as cmsg]
            [arduino-playground-client.messaging.router :as r]
            [arduino-playground-client.messaging.serial :as ser]
            [clojure.edn :as edn]
            [clojure.java.io :as io]))

(defn -main
  "I don't do a whole lot ... yet."
  [port & _]
  (let [port (ser/open-port! (ser/get-port port))
        topology (if (.exists (io/file "topology.edn"))
                   (edn/read-string (slurp "topology.edn"))
                   (r/get-topology! port))]
    (Thread/sleep 5000)
    (let [turns (c/play-game (c/setup-board topology 6))
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
      ;(doall (map #(do (r/wake-node! port (:id %))
      ;                 (Thread/sleep 1000))
      ;            (-> turns first :board vals)))
      (doall (map #(do (cmsg/set-tile! port % 0 true))
                  (-> turns first :board vals)))
      ;(doall (map (partial cmsg/update-board! port) (partition 2 1 turns)))
      ;(Thread/sleep 5000)
      ;(cmsg/update-board! port [last-turn winner-map])
      )
    (ser/close-port! port)))
