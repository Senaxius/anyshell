#!/bin/bash
cat ./etc/asci.txt
echo -e "\n\nWelcome to the official anyshell Uninstaller! :("

sudo rm -r /opt/anyshell
sudo rm /usr/bin/anyshell

sudo systemctl disable anyshell-daemon.service
sudo systemctl disable anyshell-server.service
sudo systemctl stop anyshell-daemon.service
sudo systemctl stop anyshell-server.service

sudo rm /etc/systemd/system/anyshell-daemon.service 
sudo rm /etc/systemd/system/anyshell-server.service 

echo -e "\n\ndone!"
