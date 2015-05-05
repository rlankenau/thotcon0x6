#!/bin/bash

export BADGE_FLAGS="0x01"
export BADGE_ID="0x00,0x01"

./program.pl "-Ueeprom:w:./eeprom.hex:i -Ueeprom:w:$BADGE_ID,$BADGE_FLAGS,0x00:m"
