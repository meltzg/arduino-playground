#!/bin/bash

set -ex

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
LAST_ID_FILE="$SCRIPT_DIR/last_id.txt"

while true
do
    read -p "Press any key to set ID"
    if [ -e "$LAST_ID_FILE" ]; then
    let ID="($(cat ${LAST_ID_FILE}) % 254) + 1"
    else
        ID=1
    fi

    make NODE_ID=$ID eeprom-clear set-id

    echo $ID > $LAST_ID_FILE
done
