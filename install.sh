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

read -p "Server User [server]: " User
if [ -z $User ]; then
    User="server"
fi
read -p "Server domain [noftp.ddns.net]: " domain
if [ -z $domain ]; then
    domain="noftp.ddns.net"
fi
read -p "Server SQL-port [41998]: " sql_port
if [ -z $sql_port ]; then
    sql_port="41998"
fi
read -p "Server SQL-password: " sql_password
if [ -z $sql_password ]; then
    echo "no password specified, try again..."
    read -p "Server SQL-password: " sql_password
fi
read -p "Server SSH-port [41999]: " ssh_port
if [ -z $ssh_port ]; then
    ssh_port="41999"
fi
read -p "SQL-Database [senaex]: " database
if [ -z $database ]; then
    database="senaex"
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

sed -i '/noftp.ddns.net/d' $HOME/.ssh/known_hosts &>/dev/null
sed -i '/41999/d' $HOME/.ssh/known_hosts &>/dev/null
sudo sed -i '/noftp.ddns.net/d' /root/.ssh/known_hosts &>/dev/null
sudo sed -i '/41999/d' /root/.ssh/known_hosts &>/dev/null

echo -e "\ncreating user-key...\n"
ssh-keygen -f "$HOME/.ssh/anyshell-key" -P "" 
echo -e "\nSending to server...\n"
# sshpass -p "$sql_password" ssh-copy-id -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
ssh-copy-id -f -i ~/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 

echo -e "\ncreating root-key...\n"
sudo ssh-keygen -f "/root/.ssh/anyshell-key" -P "" 
echo -e "\nSending to server...\n"
#sudo sshpass -p "$sql_password" ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
sudo ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns 
# if [ ! -f ~/.ssh/anyshell-key ]; then
#     echo -e "\nNo key found, creating ssh-key\n"
#     ssh-keygen -f "$HOME/.ssh/anyshell-key-new" -P "" 
#     echo -e "\nSending to server...\n"
#     ssh-copy-id -i ~/.ssh/anyshell-key-new.pub -p 41999 $Server_user@$Server_dns 
# else
#     echo -e "SSH-Key found, skipping process..."
# fi
# 
# if sudo test -f "/root/.ssh/anyshell-key"; then
#     echo -e "Root SSH-Key found, skipping process..."
# else
#     echo -e "\nNo root key found, creating ssh-key\n"
#     sudo ssh-keygen -f "/root/.ssh/anyshell-key" -P "" 
#     echo -e "\nSending to server...\n"
#     sudo ssh-copy-id -i /root/.ssh/anyshell-key.pub -p 41999 $Server_user@$Server_dns
# fi
echo "---------------------------done!------------------------------"

echo "compiling the raw anyshell c++ code"
make -C $DIR clean && make -j8 -C $DIR &>/dev/null 
echo "---------------------------done!------------------------------"
