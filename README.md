# SimpleTTY
A tty linux kernel TTY mod driver,example from linux kernel source drivers/tty/ttynull.c

## Usage
### clean
make clean

### build
```
make
```

### uninstall the old driver
```
sudo make uninstall
```

### load the new driver
```
sudo make install
```

### check if driver is install
```
lsmod | grep ttynull
```

### see the linux kernel log
```
dmesg | tail
```

### make node
```
sudo mknod /dev/ttynull c 200 0
sudo chmod 666 /dev/ttynull
```
### test write
```
echo "Hello, Kernel!" | sudo tee /dev/ttynull
```
