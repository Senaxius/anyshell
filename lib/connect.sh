#!/bin/bash
#ssh $1@$2 -p $3 
for (( i = 0; i <= 4 ; i++ )); do
    {
        # echo "ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break"
        ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:$3" &>/dev/null
<<<<<<< HEAD
        sed -i '/localhost/d' $HOME/.ssh/known_hosts &>/dev/null
        ssh -q $1@$2 -p $3 -o StrictHostKeyChecking=no && break
=======
        sed -i '/localhost/d' $HOME/.ssh/known_hosts
        ssh $1@$2 -p $3 -o StrictHostKeyChecking=no && break
>>>>>>> 68e31713dd0cd4d03e839602aafa09e28d02bc1d
        
    } || { 
        echo -n "."
        if [ $i == 4 ]; then
            echo -e "\nCould not create a connection, client might be offline"
        fi
        sleep 0.4
    }
done
