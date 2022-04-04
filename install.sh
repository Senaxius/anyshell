#!/bin/bash
cat ./etc/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)\n\n"

echo -e "\nInstalling dependencies..."
sudo pacman --help &>/dev/null && sudo pacman -S mariadb-libs make sshpass --needed 
sudo apt --help &>/dev/null && sudo apt install libmariadb-dev make g++ sshpass
echo "---------------------------done!------------------------------"

echo -e "\nEntering configuration tool..."
echo -e "!!!  Text in [Brackets] is default, press enter to accept it.  !!!"
echo -e "\n"

# User="anyshell"
# domain="noftp.ddns.net"
# sql_port="41998"
# ssh_port="41999"
# database="senaex"
read -p "Server User [anyshell]: " User
if [ -z $User ]; then
    User="anyshell"
fi
read -p "Server domain [noanus.com]: " domain
if [ -z $domain ]; then
    domain="noanus.com"
fi
read -p "Server SQL-port [41998]: " sql_port
if [ -z $sql_port ]; then
    sql_port="41998"
fi
read -p "Server SSH-port [41999]: " ssh_port
if [ -z $ssh_port ]; then
    ssh_port="41999"
fi
read -p "SQL-Database [senaex]: " database
if [ -z $database ]; then
    database="senaex"
fi
read -p "Server SQL-password: " sql_password
if [ -z $sql_password ]; then
    echo "no password specified, try again..."
    read -p "Server SQL-password: " sql_password
fi

echo "$User" > ./etc/config.txt
echo "$domain" >> ./etc/config.txt
echo "$sql_password" >> ./etc/config.txt
echo "$database" >> ./etc/config.txt
echo "$sql_port" >> ./etc/config.txt
echo "$ssh_port" >> ./etc/config.txt

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

sudo rm ~/.ssh/anyshell-key*
sudo rm /root/.ssh/anyshell-key
sudo rm /root/.ssh/anyshell-key.pub

sudo rm /root/.ssh/known_hosts
sudo rm ~/.ssh/known_hosts

echo -e "\ncreating user-key...\n"
ssh-keygen -f "$HOME/.ssh/anyshell-key" -P "" 
echo -e "\nSending to server...\n"
sshpass -p "$sql_password" ssh-copy-id -o "StrictHostKeyChecking accept-new" -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
# ssh-copy-id -f -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 

echo -e "\ncreating root-key...\n"
sudo ssh-keygen -f "/root/.ssh/anyshell-key" -P "" 
echo -e "\nSending to server...\n"
sudo sshpass -p "$sql_password" ssh-copy-id -o "StrictHostKeyChecking accept-new" -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
echo "---------------------------done!------------------------------"

echo "compiling the raw anyshell c++ code"
make -C $DIR clean && make -j8 -C $DIR &>/dev/null 
echo "---------------------------done!------------------------------"
