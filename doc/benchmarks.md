# Benchmarks

##### Coremark

- NEMU

```
Running CoreMark for 1000 iterations
2K performance run parameters for coremark.
CoreMark Size    : 666
Total time (ms)  : 46644
Iterations       : 1000
Compiler version : GCC11.4.0
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0xd340
Finised in 46644 ms.
==================================================
CoreMark PASS       62 Marks
                vs. 100000 Marks (i7-7700K @ 4.20GHz)
```

- native(WSL):

```
Running CoreMark for 1000 iterations
2K performance run parameters for coremark.
CoreMark Size    : 666
Total time (ms)  : 115
Iterations       : 1000
Compiler version : GCC11.4.0
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0xd340
Finised in 115 ms.
==================================================
CoreMark PASS       25403 Marks
                vs. 100000 Marks (i7-7700K @ 4.20GHz)
```

##### Dhrystone

- NEMU:

```
Dhrystone Benchmark, Version C, Version 2.2
Trying 500000 runs through Dhrystone.
Finished in 24078 ms
==================================================
Dhrystone PASS         36 Marks
                   vs. 100000 Marks (i7-7700K @ 4.20GHz)
```

- native(WSL):

```
Dhrystone Benchmark, Version C, Version 2.2
Trying 500000 runs through Dhrystone.
Finished in 13 ms
==================================================
Dhrystone PASS         67761 Marks
                   vs. 100000 Marks (i7-7700K @ 4.20GHz)
```

##### microbench

- NEMU

```
======= Running MicroBench [input *ref*] =======
[qsort] Quick sort: * Passed.
  min time: 2580.760 ms [170]
[queen] Queen placement: * Passed.
  min time: 2467.581 ms [164]
[bf] Brainf**k interpreter: * Passed.
  min time: 20913.219 ms [80]
[fib] Fibonacci number: * Passed.
  min time: 215854.155 ms [9]
[sieve] Eratosthenes sieve: * Passed.
  min time: 65713.2525 ms [52]
[15pz] A* 15-puzzle search: * Passed.
  min time: 8883.97125 ms [60]
[dinic] Dinic's maxflow algorithm: * Passed.
  min time: 4701.17425 ms [174]
[lzip] Lzip compression: * Passed.
  min time: 9987.17625 ms [68]
[ssort] Suffix sort: * Passed.
  min time: 2475.91025 ms [161]
[md5] MD5 digest: * Passed.
  min time: 32783.8285 ms [46]
==================================================
MicroBench PASS        98 Marks
                   vs. 100000 Marks (i9-9900K @ 3.60GHz)
Scored time: 366361.026 ms
Total  time: 387636.084 ms
```

- native

```
======= Running MicroBench [input *ref*] =======
[qsort] Quick sort: * Passed.
  min time: 8.190 ms [53772]
[queen] Queen placement: * Passed.
  min time: 7.197 ms [56537]
[bf] Brainf**k interpreter: * Passed.
  min time: 32.151 ms [52300]
[fib] Fibonacci number: * Passed.
  min time: 197.337 ms [10220]
[sieve] Eratosthenes sieve: * Passed.
  min time: 71.106 ms [48973]
[15pz] A* 15-puzzle search: * Passed.
  min time: 16.622 ms [32246]
[dinic] Dinic's maxflow algorithm: * Passed.
  min time: 13.018 ms [62851]
[lzip] Lzip compression: * Passed.
  min time: 17.149 ms [39623]
[ssort] Suffix sort: * Passed.
  min time: 6.846 ms [58457]
[md5] MD5 digest: * Passed.
  min time: 32.832 ms [46293]
==================================================
MicroBench PASS        46127 Marks
                   vs. 100000 Marks (i9-9900K @ 3.60GHz)
Scored time: 402.448 ms
Total  time: 431.743 ms
```