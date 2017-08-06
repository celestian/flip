flip
====

Forex Simulation Platform

Features plan
-------------

-   \[ \] CLI flip can ping flipd daemon (\#12)
-   \[ \] flipd daemon can start flip\_crawler with proper configuration (\#13)
-   \[ \] flipd daemon can start flip\_collector with proper configuration (\#14)
-   \[ \] flip\_collector can save data to disk periodically (\#15)
-   \[ \] flip\_collector can save data to disk and end if flipd ask (\#16)

Development & Compilation
-------------------------

### Prerequisities

### nanomsg library

##### nanomsg installation from copr

``` bash
sudo dnf copr enable tknapstad/nanomsg
sudo dnf install -y nanomsg nanomsg-devel nanomsg-utils
```

If you need you can install nanomasg [manually](doc/howto_nanomsg_manually.md).

#### others libraries and tools

``` bash
sudo dnf groups install -y "Development Tools" \
                      "C Development Tools and Libraries"

sudo dnf install -y \
    pkgconf-pkg-config autoconf-archive clang pandoc \
    systemd-devel \
    libtalloc libtalloc-devel \
    libtevent libtevent-devel \
    libuuid libuuid-devel \
    libcurl libcurl-devel \
    libwebsockets libwebsockets-devel \
    sqlite-libs sqlite-devel
```

### Compile flip system

``` bash

git clone --recursive https://github.com/celestian/flip.git

cd flip
./bootstrap.sh
. contrib/bashrc_flip
reconfig && chmake
```

Resources
---------

-   \[C/C++\] <http://en.cppreference.com>
-   \[argp\] <https://www.gnu.org/software/libc/manual/html_node/Argp.html>
-   \[talloc\] <https://talloc.samba.org/talloc/doc/html/group__talloc.html>
-   \[talloc best practicies\] <https://talloc.samba.org/talloc/doc/html/libtalloc__bestpractices.html>
-   \[tevent\] <https://tevent.samba.org/index.html>
-   \[nanomsg\] <http://nanomsg.org/>
-   \[libcurl\] <https://curl.haxx.se/libcurl/c/libcurl.html>
-   \[websockets\] <https://libwebsockets.org/>
-   \[jsmn\] <https://github.com/zserge/jsmn>
-   \[jsmn examples\] <https://github.com/alisdair/jsmn-example>
-   \[sqlite\] <https://www.sqlite.org/>
-   \[inih\] <https://github.com/benhoyt/inih>
-   \[creating library in C\] <https://www.gnu.org/software/automake/manual/automake.html#A-Library>
-   \[daemon\] <https://www.freedesktop.org/software/systemd/man/daemon.html>

How to use
----------

TBD, draft:

``` bash
    reconfig --enable-debug

    make clean && chmake && ./flipd /home/celestian/Projects/flip/example/flip.conf

    ps -e | grep flip ; ll -l /tmp | grep flip

    journalctl -t flip_crawler

    nanocat --sub --connect ipc:///tmp/crawler_pubsub.ipc -AQ


    coredumpctl list
    coredumpctl -o bar.coredump dump /<path>/<to>/<service>
```
