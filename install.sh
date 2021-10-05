#!/bin/bash
Server_user=$(sed -n 5p ./anyshell)
Server_user=$(echo $Server_user | sed -n 's/^.*=//p')
Server_dns=$(sed -n 2p ./anyshell)
Server_dns=$(echo $Server_dns | sed -n 's/^.*=//p')

cat ./.anyshell/asci.txt
echo -e "\n\nWelcome to the official anyshell Installer! :)"
echo ""
cat ./.anyshell/asci2.txt

echo -e "\n  Any: Pls. dont be the root User, he is very mean to me :("
echo -e "  Any: If you are root, go fuck yourself and exit, else just press enter :)"
read 

echo -e "\n  Any: So now that we are on our own, lets get started!  (press enter u dump fuck)"
read

echo -e "  Any: firstly, I have to link the .anyshell folder in you users home directory"
echo -e "  Any: Sadly, some commands need to be executed by root, so pls be ready to type in your sudo password"
sleep 2
echo -e "\n  Any: Oh, I forgot to mention, did you change the Server and IP variebles at the start of the anyshell-script?"
read

DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ln -s $DIR/.anyshell $HOME
sudo ln -s $DIR/.anyshell /root
sudo timedatectl set-timezone "Europe/Berlin"
echo -e "  Any: done :)"

echo -e "\n  Any: Next thing on my to do list: "
sleep 3
echo -e "  - Fuck you mom ✔"
sleep 2
echo -e "  - eat fish ✔"
sleep 2
echo -e "  - tell you about the ssh-key stuff"
sleep 2

echo -e "\n  Any: Ahhh, I remember."
echo -e "  Any: You need to move your public key to the server, if you prefer to do it by yourself, type fck_u, else typ yes"
read -p "(fck_u | yes): " answer
if [ "$answer" == "yes" ]; then
    echo "  Any: you have choosen wisely!"
    echo "  Any: Do you want me to generate a key?"
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
elif [ "$answer" == "fck_u" ]; then
    echo "  Any: I hate you and fuck u too"
fi

echo -e "\n  Any: Now I'm gonna link the executable somewhere in the path, but I won't tell you where hehehehehe"

sudo ln -s $DIR/anyshell /usr/bin
sudo ln -s ~/.anyshell/anyshell-deamon.service /etc/systemd/system/
sudo ln -s ~/.anyshell/anyshell-server.service /etc/systemd/system/

echo "  Any: done!"

echo -e "\n  Any: We are done here, but I will give you some tipps:"
echo -e "  Any: to setup a host: type anyshell host setup"
echo -e "  Any: to connect to your hosts: type anyshell connect"
echo -e "  Any: to list your hosts: type anyshell list"

echo -e "\n  Any: Thats all I can tell you, and now I'm going to eat some fish and bbq"
sleep 2
echo -e "  Any: Ps. fuck u lennart, thats why"
