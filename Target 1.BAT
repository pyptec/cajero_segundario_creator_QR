SET C51INC=C:\Keil_v5\C51\Inc;C:\Keil_v5\C51\Inc\Atmel
SET C51LIB=C:\Keil_v5\C51\Lib
SET CPU_TYPE=AT89C51RD2
SET CPU_VENDOR=Atmel
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x01515F30
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\Creator.__i"
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\uart.__i"
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\debug.__i"
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\CRT_288.__i"
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\ptoprlelo.__i"
"C:\Keil_v5\C51\BIN\C51.EXE" @".\hex\lector.__i"
"C:\Keil_v5\C51\BIN\BL51.EXE" @".\hex\Creator jp.lnp"
"C:\Keil_v5\C51\BIN\OH51.EXE" ".\hex\Creator jp" 
