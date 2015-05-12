#!/bin/bash

case $1 in
    HACKER)
    export BADGE_FLAGS="0x20"
    ;;
    VOICE)
    export BADGE_FLAGS="0x10"
    ;;
    OPER)
    export BADGE_FLAGS="0x04"
    ;;
    VIP)
    export BADGE_FLAGS="0x08"
    ;;
    ROOT)
    export BADGE_FLAGS="0x02"
    ;;
    W88)
    export BADGE_FLAGS="0x01"
    ;;
    RED)
    export BADGE_FLAGS="0x41"
    ;;
    BLUE)
    export BADGE_FLAGS="0x82"
    ;;
    *)
    echo "usage: $0 <HACKER|VOICE|OPER|VIP|ROOT|W88>"
    exit
    ;;
esac


echo ./program.pl \"-Uflash:w:./final.cpp.hex:i -Ueeprom:w:./eeprom.hex:i -Ueeprom:w:$BADGE_ID,$BADGE_FLAGS,0x00:m\"
perl ./program.pl "-V -e -Ulock:w:0x3F:m -Uhfuse:w:0xD8:m -Ulfuse:w:0xFF:m -Uefuse:w:0xCB:m"
perl ./program.pl "-V -Uflash:w:Leonardo.hex:i -Ulock:w:0x2F:m"
