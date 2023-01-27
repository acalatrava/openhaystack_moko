## Formato de fichero DFU

Primeros 9 bytes => 12 8a 01 0a 44 08 01 12 40 
[ 
siguiente 24 bytes => 08 01 10 34 1a 02 83 02 20 00 28 00 30 00 38 84 8c 03 42 24 08 03 12 20
Byte 34 => 32 bytes sha256 bin file - little endian
byte 65 => 48 00 52 04 08 01 12 00 
] <-- esto se firma con ecds y se pone a partir del byte 77
byte 73 => 10 00 1a 40 
byte 77 => 