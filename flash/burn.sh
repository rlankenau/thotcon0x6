#!/bin/bash
perl ./program.pl "-V -e -Ulock:w:0x3F:m -Uhfuse:w:0xD8:m -Ulfuse:w:0xFF:m -Uefuse:w:0xCB:m"
perl ./program.pl "-V -Uflash:w:Leonardo.hex:i -Ulock:w:0x2F:m"
