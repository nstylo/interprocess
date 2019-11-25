### Remove message queues

First mount queues as file system

```
$ sudo mkdir /dev/mqueue
$ sudo mount none /dev/mqueue -t mqueue
```

Then list and remove all queue

```
$ ls -l /dev/mqueue
$ rm /dev/mqueue/*
```

### Compiling

Either manually compile

```
$ gcc -Wall -o farmer farmer.c -lrt
$ gcc -Wall -o worker worker.c md5s.c -lrt -lm
```

Or use make

```
$ make
$ make clean       # Removes all binaries
```

### Create your own hash values

Compile the md5s programm with `$ make md5s` and run it with `$ ./md5s arg0 arg1 ...` which computes hash values for all strings given as arguments.

### TODO

- [ ] add name and student number to worker.c, farmer.c
- [ ] define message datastucture
- [ ] define worker function for computing hash value
- [ ] when does a worker know when all jobs are done?
- [ ] write documentation

### Important links:

[Linux message queue documentation](http://man7.org/linux/man-pages/man7/mq_overview.7.html)
