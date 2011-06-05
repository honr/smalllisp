;; (ns example/html)

(println
 (html
  "<!DOCTYPE html>"
  (`
   (:html (:LANG :en-US :dir :ltr)
	  (:head ())
	  (:body ()
		 (:a (:href "http://planet.gnome.org")
		     (:b () "planet gnome")
		     (~ (str (range 10)))))))
  ))
