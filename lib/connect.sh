#!/bin/bash
#ssh $1@$2 -p $3 
for (( i = 0; i <= 4 ; i++ )); do
    {
        # echo "ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break"
        ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:$3" &>/dev/null
        sed -i '/localhost/d' $HOME/.ssh/known_hosts
        ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break
        
    } || { 
        echo -n "."
        if [ $i == 4 ]; then
            echo -e "\nCould not create a connection, client might be offline"
        fi
        sleep 0.4
    }
done
