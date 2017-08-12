Prepare on Fedora 26
--------------------

### Prerequisities

``` bash
sudo dnf update
```

### nanomsg library

You can install it from COPR:

``` bash
sudo dnf copr enable tknapstad/nanomsg
sudo dnf install -y nanomsg nanomsg-devel nanomsg-utils
```

Or you can install nanomasg [manually](doc/fedora_26/howto_nanomsg_manually.md).

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
