Manual installation of nanomsg
------------------------------

``` bash
sudo dnf install -y tar wget rubygem-asciidoctor cmake

# compilation of nanomsg
cd /tmp
wget -qO- https://github.com/nanomsg/nanomsg/archive/1.0.0.tar.gz | \
          tar --transform 's/^dbt2-0.37.50.3/dbt2/' -xvz

cd /tmp/nanomsg-1.0.0
mkdir build

cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build .
ctest -C Debug .

# installation
sudo cmake --build . --target install

# post-installation
sudo ldconfig

# please, add to your ~/.bashrc
export LD_LIBRARY_PATH=/usr/local/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:$PKG_CONFIG_PATH

# Clean up
cd ~
rm -fR /tmp/nanomsg-1.0.0
```
