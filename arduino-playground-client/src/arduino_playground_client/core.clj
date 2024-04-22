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
    (doall (map #(do (r/wake-node! port (:id %))
                     (Thread/sleep 2000))
                topology))
    (loop []
      (let [playthrough (c/play-game (c/setup-board topology 6))
            winner (-> playthrough
                       last
                       :current-player)
            winner-tile (->> playthrough
                             last
                             :board
                             vals
                             (filter #(every? nil? (take 3 (:neighbors %))))
                             first)]
        (doall (map #(do (cmsg/set-tile! port % 0 true))
                    (-> playthrough first :board vals)))
        (doall (map (partial cmsg/update-board! port) (partition 2 1 playthrough)))
        (cmsg/set-tile! port
                        (assoc winner-tile :settlements
                                           (map #(do {:player-num winner :city? true :side %}) (range 2)))
                        winner
                        true)
        (Thread/sleep 2000))
      (recur))
    (ser/close-port! port)))
