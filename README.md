An ANSI C Binson serialization library. Binson is like JSON, 
but faster, binary and even simpler. See [binson.org](http://binson.org/)


Status
------

Alpha version (0.1.0)


Features
--------

* Written in ANSI C (C89 standard)
* Multiplatform. Tested at:
  * x86, x86_64
  * ARM (ARMv71, Cortext-A8)
  * MIPS (big-endian)
* Compatible with [binson-java](https://github.com/franslundberg/binson-java)
* Has not 3rd party dependencies (libc.so only)
* Streaming mode parsing
* Input/output abstraction layer
* Tested with fuzzing stress test tool (included in currenty source tree)


Usage
-----

See ./examples subdirectory for usage examples.


Build
-----

You need to have CMake installed in your system.
Check its presence by:

`
$ cmake --version
`

If you can't see the version info, you need to install it.
For Debian/Ubuntu installation from repo:

`
$ sudo apt-get install cmake
`

For CentOS installation from repo:

`
$ sudo yum install cmake
`


To build binson-c library (plus examples) run from project directory:

```
$ mkdir build
$ cd build
$ cmake ..
$ make all
```

Default configuration is 'Debug'. To build 'Release' replace 3rd command with:

```
$ cmake -DCMAKE_BUILD_TYPE=Release ..
```

To run unit test (from ./build subdirectory):

```
$ make test
```

To run fuzzing test session:

```
$ make fuzz
```

To build documentation (see results in ./build/doc)

```
$ make doc
```
