# TESTS


## BUF_SCAN

### User
```
$ sudo swapoff -a
$ ./build/buf_scan 100
```

### Kernel
```
$ sudo swapoff -a
$ sudo insmod ./driver/test
$ sudo chmod a+rw /dev/tbuf
$ ./build/test-buf 100
```
