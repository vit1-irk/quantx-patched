This is Qt5-patched version of Quantx program. Original readme file is README.

Installation on Ubuntu:

```
sudo apt-get install build-essential git qt5-default
git clone https://github.com/vit1-irk/quantx-patched/

cd quantx-patched

qmake
make -j3

sudo cp kvant /usr/bin/
```

To run program, type `kvant`
