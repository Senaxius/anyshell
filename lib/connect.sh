#!/bin/bash
#ssh $1@$2 -p $3 
for (( i = 0; i <= 10 ; i++ )); do
    {
        # echo "ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break"
        ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break
        
    } || { 
        echo -n "."
        if [ $i == 10 ]; then
            echo -e "\nCould not create a connection, client might be offline"
        fi
        sleep 0.4
    }
done
