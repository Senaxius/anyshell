#!/bin/bash
cat ./.anyshell/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)\n\n"

sudo timedatectl set-timezone "Europe/Berlin"

DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

sudo ln -s $DIR/lib/libmariadb.so /usr/lib
sudo ln -s $DIR/lib/libmariadb.so.3 /usr/lib
sudo ln -s $DIR/lib/libmariadbd.so /usr/lib
sudo ln -s $DIR/lib/libmariadbd.so.19 /usr/lib
# sudo ln -s ~/.anyshell/anyshell-deamon.service /etc/systemd/system/
# sudo ln -s ~/.anyshell/anyshell-server.service /etc/systemd/system/