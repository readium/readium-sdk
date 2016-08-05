# Requirements

## Linux

* python 2.7
* npm (node 4.x)
* gcc
* libicu-dev
* libssl-dev

## Windows

* python 2.7
* npm (node 4.x **32bits**)
* Visual Studio 2015 Community

## Mac

* python 2.7
* npm (node 4.x **64bits**)
* XCode 6+

# Build

Build system is very simple.
First you have to bootstrap the project.
Then you can build it.

```
python bootstrap.py
python build.py
```

# Testing

Build has been tested on the following platforms:
* Ubuntu 16.04 - gcc 5.3
* Windows 10 - Visual studio 2015 Community

For the following targets:
* linux 64 bits: libreadium.so
* windows 32 bits: readium.dll
