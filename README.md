# flip
Forex Simulation Platform


## How to use
```
reconfig --enable-debug
./flip_crawler ../config/flip_collector.conf
./flip_collector ../config/flip_collector.conf

make clean && chmake && ./flipd /home/celestian/Projects/flip/example/flip.conf
```

```
journalctl -t flip_crawler
nanocat --sub --connect ipc:///tmp/crawler_pubsub.ipc -AQ
```

## Resources
* [C/C++] http://en.cppreference.com
* [argp] https://www.gnu.org/software/libc/manual/html_node/Argp.html
* [talloc] https://talloc.samba.org/talloc/doc/html/group__talloc.html
* [nanomsg] http://nanomsg.org/
* [libcurl] https://curl.haxx.se/libcurl/c/libcurl.html
* [jsmn] https://github.com/zserge/jsmn
* [sqlite] https://www.sqlite.org/
* [inih] https://github.com/benhoyt/inih

https://tevent.samba.org/group__tevent.html#details

https://www.gnu.org/software/automake/manual/automake.html#A-Library

https://talloc.samba.org/talloc/doc/html/libtalloc__bestpractices.html
https://github.com/alisdair/jsmn-example


## Notice
`src/json/jsmn` is a git submodule.

`src/conf/inih` is a git submodule.

```
git submodule foreach git reset --hard
```
