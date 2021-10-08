#!/bin/bash
# Server_user=$(sed -n 5p ./anyshell)
# Server_user=$(echo $Server_user | sed -n 's/^.*=//p')
# Server_dns=$(sed -n 2p ./anyshell)
# Server_dns=$(echo $Server_dns | sed -n 's/^.*=//p')

cat ./.anyshell/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)\n\n"

if [ -f ./.anyshell/config ]; then
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
    read -p "Server dns: " dns
    read -p "Server local-IP: " local_IP
    read -p "Server User: " User
    echo "$dns" > ./.anyshell/config
    echo "$local_IP" >> ./.anyshell/config
    echo "$User" >> ./.anyshell/config
fi

Server_user=$(sed -n 3p ./.anyshell/config)
Server_dns=$(sed -n 1p ./.anyshell/config)

echo $Server_user
echo $Server_dns

DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ln -s $DIR/.anyshell $HOME
sudo ln -s $DIR/.anyshell /root
sudo timedatectl set-timezone "Europe/Berlin"

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

exit 1

sudo ln -s $DIR/anyshell /usr/bin
sudo ln -s ~/.anyshell/anyshell-deamon.service /etc/systemd/system/
sudo ln -s ~/.anyshell/anyshell-server.service /etc/systemd/system/

echo -e "\n\ndone!"
