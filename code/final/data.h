/* Data layout of eeprom

                           1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |            Badge ID           |     Flags     |   Badge Data  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |   W88 Offset  |  Root Offset  |  Oper Offset  |   VIP Offset  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Voice Offset | Hacker Offset |  Blue Offset  |   Red Offset  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   12 |                             Color                           ...
      ...                            Data                             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  268 | Scrn0 Offset  | Scrn0 Length  | Scrn1 Offset  | Scrn1 Length  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Scrn2 Offset  | Scrn2 Length  | Scrn3 Offset  | Scrn3 Length  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Scrn4 Offset  | Scrn4 Length  | Scrn5 Offset  | Scrn5 Length  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Scrn6 Offset  | Scrn6 Length  | Scrn7 Offset  | Scrn7 Length  |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  286 |                             Screen                          ...
      |                              Data                             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


  Field Descriptions

  Flags 
  
  see #defines below for values.  Describes the type or mode of the badge.
  
  Pattern Offset Table

  The starting offset in bytes of the color string for the corresponding 
  badge type/mode. 

  Color Data

  A block of 8 sequences of 8-bit color value, corresponding to the 8 
  badge modes/types.  Each sequence is terminated by a single byte with
  value 0xF8.  Each byte is interpreted as follows:

   0 1 2 3 4 5 6 7 
  +-+-+-+-+-+-+-+-+
  | R | G | B | I |
  +-+-+-+-+-+-+-+-+

  R - Red value of the next pixel
  G - Green value of the next pixel
  B - Blue value of the next pixel
  I - Intensity of the next pixel
  
  Screen Offset Table
  

*/

//Flags

#define FLAG_BTYPE_W88      0x01 
#define FLAG_BTYPE_ROOT     0x02 
#define FLAG_BTYPE_OPER     0x04 
#define FLAG_BTYPE_VIP      0x08 
#define FLAG_BTYPE_VOICE    0x10
#define FLAG_BTYPE_HACKER   0x20
#define FLAG_MODE_BLUE      0x40
#define FLAG_MODE_RED       0x80 

//EEPROM offset table
#define BADGE_BLOCK_OFF     0
#define OFFSET_BLOCK_OFF    4
#define COLOR_BLOCK_OFF     12
#define SCREEN_BLOCK_OFF    268


#define COLOR_STREAM_END    0xF8

typedef struct eeprom_badge_data_t_stct {
    uint16_t id;
    uint8_t  flags;
    uint8_t  data;
} eeprom_badge_date_t;

typedef eeprom_screen_ident_t_stct {
    uint8_t offset;
    uint8_t len;
}
