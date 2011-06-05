(println (quote (:a :b :c (:d :e :f))))

(println (quote-eval :a))
(println (quote-eval 3x111))
(println)
(println)
(println (str (quote-eval (:a :b (unquote (+ 1 2 3))))))
;; (println (quote-eval (:a (1 (unquote (+ 1 2 3))))))



