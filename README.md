# TESTS


## BUF_SCAN

### User
```
$ sudo swapoff -a
$ ./build/buf_scan 100
```

### Kernel buffer test

Turn off KASLR: Add `nokaslr` to Linux boot param and reboot.

Get symbol addr of `set_memory_ro` and `set_memory_rw`
```
$ sudo cat /proc/kallsyms |grep set_memory_ro
ffffffff810a51e0 T set_memory_ro

$ sudo cat /proc/kallsyms |grep set_memory_rw
ffffffff810a5210 T set_memory_rw
```

Start scan
```
$ sudo swapoff -a
$ make
$ sudo insmod ./driver/test/test_buf.ko
$ sudo chmod a+rw /dev/tbuf
$ ./build/test-buf start 0xffffffff810a51e0 0xffffffff810a51e0
```

Stop scan
```
$ ./build/test-buf start 0 0
```
