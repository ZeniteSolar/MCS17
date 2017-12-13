make clean
rm -rf bin obj
make all
fuser -k /dev/ttyACM0
avrdude -c arduino -P /dev/ttyACM2 -p m328p -D -U flash:w:bin/firmware.elf
figlet "MCS17"
