#!/bin/bash
cat ./etc/asci.txt
echo -e "\n\nWelcome to the official anyshell Uninstaller! :("

sudo rm -r /opt/anyshell 2>/dev/null
sudo rm /usr/bin/anyshell 2>/dev/null

sudo systemctl disable anyshell-daemon.service 2>/dev/null
sudo systemctl disable anyshell-server.service 2>/dev/null
sudo systemctl stop anyshell-daemon.service 2>/dev/null
sudo systemctl stop anyshell-server.service 2>/dev/null

sudo rm /etc/systemd/system/anyshell-daemon.service 2>/dev/null
sudo rm /etc/systemd/system/anyshell-server.service 2>/dev/null

echo -e "\ndone!"
