#!/bin/bash

case $1 in
    HACKER)
    export BADGE_FLAGS="0x01"
    ;;
    VOICE)
    export BADGE_FLAGS="0x02"
    ;;
    OPER)
    export BADGE_FLAGS="0x04"
    ;;
    VIP)
    export BADGE_FLAGS="0x08"
    ;;
    ROOT)
    export BADGE_FLAGS="0x10"
    ;;
    W88)
    export BADGE_FLAGS="0x20"
    ;;
    *)
    echo "usage: $0 <HACKER|VOICE|OPER|VIP|ROOT|W88>"
    exit
    ;;
esac

export BADGE_ID=`nc uranium.workshop88.com 8192`

echo ./program.pl \"-Ueeprom:w:./eeprom.hex:i -Ueeprom:w:$BADGE_ID,$BADGE_FLAGS,0x00:m\"
perl ./program.pl "-Ueeprom:w:./eeprom.hex:i -Ueeprom:w:$BADGE_ID,$BADGE_FLAGS,0x00:m"
