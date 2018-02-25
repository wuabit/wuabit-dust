## Entropy cryptanalysis

Procedure:

* Taking samples generated in test mode and saved in file *entropy.txt*
* Convert from hexadecimal to binary: 

~~~bash
$ cat entropy.txt | xxd -r -p > entropy.bin  
~~~

* Analyze with [ent](http://www.fourmilab.ch/random/):

~~~bash
$ ent entropy.bin > entropy.ent.txt
~~~

# Current results:

[entropy.ent.txt](entropy.ent.txt):

~~~
Entropy = 7.996615 bits per byte.

Optimum compression would reduce the size
of this 53609 byte file by 0 percent.

Chi square distribution for 53609 samples is 251.98, and randomly
would exceed this value 54.16 percent of the times.

Arithmetic mean value of data bytes is 127.1665 (127.5 = random).
Monte Carlo value for Pi is 3.143496754 (error 0.06 percent).
Serial correlation coefficient is 0.000128 (totally uncorrelated = 0.0).
~~~