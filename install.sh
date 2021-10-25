#!/bin/bash
cat ./etc/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)\n\n"

sudo pacman -S mariadb-libs make --needed 2>/dev/null
sudo apt install libmariadb-dev make g++ 2>/dev/null

echo -e "\n"
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
echo "---------------------------done!------------------------------"
sudo timedatectl set-timezone "Europe/Berlin"
DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

echo -e "moving source files..."
echo "---------------------------done!------------------------------"
sudo cp -p -r $DIR /opt/ 2>/dev/null
DIR="/opt/anyshell"
sudo ln -s $DIR/anyshell /usr/bin 2>/dev/null
sudo ln -s $DIR/etc/anyshell-daemon.service /etc/systemd/system/ 2>/dev/null
sudo ln -s $DIR/etc/anyshell-server.service /etc/systemd/system/ 2>/dev/null

##############ssh-key#################
Server_user=$(sed -n 1p ./etc/config.txt)
Server_dns=$(sed -n 2p ./etc/config.txt)
echo "Checking ssh-keys"
if [ ! -f ~/.ssh/anyshell-key ]; then
    echo -e "\nNo key found, creating ssh-key\n"
    ssh-keygen -f "$HOME/.ssh/anyshell-key" -P "" 
    echo -e "\nSending to server...\n"
    ssh-copy-id -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
else
    echo -e "SSH-Key found, skipping process..."
fi

if sudo test -f "/root/.ssh/anyshell-key"; then
    echo -e "Root SSH-Key found, skipping process..."
else
    echo -e "\nNo root key found, creating ssh-key\n"
    sudo ssh-keygen -f "/root/.ssh/anyshell-key" -P "" 
    echo -e "\nSending to server...\n"
    sudo ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns
fi
echo "---------------------------done!------------------------------"

echo "compiling the row anyshell c++ code"
make clean && make 
echo "---------------------------done!------------------------------"
