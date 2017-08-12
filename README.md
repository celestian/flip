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

### Preparation

-   [Fedora 26](doc/fedora_26/prepare.md)
-   [Ubuntu 16.04](doc/ubuntu_16.04/prepare.md)

### Compile flip system

``` bash
git clone --recursive https://github.com/celestian/flip.git

cd flip
./bootstrap.sh
./configure && make
```

``` bash
# for cleaning
make distclean
```

### Development utilities

``` bash
# Compilation out of source tree:
. contrib/bashrc_flip
reconfig && chmake
# and for cleaning just remove build directory (e.g. `x86_64`)

# Source code and markdown autoformat
./prepare-commit.sh
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
# Enable debug
reconfig --enable-debug

# Run server flipd
make clean && chmake && ./flipd /home/celestian/Projects/flip/example/flip.conf

# List flip processes
ps -e | grep flip ; ll -l /tmp | grep flip

# See flip's logs
journalctl -t flipd
journalctl -t flip_crawler

# Debug on nanomsg sockets
nanocat --sub --connect ipc:///tmp/crawler_pubsub.ipc -AQ

# Coredump
coredumpctl list
coredumpctl -o bar.coredump dump /<path>/<to>/<service>
```
