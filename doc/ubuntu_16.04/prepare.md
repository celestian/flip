Prepare on Ubuntu 16.04
-----------------------

### Prerequisities

``` bash
sudo apt update && sudo apt upgrade
```

### nanomsg library

Library nanomsg is unfortunately in 0.5 version—so we need to install it [manually](doc/ubuntu_16.04/howto_nanomsg_manually.md).

#### others libraries and tools

``` bash
sudo apt install \
    pkgconf autoconf-archive clang pandoc \
    libsystemd0 libsystemd-dev \
    libtalloc2 libtalloc-dev libtalloc2-dbg \
    libtevent0 libtevent-dev libtevent0-dbg \
    uuid uuid-dev libuuid1 \
    libcurl3 libcurl3-dev libcurl3-dbg \
    libwebsockets7 libwebsockets-dev \
    libsqlite3-0 libsqlite3-dev libsqlite3-0-dbg
```
