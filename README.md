binson-c
===========

ANSI C implementation of Binson format serialization library. Binson is like JSON, but faster, binary and
even simpler. See [binson.org](http://binson.org/)

Status
========

Pre-alpha (debugging and testing functionality)

Features
========

* Written in ANSI C (C89)
* No 3rd party dependencies (stdlib only)
* Streaming mode parsing
* Input/output abstraction layer

ToDo
========
* Test on bigendian (ARM, etc) platforms
* Unit testing configuration
* Binson-java compatibility testing
* Integrate with CMake build system
* Extensive error handling
* Adopt interfaces to provide model's storage abstraction
* ABI review

Feature requests
================

* JSON import (using external parser library)


Usage
=====

See ./examples subdirectory for usage examples.


Build
=====

To build library plus examples run from project directory:

$ make -f binson.workspace.mak all


Log
===
