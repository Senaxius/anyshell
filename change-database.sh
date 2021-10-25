#!/bin/bash
read -p "Database: " data
sed -i "5s/.*/$data/" /opt/anyshell/etc/config.txt
echo "done"
