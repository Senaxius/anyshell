#!/bin/bash
read -p "Database: " data
sed -i "4s/.*/$data/" /opt/anyshell/etc/config.txt
sudo systemctl restart anyshell-daemon.service
echo "done"
