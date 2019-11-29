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
- [x] initializes message queues
- [x] farmer has create_worker function
- [x] define message datastucture
- [ ] define worker function for computing hash value
- [x] when does a worker know when all jobs are done?
  - boolean flag in req message
- [ ] cleans up queues
  - only temporary solution for now
- [ ] create for loop which loops over all plaintext passwords
- [ ] worker should check whether 'finished' flag was set by farmer
- [ ] farmer should be efficient with the queues, i.e. none should be empty for too long
- [ ] write documentation

### OPTIONAL

- [ ] Formal analysis of deadlock freeness

### Important links:

[Linux message queue documentation](http://man7.org/linux/man-pages/man7/mq_overview.7.html)
