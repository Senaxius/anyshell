#!/bin/bash
cat ./etc/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)\n\n"

if [ -f ./etc/config.txt ]; then
    echo "Config file found! Create new one? (yes | no):"
    read i
    if [ "$i" == "yes" ]; then
        config=1
    else
        config=0
    fi
else
    echo "No config file found, creating new one..."
    config=1
fi

if [ "$config" == "1" ]; then
    read -p "Server User: " User
    read -p "Server domain: " domain
    read -p "Server local-IP: " local_IP
    read -p "Server Port: " port
    echo "$User" > ./etc/config.txt
    echo "$domain" >> ./etc/config.txt
    echo "$local_IP" >> ./etc/config.txt
    echo "$port" >> ./etc/config.txt
fi

##############Setup#################
echo -e "\nSetting correct timezone..."
sudo timedatectl set-timezone "Europe/Berlin"
DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
echo $DIR
sudo cp -p -r $DIR /opt/
DIR="/opt/anyshell"
sudo ln -s $DIR/lib/libmariadb.so /usr/lib
sudo ln -s $DIR/lib/libmariadb.so.3 /usr/lib
sudo ln -s $DIR/lib/libmariadbd.so /usr/lib
sudo ln -s $DIR/lib/libmariadbd.so.19 /usr/lib
sudo ln -s $DIR/anyshell /usr/bin
sudo ln -s $DIR/etc/anyshell-daemon.service /etc/systemd/system/
sudo ln -s $DIR/etc/anyshell-server.service /etc/systemd/system/
##############ssh-key#################
Server_user=$(sed -n 1p ./config.txt)
Server_dns=$(sed -n 2p ./config.txt)
if [ ! -f ~/.ssh/anyshell-key ]; then
    echo -e "\nNo key found, creating ssh-key\n"
    ssh-keygen -f "$HOME/.ssh/anyshell-key" -P "" 
    echo -e "\nSending to server...\n"
    ssh-copy-id -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
else
    echo -e "\nKey found, sending to server...\n"
    ssh-copy-id -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns
fi
if [ ! -f /root/.ssh/anyshell-key ]; then
    echo -e "\nNo root key found, creating ssh-key\n"
    sudo ssh-keygen -f "/root/.ssh/anyshell-key" -P "" 
    echo -e "\nSending to server...\n"
    sudo ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns
else
    echo -e "\n Root-Key found, sending to server...\n"
    sudo ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
fi

