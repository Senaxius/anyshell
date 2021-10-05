#!/bin/bash
Server_user=$(sed -n 5p ./anyshell)
Server_user=$(echo $Server_user | sed -n 's/^.*=//p')
Server_dns=$(sed -n 2p ./anyshell)
Server_dns=$(echo $Server_dns | sed -n 's/^.*=//p')

cat ./.anyshell/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)"

DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ln -s $DIR/.anyshell $HOME
sudo ln -s $DIR/.anyshell /root
sudo timedatectl set-timezone "Europe/Berlin"

echo -e "You need to move your public key to the server, if you prefer to do it by yourself, type no, else typ yes"
read -p "(no | yes): " answer
if [ "$answer" == "yes" ]; then
    echo "Do you want me to generate a key?"
    read -p "(yes | no): " answer2
    if [ "$answer2" == "yes" ]; then
        ssh-keygen
        ssh-copy-id -p 41999 $Server_user@$Server_dns
        sudo ssh-keygen
        sudo ssh-copy-id -p 41999 $Server_user@$Server_dns
    elif [ "$answer2" == "no" ]; then 
        ssh-copy-id -p 41999 $Server_user@$Server_dns
        sudo ssh-copy-id -p 41999 $Server_user@$Server_dns
    fi
elif [ "$answer" == "no" ]; then
    echo "ok"
fi

sudo ln -s $DIR/anyshell /usr/bin
sudo ln -s ~/.anyshell/anyshell-deamon.service /etc/systemd/system/
sudo ln -s ~/.anyshell/anyshell-server.service /etc/systemd/system/

echo -e "\n\ndone!"
