# TESTS


## BUF_SCAN

### User
```
$ sudo swapoff -a
$ ./build/buf_scan 100
```

### Kernel buffer test
```
$ sudo swapoff -a
$ make
$ sudo insmod ./driver/test/test_buf.ko
$ sudo chmod a+rw /dev/tbuf
$ ./build/test-buf 100 0 0
```
