#!/bin/bash

set IPCS_M = ipcs -m | egrep "0x[0-9a-f]+ [0-9]+" | grep $USERNAME | cut -f1 -d" "

for id in $IPCS_M; do
  ipcrm -M $id;
done
