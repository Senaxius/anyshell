#!/bin/bash
cat ./.anyshell/asci.txt
echo -e "\n\nWelcome to the official anyshell Uninstaller! :)"

rm -r ~/.anyshell
sudo rm -r /root/.anyshell
sudo rm /usr/bin/anyshell
sudo rm /etc/systemd/system/anyshell-deamon.service 
sudo rm /etc/systemd/system/anyshell-serveserverice 

echo -e "\n\ndone!"
