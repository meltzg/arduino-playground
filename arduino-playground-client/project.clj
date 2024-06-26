(defproject arduino-playground-client "0.1.0-SNAPSHOT"
  :description "FIXME: write description"
  :url "http://example.com/FIXME"
  :license {:name "EPL-2.0 OR GPL-2.0-or-later WITH Classpath-exception-2.0"
            :url "https://www.eclipse.org/legal/epl-2.0/"}
  :dependencies [[org.clojure/clojure "1.11.2"]
                 [org.clojars.rorokimdim/mindra "0.0.4"]
                 [com.fazecast/jSerialComm "2.10.4"]]
  :plugins [[lein-ancient "0.7.0"]
            [com.github.clj-kondo/lein-clj-kondo "2024.02.12"]]
  :main ^:skip-aot arduino-playground-client.core
  :target-path "target/%s"
  :profiles {:uberjar {:aot :all}}
  :repl-options {:timeout 120000})
