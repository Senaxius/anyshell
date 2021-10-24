#!/bin/bash
ssh $1@$2 -p $3 
# for (( i = 0; i <= 20 ; i++ )); do
#     {
#         ssh $1@$2 -p $3 -q && break 
#     } || { 
#         echo -n "."
#         if [ $i == 20 ]; then
#             echo -e "\nCould not create a connection, client might be offline"
#         fi
#         sleep 0.4
#     }
# done