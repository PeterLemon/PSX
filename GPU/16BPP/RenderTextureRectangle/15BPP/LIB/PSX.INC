;============== (Key: R=Read, W=Write) 
; PSX Include
;==============
; Memory Map
RAM_KUSEG equ 0x00000000 ; Main RAM KUSEG (Cached)   (First 64KB Reserved For BIOS)                                      2048KB/RW
RAM_KSEG0 equ 0x80000000 ; Main RAM KSEG0 (Uncached) (First 64KB Reserved For BIOS)                                      2048KB/RW
RAM_KSEG1 equ 0xA0000000 ; Main RAM KSEG1 (Virtual)  (First 64KB Reserved For BIOS)                                      2048KB/RW

EXP1_KUSEG equ 0x1F000000 ; Expansion Region 1 (ROM/RAM) KUSEG (Cached)                                                  8192KB/RW
EXP1_KSEG0 equ 0x9F000000 ; Expansion Region 1 (ROM/RAM) KSEG0 (Uncached)                                                8192KB/RW
EXP1_KSEG1 equ 0xBF000000 ; Expansion Region 1 (ROM/RAM) KSEG1 (Virtual)                                                 8192KB/RW

SPM_KUSEG equ 0x1F800000 ; Scratchpad (D-Cache Used As Fast RAM) KUSEG (Cached)                                             1KB/RW
SPM_KSEG0 equ 0x9F800000 ; Scratchpad (D-Cache Used As Fast RAM) KSEG0 (Uncached)                                           1KB/RW
SPM_KSEG1 equ 0xBF800000 ; Scratchpad (D-Cache Used As Fast RAM) KSEG1 (Virtual)                                            1KB/RW

IOP_KUSEG equ 0x1F801000 ; I/O Ports KUSEG (Cached)                                                                         8KB/RW
IOP_KSEG0 equ 0x9F801000 ; I/O Ports KSEG0 (Uncached)                                                                       8KB/RW
IOP_KSEG1 equ 0xBF801000 ; I/O Ports KSEG1 (Virtual)                                                                        8KB/RW

EXP2_KUSEG equ 0x1F802000 ; Expansion Region 2 (I/O Ports) KUSEG (Cached)                                                   8KB/RW
EXP2_KSEG0 equ 0x9F802000 ; Expansion Region 2 (I/O Ports) KSEG0 (Uncached)                                                 8KB/RW
EXP2_KSEG1 equ 0xBF802000 ; Expansion Region 2 (I/O Ports) KSEG1 (Virtual)                                                  8KB/RW

EXP3_KUSEG equ 0x1FA00000 ; Expansion Region 3 (Whatever Purpose) KUSEG (Cached)                                         2048KB/RW
EXP3_KSEG0 equ 0x9FA00000 ; Expansion Region 3 (Whatever Purpose) KSEG0 (Uncached)                                       2048KB/RW
EXP3_KSEG1 equ 0xBFA00000 ; Expansion Region 3 (Whatever Purpose) KSEG1 (Virtual)                                        2048KB/RW

BIOS_KUSEG equ 0x1FC00000 ; BIOS ROM (Kernel) (4096K Max) KUSEG (Cached)                                                 4096KB/R
BIOS_KSEG0 equ 0x9FC00000 ; BIOS ROM (Kernel) (4096K max) KSEG0 (Uncached)                                               4096KB/R
BIOS_KSEG1 equ 0xBFC00000 ; BIOS ROM (Kernel) (4096K max) KSEG1 (Virtual)                                                4096KB/R

IOP_KSEG2 equ 0xFFFE0000 ; I/O Ports (Cache Control) KSEG2                                                                 512B/RW

; I/O Map
IO_BASE equ 0x1F800000 ; I/O Port Base Address ($1F80XXXX)

; Expansion Region 1
EXP1_BASE equ 0x1F000000 ; $1F000000..$1F07FFFF Expansion Region 1: Region (Default 512KB, Max 8MB)                      8192KB/RW
EXP1_ROM  equ 0x1F000000 ; $1F000000..$1F0000FF Expansion Region 1: ROM Header (IDs & Entrypoints)                         256B/R

; Scratchpad
SPM_RAM equ 0x0000 ; $1F800000..$1F8003FF Scratchpad: (1KB Fast RAM) (Data Cache Mapped To Fixed Address)                   1KB/RW

; Memory Control 1
EXP1_ADDR   equ 0x1000 ; $1F801000..$1F801003 Memory Control 1: Expansion 1 Base Address (Usually $1F000000)                 4B/RW
EXP2_ADDR   equ 0x1004 ; $1F801004..$1F801007 Memory Control 1: Expansion 2 Base Address (usually $1F802000)                 4B/RW
EXP1_DELAY  equ 0x1008 ; $1F801008..$1F80100B Memory Control 1: Expansion 1 Delay/Size   (Usually $0013243F/512KB 8-Bit BUS) 4B/RW
EXP3_DELAY  equ 0x100C ; $1F80100C..$1F80100F Memory Control 1: Expansion 3 Delay/Size   (Usually $00003022/1B)              4B/RW
BIOS_DELAY  equ 0x1010 ; $1F801010..$1F801013 Memory Control 1: BIOS ROM    Delay/Size   (Usually $0013243F/512KB 8-Bit BUS) 4B/RW
SPU_DELAY   equ 0x1014 ; $1F801014..$1F801017 Memory Control 1: SPU         Delay/Size   (Usually $200931E1)                 4B/RW
CDROM_DELAY equ 0x1018 ; $1F801018..$1F80101B Memory Control 1: CDROM       Delay/Size   (Usually $00020843 Or $00020943)    4B/RW
EXP2_DELAY  equ 0x101C ; $1F80101C..$1F80101F Memory Control 1: Expansion 2 Delay/Size   (Usually $00070777/128B  8-Bit BUS) 4B/RW
COM_DELAY   equ 0x1020 ; $1F801020..$1F801023 Memory Control 1: COMMON      Delay/Size ($00031125 Or $0000132C Or $00001325) 4B/RW

; Peripheral I/O Ports
JOY_DATA equ 0x1040 ; $1F801040..$1F801043 Peripheral I/O Ports: Joypad/Memory Card Data                                     4B/RW
JOY_STAT equ 0x1044 ; $1F801044..$1F801047 Peripheral I/O Ports: Joypad/Memory Card Status                                   4B/R
JOY_MODE equ 0x1048 ; $1F801048..$1F801049 Peripheral I/O Ports: Joypad/Memory Card Mode                                     2B/RW
JOY_CTRL equ 0x104A ; $1F80104A..$1F80104D Peripheral I/O Ports: Joypad/Memory Card Control                                  4B/RW
JOY_BAUD equ 0x104E ; $1F80104E..$1F80104F Peripheral I/O Ports: Joypad/Memory Card Baudrate                                 2B/RW
SIO_DATA equ 0x1050 ; $1F801050..$1F801053 Peripheral I/O Ports: Serial Port Data                                            4B/RW
SIO_STAT equ 0x1054 ; $1F801054..$1F801057 Peripheral I/O Ports: Serial Port Status                                          4B/R
SIO_MODE equ 0x1058 ; $1F801058..$1F801059 Peripheral I/O Ports: Serial Port Mode                                            2B/RW
SIO_CTRL equ 0x105A ; $1F80105A..$1F80105B Peripheral I/O Ports: Serial Port Control                                         2B/RW
SIO_MISC equ 0x105C ; $1F80105C..$1F80105D Peripheral I/O Ports: Serial Port Internal Register                               2B/RW
SIO_BAUD equ 0x105E ; $1F80105E..$1F80105F Peripheral I/O Ports: Serial Port Baudrate                                        2B/RW

; Memory Control 2
RAM_SIZE equ 0x1060 ; $1F801060..$1F801063 Memory Control 2: RAM Size (Usually $00000B88/2MB RAM Mirrored In First 8MB)      4B/RW

; Interrupt Control
I_STAT equ 0x1070 ; $1F801070..$1F801073 Interrupt Control: Interrupt Status Register                                        4B/RW
I_MASK equ 0x1074 ; $1F801074..$1F801077 Interrupt Control: Interrupt Mask Register                                          4B/RW

; DMA Registers
D0_MADR equ 0x1080 ; $1F801080..$1F801083 DMA Registers: DMA Channel 0 Base Address    (MDECin)                              4B/RW
D0_BCR  equ 0x1084 ; $1F801084..$1F801087 DMA Registers: DMA Channel 0 Block Control   (MDECin)                              4B/RW
D0_CHCR equ 0x1088 ; $1F801088..$1F80108B DMA Registers: DMA Channel 0 Channel Control (MDECin)                              4B/RW

D1_MADR equ 0x1090 ; $1F801090..$1F801093 DMA Registers: DMA Channel 1 Base Address    (MDECout)                             4B/RW
D1_BCR  equ 0x1094 ; $1F801094..$1F801097 DMA Registers: DMA Channel 1 Block Control   (MDECout)                             4B/RW
D1_CHCR equ 0x1098 ; $1F801098..$1F80109B DMA Registers: DMA Channel 1 Channel Control (MDECout)                             4B/RW

D2_MADR equ 0x10A0 ; $1F8010A0..$1F8010A3 DMA Registers: DMA Channel 2 Base Address    (GPU Display List & Image Data)       4B/RW
D2_BCR  equ 0x10A4 ; $1F8010A4..$1F8010A7 DMA Registers: DMA Channel 2 Block Control   (GPU Display List & Image Data)       4B/RW
D2_CHCR equ 0x10A8 ; $1F8010A8..$1F8010AB DMA Registers: DMA Channel 2 Channel Control (GPU Display List & Image Data)       4B/RW

D3_MADR equ 0x10B0 ; $1F8010B0..$1F8010B3 DMA Registers: DMA Channel 3 Base Address    (CDROM)                               4B/RW
D3_BCR  equ 0x10B4 ; $1F8010B4..$1F8010B7 DMA Registers: DMA Channel 3 Block Control   (CDROM)                               4B/RW
D3_CHCR equ 0x10B8 ; $1F8010B8..$1F8010BB DMA Registers: DMA Channel 3 Channel Control (CDROM)                               4B/RW

D4_MADR equ 0x10C0 ; $1F8010C0..$1F8010C3 DMA Registers: DMA Channel 4 Base Address    (SPU)                                 4B/RW
D4_BCR  equ 0x10C4 ; $1F8010C4..$1F8010C7 DMA Registers: DMA Channel 4 Block Control   (SPU)                                 4B/RW
D4_CHCR equ 0x10C8 ; $1F8010C8..$1F8010CB DMA Registers: DMA Channel 4 Channel Control (SPU)                                 4B/RW

D5_MADR equ 0x10D0 ; $1F8010D0..$1F8010D3 DMA Registers: DMA Channel 5 Base Address    (PIO Expansion Port)                  4B/RW
D5_BCR  equ 0x10D4 ; $1F8010D4..$1F8010D7 DMA Registers: DMA Channel 5 Block Control   (PIO Expansion Port)                  4B/RW
D5_CHCR equ 0x10D8 ; $1F8010D8..$1F8010DB DMA Registers: DMA Channel 5 Channel Control (PIO Expansion Port)                  4B/RW

D6_MADR equ 0x10E0 ; $1F8010E0..$1F8010E3 DMA Registers: DMA Channel 6 Base Address    (OTC Reverse Clear OT, GPU Related)   4B/RW
D6_BCR  equ 0x10E4 ; $1F8010E4..$1F8010E7 DMA Registers: DMA Channel 6 Block Control   (OTC Reverse Clear OT, GPU Related)   4B/RW
D6_CHCR equ 0x10E8 ; $1F8010E8..$1F8010EB DMA Registers: DMA Channel 6 Channel Control (OTC Reverse Clear OT, GPU Related)   4B/RW

DPCR    equ 0x10F0 ; $1F8010F0..$1F8010F3 DMA Registers: DMA Control Register                                                4B/RW
DICR    equ 0x10F4 ; $1F8010F4..$1F8010F7 DMA Registers: DMA Interrupt Register                                              4B/RW
D_UNK1  equ 0x10F8 ; $1F8010F8..$1F8010FB DMA Registers: DMA Unknown (Changes To $7FE358D1 After DMA Transfer)               4B/RW
D_UNK2  equ 0x10FC ; $1F8010FC..$1F8010FF DMA Registers: DMA Unknown (Usually $00FFFFF7) (Maybe OTC Fill Value)              4B/RW

; Timers (AKA Root Counters)
T0_CNT  equ 0x1100 ; $1F801100..$1F801103 Timers: Timer 0 Current Counter Value (Dotclock)                                   4B/RW
T0_CNTM equ 0x1104 ; $1F801104..$1F801107 Timers: Timer 0 Counter Mode          (Dotclock)                                   4B/RW
T0_CNTT equ 0x1108 ; $1F801108..$1F80110B Timers: Timer 0 Counter Target Value  (Dotclock)                                   4B/RW

T1_CNT  equ 0x1110 ; $1F801110..$1F801113 Timers: Timer 1 Current Counter Value (Horizontal Retrace)                         4B/RW
T1_CNTM equ 0x1114 ; $1F801114..$1F801117 Timers: Timer 1 Counter Mode          (Horizontal Retrace)                         4B/RW
T1_CNTT equ 0x1118 ; $1F801118..$1F80111B Timers: Timer 1 Counter Target Value  (Horizontal Retrace)                         4B/RW

T2_CNT  equ 0x1120 ; $1F801120..$1F801123 Timers: Timer 2 Current Counter Value (1/8 System Clock)                           4B/RW
T2_CNTM equ 0x1124 ; $1F801124..$1F801127 Timers: Timer 2 Counter Mode          (1/8 System Clock)                           4B/RW
T2_CNTT equ 0x1128 ; $1F801128..$1F80112B Timers: Timer 2 Counter Target Value  (1/8 System Clock)                           4B/RW

; CDROM Registers (Address.Read/Write.Index)
CD_STAT  equ 0x1800 ; $1F801800..$1F801800 CDROM Registers: CD Index/Status Register (Bit 0..1 R/W, Bit 2..7 Read Only)      1B/RW
CD_RRFI  equ 0x1801 ; $1F801801..$1F801801 CDROM Registers: Read CD Response Fifo (Usually With Index1)                      1B/R
CD_RDFI  equ 0x1802 ; $1F801802..$1F801802 CDROM Registers: Read CD Data Fifo 8-Bit/16-Bit (Usually With Index0..1)          1B/R

CD_R0INT equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Read 0 CD Interrupt Enable Register                              1B/R
CD_R1FLG equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Read 1 CD Interrupt Flag Register                                1B/RW
CD_R2INT equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Read 2 CD Interrupt Enable Register (Mirror)                     1B/R
CD_R3FLG equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Read 3 CD Interrupt Flag Register (Mirror)                       1B/RW

CD_W0COM equ 0x1801 ; $1F801801..$1F801801 CDROM Registers: Write 0 CD Command Register                                      1B/W
CD_W0PFI equ 0x1802 ; $1F801802..$1F801802 CDROM Registers: Write 0 CD Parameter Fifo                                        1B/W
CD_W0REQ equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Write 0 CD Request Register                                      1B/W

CD_W1UNK equ 0x1801 ; $1F801801..$1F801801 CDROM Registers: Write 1 CD Unknown/Unused                                        1B/W
CD_W1INT equ 0x1802 ; $1F801802..$1F801802 CDROM Registers: Write 1 CD Interrupt Enable Register                             1B/W
CD_W1FLG equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Write 1 CD Interrupt Flag Register                               1B/RW

CD_W2UNK equ 0x1801 ; $1F801801..$1F801801 CDROM Registers: Write 2 CD Unknown/Unused                                        1B/W
CD_W2VLL equ 0x1802 ; $1F801802..$1F801802 CDROM Registers: Write 2 CD Audio Volume For Left-CD-Out To Left-SPU-Input        1B/W
CD_W2VLR equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Write 2 CD Audio Volume For Left-CD-Out To Right-SPU-Input       1B/W

CD_W3VRR equ 0x1801 ; $1F801801..$1F801801 CDROM Registers: Write 3 CD Audio Volume For Right-CD-Out To Right-SPU-Input      1B/W
CD_W3VRL equ 0x1802 ; $1F801802..$1F801802 CDROM Registers: Write 3 CD Audio Volume For Right-CD-Out To Left-SPU-Input       1B/W
CD_W3VAC equ 0x1803 ; $1F801803..$1F801803 CDROM Registers: Write 3 CD Audio Volume Apply Changes (By Writing Bit 5 = 1)     1B/W

; GPU Registers
GP0     equ 0x1810 ; $1F801810..$1F801813 GPU Registers: Write GP0 Commands/Packets (Rendering & VRAM Access)                4B/W
GP1     equ 0x1814 ; $1F801814..$1F801817 GPU Registers: Write GP1 Commands (Display Control)                                4B/W

GPUREAD equ 0x1810 ; $1F801810..$1F801813 GPU Registers: Read Responses To GP0($C0) & GP1($10) Commands                      4B/R
GPUSTAT equ 0x1814 ; $1F801814..$1F801817 GPU Registers: Read GPU Status Register                                            4B/R

; MDEC Registers
MDECPAR  equ 0x1820 ; $1F801820..$1F801823 MDEC Registers: Write MDEC Command/Parameter Register                             4B/W
MDECCNT  equ 0x1824 ; $1F801824..$1F801827 MDEC Registers: Write MDEC Control/Reset Register                                 4B/W

MDECREAD equ 0x1820 ; $1F801820..$1F801823 MDEC Registers: Read MDEC Data/Response Register                                  4B/R
MDECSTAT equ 0x1824 ; $1F801824..$1F801827 MDEC Registers: Read MDEC Status Register                                         4B/R

; SPU Voice Registers (0..23)
SPUVOL0  equ 0x1C00 ; $1F801C00..$1F801C03 SPU Voice Registers: Voice 0  Volume Left/Right                                   4B/RW
SPUVOL1  equ 0x1C10 ; $1F801C10..$1F801C13 SPU Voice Registers: Voice 1  Volume Left/Right                                   4B/RW
SPUVOL2  equ 0x1C20 ; $1F801C20..$1F801C23 SPU Voice Registers: Voice 2  Volume Left/Right                                   4B/RW
SPUVOL3  equ 0x1C30 ; $1F801C30..$1F801C33 SPU Voice Registers: Voice 3  Volume Left/Right                                   4B/RW
SPUVOL4  equ 0x1C40 ; $1F801C40..$1F801C43 SPU Voice Registers: Voice 4  Volume Left/Right                                   4B/RW
SPUVOL5  equ 0x1C50 ; $1F801C50..$1F801C53 SPU Voice Registers: Voice 5  Volume Left/Right                                   4B/RW
SPUVOL6  equ 0x1C60 ; $1F801C60..$1F801C63 SPU Voice Registers: Voice 6  Volume Left/Right                                   4B/RW
SPUVOL7  equ 0x1C70 ; $1F801C70..$1F801C73 SPU Voice Registers: Voice 7  Volume Left/Right                                   4B/RW
SPUVOL8  equ 0x1C80 ; $1F801C80..$1F801C83 SPU Voice Registers: Voice 8  Volume Left/Right                                   4B/RW
SPUVOL9  equ 0x1C90 ; $1F801C90..$1F801C93 SPU Voice Registers: Voice 9  Volume Left/Right                                   4B/RW
SPUVOL10 equ 0x1CA0 ; $1F801CA0..$1F801CA3 SPU Voice Registers: Voice 10 Volume Left/Right                                   4B/RW
SPUVOL11 equ 0x1CB0 ; $1F801CB0..$1F801CB3 SPU Voice Registers: Voice 11 Volume Left/Right                                   4B/RW
SPUVOL12 equ 0x1CC0 ; $1F801CC0..$1F801CC3 SPU Voice Registers: Voice 12 Volume Left/Right                                   4B/RW
SPUVOL13 equ 0x1CD0 ; $1F801CD0..$1F801CD3 SPU Voice Registers: Voice 13 Volume Left/Right                                   4B/RW
SPUVOL14 equ 0x1CE0 ; $1F801CE0..$1F801CE3 SPU Voice Registers: Voice 14 Volume Left/Right                                   4B/RW
SPUVOL15 equ 0x1CF0 ; $1F801CF0..$1F801CF3 SPU Voice Registers: Voice 15 Volume Left/Right                                   4B/RW
SPUVOL16 equ 0x1D00 ; $1F801D00..$1F801D03 SPU Voice Registers: Voice 16 Volume Left/Right                                   4B/RW
SPUVOL17 equ 0x1D10 ; $1F801D10..$1F801D13 SPU Voice Registers: Voice 17 Volume Left/Right                                   4B/RW
SPUVOL18 equ 0x1D20 ; $1F801D20..$1F801D23 SPU Voice Registers: Voice 18 Volume Left/Right                                   4B/RW
SPUVOL19 equ 0x1D30 ; $1F801D30..$1F801D33 SPU Voice Registers: Voice 19 Volume Left/Right                                   4B/RW
SPUVOL20 equ 0x1D40 ; $1F801D40..$1F801D43 SPU Voice Registers: Voice 20 Volume Left/Right                                   4B/RW
SPUVOL21 equ 0x1D50 ; $1F801D50..$1F801D53 SPU Voice Registers: Voice 21 Volume Left/Right                                   4B/RW
SPUVOL22 equ 0x1D60 ; $1F801D60..$1F801D63 SPU Voice Registers: Voice 22 Volume Left/Right                                   4B/RW
SPUVOL23 equ 0x1D70 ; $1F801D70..$1F801D73 SPU Voice Registers: Voice 23 Volume Left/Right                                   4B/RW

SPUFRQ0  equ 0x1C04 ; $1F801C04..$1F801C05 SPU Voice Registers: Voice 0  ADPCM Sample Rate                                   2B/RW
SPUFRQ1  equ 0x1C14 ; $1F801C14..$1F801C15 SPU Voice Registers: Voice 1  ADPCM Sample Rate                                   2B/RW
SPUFRQ2  equ 0x1C24 ; $1F801C24..$1F801C25 SPU Voice Registers: Voice 2  ADPCM Sample Rate                                   2B/RW
SPUFRQ3  equ 0x1C34 ; $1F801C34..$1F801C35 SPU Voice Registers: Voice 3  ADPCM Sample Rate                                   2B/RW
SPUFRQ4  equ 0x1C44 ; $1F801C44..$1F801C45 SPU Voice Registers: Voice 4  ADPCM Sample Rate                                   2B/RW
SPUFRQ5  equ 0x1C54 ; $1F801C54..$1F801C55 SPU Voice Registers: Voice 5  ADPCM Sample Rate                                   2B/RW
SPUFRQ6  equ 0x1C64 ; $1F801C64..$1F801C65 SPU Voice Registers: Voice 6  ADPCM Sample Rate                                   2B/RW
SPUFRQ7  equ 0x1C74 ; $1F801C74..$1F801C75 SPU Voice Registers: Voice 7  ADPCM Sample Rate                                   2B/RW
SPUFRQ8  equ 0x1C84 ; $1F801C84..$1F801C85 SPU Voice Registers: Voice 8  ADPCM Sample Rate                                   2B/RW
SPUFRQ9  equ 0x1C94 ; $1F801C94..$1F801C95 SPU Voice Registers: Voice 9  ADPCM Sample Rate                                   2B/RW
SPUFRQ10 equ 0x1CA4 ; $1F801CA4..$1F801CA5 SPU Voice Registers: Voice 10 ADPCM Sample Rate                                   2B/RW
SPUFRQ11 equ 0x1CB4 ; $1F801CB4..$1F801CB5 SPU Voice Registers: Voice 11 ADPCM Sample Rate                                   2B/RW
SPUFRQ12 equ 0x1CC4 ; $1F801CC4..$1F801CC5 SPU Voice Registers: Voice 12 ADPCM Sample Rate                                   2B/RW
SPUFRQ13 equ 0x1CD4 ; $1F801CD4..$1F801CD5 SPU Voice Registers: Voice 13 ADPCM Sample Rate                                   2B/RW
SPUFRQ14 equ 0x1CE4 ; $1F801CE4..$1F801CE5 SPU Voice Registers: Voice 14 ADPCM Sample Rate                                   2B/RW
SPUFRQ15 equ 0x1CF4 ; $1F801CF4..$1F801CF5 SPU Voice Registers: Voice 15 ADPCM Sample Rate                                   2B/RW
SPUFRQ16 equ 0x1D04 ; $1F801D04..$1F801D05 SPU Voice Registers: Voice 16 ADPCM Sample Rate                                   2B/RW
SPUFRQ17 equ 0x1D14 ; $1F801D14..$1F801D15 SPU Voice Registers: Voice 17 ADPCM Sample Rate                                   2B/RW
SPUFRQ18 equ 0x1D24 ; $1F801D24..$1F801D25 SPU Voice Registers: Voice 18 ADPCM Sample Rate                                   2B/RW
SPUFRQ19 equ 0x1D34 ; $1F801D34..$1F801D35 SPU Voice Registers: Voice 19 ADPCM Sample Rate                                   2B/RW
SPUFRQ20 equ 0x1D44 ; $1F801D44..$1F801D45 SPU Voice Registers: Voice 20 ADPCM Sample Rate                                   2B/RW
SPUFRQ21 equ 0x1D54 ; $1F801D54..$1F801D55 SPU Voice Registers: Voice 21 ADPCM Sample Rate                                   2B/RW
SPUFRQ22 equ 0x1D64 ; $1F801D64..$1F801D65 SPU Voice Registers: Voice 22 ADPCM Sample Rate                                   2B/RW
SPUFRQ23 equ 0x1D74 ; $1F801D74..$1F801D75 SPU Voice Registers: Voice 23 ADPCM Sample Rate                                   2B/RW

SPUBASE0  equ 0x1C06 ; $1F801C06..$1F801C07 SPU Voice Registers: Voice 0  ADPCM Start Address                                2B/RW
SPUBASE1  equ 0x1C16 ; $1F801C16..$1F801C17 SPU Voice Registers: Voice 1  ADPCM Start Address                                2B/RW
SPUBASE2  equ 0x1C26 ; $1F801C26..$1F801C27 SPU Voice Registers: Voice 2  ADPCM Start Address                                2B/RW
SPUBASE3  equ 0x1C36 ; $1F801C36..$1F801C37 SPU Voice Registers: Voice 3  ADPCM Start Address                                2B/RW
SPUBASE4  equ 0x1C46 ; $1F801C46..$1F801C47 SPU Voice Registers: Voice 4  ADPCM Start Address                                2B/RW
SPUBASE5  equ 0x1C56 ; $1F801C56..$1F801C57 SPU Voice Registers: Voice 5  ADPCM Start Address                                2B/RW
SPUBASE6  equ 0x1C66 ; $1F801C66..$1F801C67 SPU Voice Registers: Voice 6  ADPCM Start Address                                2B/RW
SPUBASE7  equ 0x1C76 ; $1F801C76..$1F801C77 SPU Voice Registers: Voice 7  ADPCM Start Address                                2B/RW
SPUBASE8  equ 0x1C86 ; $1F801C86..$1F801C87 SPU Voice Registers: Voice 8  ADPCM Start Address                                2B/RW
SPUBASE9  equ 0x1C96 ; $1F801C96..$1F801C97 SPU Voice Registers: Voice 9  ADPCM Start Address                                2B/RW
SPUBASE10 equ 0x1CA6 ; $1F801CA6..$1F801CA7 SPU Voice Registers: Voice 10 ADPCM Start Address                                2B/RW
SPUBASE11 equ 0x1CB6 ; $1F801CB6..$1F801CB7 SPU Voice Registers: Voice 11 ADPCM Start Address                                2B/RW
SPUBASE12 equ 0x1CC6 ; $1F801CC6..$1F801CC7 SPU Voice Registers: Voice 12 ADPCM Start Address                                2B/RW
SPUBASE13 equ 0x1CD6 ; $1F801CD6..$1F801CD7 SPU Voice Registers: Voice 13 ADPCM Start Address                                2B/RW
SPUBASE14 equ 0x1CE6 ; $1F801CE6..$1F801CE7 SPU Voice Registers: Voice 14 ADPCM Start Address                                2B/RW
SPUBASE15 equ 0x1CF6 ; $1F801CF6..$1F801CF7 SPU Voice Registers: Voice 15 ADPCM Start Address                                2B/RW
SPUBASE16 equ 0x1D06 ; $1F801D06..$1F801D07 SPU Voice Registers: Voice 16 ADPCM Start Address                                2B/RW
SPUBASE17 equ 0x1D16 ; $1F801D16..$1F801D17 SPU Voice Registers: Voice 17 ADPCM Start Address                                2B/RW
SPUBASE18 equ 0x1D26 ; $1F801D26..$1F801D27 SPU Voice Registers: Voice 18 ADPCM Start Address                                2B/RW
SPUBASE19 equ 0x1D36 ; $1F801D36..$1F801D37 SPU Voice Registers: Voice 19 ADPCM Start Address                                2B/RW
SPUBASE20 equ 0x1D46 ; $1F801D46..$1F801D47 SPU Voice Registers: Voice 20 ADPCM Start Address                                2B/RW
SPUBASE21 equ 0x1D56 ; $1F801D56..$1F801D57 SPU Voice Registers: Voice 21 ADPCM Start Address                                2B/RW
SPUBASE22 equ 0x1D66 ; $1F801D66..$1F801D67 SPU Voice Registers: Voice 22 ADPCM Start Address                                2B/RW
SPUBASE23 equ 0x1D76 ; $1F801D76..$1F801D77 SPU Voice Registers: Voice 23 ADPCM Start Address                                2B/RW

SPUADSR0  equ 0x1C08 ; $1F801C08..$1F801C0B SPU Voice Registers: Voice 0  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR1  equ 0x1C18 ; $1F801C18..$1F801C1B SPU Voice Registers: Voice 1  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR2  equ 0x1C28 ; $1F801C28..$1F801C2B SPU Voice Registers: Voice 2  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR3  equ 0x1C38 ; $1F801C38..$1F801C3B SPU Voice Registers: Voice 3  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR4  equ 0x1C48 ; $1F801C48..$1F801C4B SPU Voice Registers: Voice 4  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR5  equ 0x1C58 ; $1F801C58..$1F801C5B SPU Voice Registers: Voice 5  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR6  equ 0x1C68 ; $1F801C68..$1F801C6B SPU Voice Registers: Voice 6  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR7  equ 0x1C78 ; $1F801C78..$1F801C7B SPU Voice Registers: Voice 7  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR8  equ 0x1C88 ; $1F801C88..$1F801C8B SPU Voice Registers: Voice 8  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR9  equ 0x1C98 ; $1F801C98..$1F801C9B SPU Voice Registers: Voice 9  ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR10 equ 0x1CA8 ; $1F801CA8..$1F801CAB SPU Voice Registers: Voice 10 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR11 equ 0x1CB8 ; $1F801CB8..$1F801CBB SPU Voice Registers: Voice 11 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR12 equ 0x1CC8 ; $1F801CC8..$1F801CCB SPU Voice Registers: Voice 12 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR13 equ 0x1CD8 ; $1F801CD8..$1F801CDB SPU Voice Registers: Voice 13 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR14 equ 0x1CE8 ; $1F801CE8..$1F801CEB SPU Voice Registers: Voice 14 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR15 equ 0x1CF8 ; $1F801CF8..$1F801CFB SPU Voice Registers: Voice 15 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR16 equ 0x1D08 ; $1F801D08..$1F801D0B SPU Voice Registers: Voice 16 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR17 equ 0x1D18 ; $1F801D18..$1F801D1B SPU Voice Registers: Voice 17 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR18 equ 0x1D28 ; $1F801D28..$1F801D2B SPU Voice Registers: Voice 18 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR19 equ 0x1D38 ; $1F801D38..$1F801D3B SPU Voice Registers: Voice 19 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR20 equ 0x1D48 ; $1F801D48..$1F801D4B SPU Voice Registers: Voice 20 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR21 equ 0x1D58 ; $1F801D58..$1F801D5B SPU Voice Registers: Voice 21 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR22 equ 0x1D68 ; $1F801D68..$1F801D6B SPU Voice Registers: Voice 22 ADSR (Attack/Decay/Sustain/Release)                4B/RW
SPUADSR23 equ 0x1D78 ; $1F801D78..$1F801D7B SPU Voice Registers: Voice 23 ADSR (Attack/Decay/Sustain/Release)                4B/RW

SPUADSRVOL0  equ 0x1C0C ; $1F801C0C..$1F801C0D SPU Voice Registers: Voice 0  ADSR Current Volume                             2B/RW
SPUADSRVOL1  equ 0x1C1C ; $1F801C1C..$1F801C1D SPU Voice Registers: Voice 1  ADSR Current Volume                             2B/RW
SPUADSRVOL2  equ 0x1C2C ; $1F801C2C..$1F801C2D SPU Voice Registers: Voice 2  ADSR Current Volume                             2B/RW
SPUADSRVOL3  equ 0x1C3C ; $1F801C3C..$1F801C3D SPU Voice Registers: Voice 3  ADSR Current Volume                             2B/RW
SPUADSRVOL4  equ 0x1C4C ; $1F801C4C..$1F801C4D SPU Voice Registers: Voice 4  ADSR Current Volume                             2B/RW
SPUADSRVOL5  equ 0x1C5C ; $1F801C5C..$1F801C5D SPU Voice Registers: Voice 5  ADSR Current Volume                             2B/RW
SPUADSRVOL6  equ 0x1C6C ; $1F801C6C..$1F801C6D SPU Voice Registers: Voice 6  ADSR Current Volume                             2B/RW
SPUADSRVOL7  equ 0x1C7C ; $1F801C7C..$1F801C7D SPU Voice Registers: Voice 7  ADSR Current Volume                             2B/RW
SPUADSRVOL8  equ 0x1C8C ; $1F801C8C..$1F801C8D SPU Voice Registers: Voice 8  ADSR Current Volume                             2B/RW
SPUADSRVOL9  equ 0x1C9C ; $1F801C9C..$1F801C9D SPU Voice Registers: Voice 9  ADSR Current Volume                             2B/RW
SPUADSRVOL10 equ 0x1CAC ; $1F801CAC..$1F801CAD SPU Voice Registers: Voice 10 ADSR Current Volume                             2B/RW
SPUADSRVOL11 equ 0x1CBC ; $1F801CBC..$1F801CBD SPU Voice Registers: Voice 11 ADSR Current Volume                             2B/RW
SPUADSRVOL12 equ 0x1CCC ; $1F801CCC..$1F801CCD SPU Voice Registers: Voice 12 ADSR Current Volume                             2B/RW
SPUADSRVOL13 equ 0x1CDC ; $1F801CDC..$1F801CDD SPU Voice Registers: Voice 13 ADSR Current Volume                             2B/RW
SPUADSRVOL14 equ 0x1CEC ; $1F801CEC..$1F801CED SPU Voice Registers: Voice 14 ADSR Current Volume                             2B/RW
SPUADSRVOL15 equ 0x1CFC ; $1F801CFC..$1F801CFD SPU Voice Registers: Voice 15 ADSR Current Volume                             2B/RW
SPUADSRVOL16 equ 0x1D0C ; $1F801D0C..$1F801D0D SPU Voice Registers: Voice 16 ADSR Current Volume                             2B/RW
SPUADSRVOL17 equ 0x1D1C ; $1F801D1C..$1F801D1D SPU Voice Registers: Voice 17 ADSR Current Volume                             2B/RW
SPUADSRVOL18 equ 0x1D2C ; $1F801D2C..$1F801D2D SPU Voice Registers: Voice 18 ADSR Current Volume                             2B/RW
SPUADSRVOL19 equ 0x1D3C ; $1F801D3C..$1F801D3D SPU Voice Registers: Voice 19 ADSR Current Volume                             2B/RW
SPUADSRVOL20 equ 0x1D4C ; $1F801D4C..$1F801D4D SPU Voice Registers: Voice 20 ADSR Current Volume                             2B/RW
SPUADSRVOL21 equ 0x1D5C ; $1F801D5C..$1F801D5D SPU Voice Registers: Voice 21 ADSR Current Volume                             2B/RW
SPUADSRVOL22 equ 0x1D6C ; $1F801D6C..$1F801D6D SPU Voice Registers: Voice 22 ADSR Current Volume                             2B/RW
SPUADSRVOL23 equ 0x1D7C ; $1F801D7C..$1F801D7D SPU Voice Registers: Voice 23 ADSR Current Volume                             2B/RW

SPULOOP0  equ 0x1C0E ; $1F801C0E..$1F801C0F SPU Voice Registers: Voice 0  ADPCM Repeat Address                               2B/RW
SPULOOP1  equ 0x1C1E ; $1F801C1E..$1F801C1F SPU Voice Registers: Voice 1  ADPCM Repeat Address                               2B/RW
SPULOOP2  equ 0x1C2E ; $1F801C2E..$1F801C2F SPU Voice Registers: Voice 2  ADPCM Repeat Address                               2B/RW
SPULOOP3  equ 0x1C3E ; $1F801C3E..$1F801C3F SPU Voice Registers: Voice 3  ADPCM Repeat Address                               2B/RW
SPULOOP4  equ 0x1C4E ; $1F801C4E..$1F801C4F SPU Voice Registers: Voice 4  ADPCM Repeat Address                               2B/RW
SPULOOP5  equ 0x1C5E ; $1F801C5E..$1F801C5F SPU Voice Registers: Voice 5  ADPCM Repeat Address                               2B/RW
SPULOOP6  equ 0x1C6E ; $1F801C6E..$1F801C6F SPU Voice Registers: Voice 6  ADPCM Repeat Address                               2B/RW
SPULOOP7  equ 0x1C7E ; $1F801C7E..$1F801C7F SPU Voice Registers: Voice 7  ADPCM Repeat Address                               2B/RW
SPULOOP8  equ 0x1C8E ; $1F801C8E..$1F801C8F SPU Voice Registers: Voice 8  ADPCM Repeat Address                               2B/RW
SPULOOP9  equ 0x1C9E ; $1F801C9E..$1F801C9F SPU Voice Registers: Voice 9  ADPCM Repeat Address                               2B/RW
SPULOOP10 equ 0x1CAE ; $1F801CAE..$1F801CAF SPU Voice Registers: Voice 10 ADPCM Repeat Address                               2B/RW
SPULOOP11 equ 0x1CBE ; $1F801CBE..$1F801CBF SPU Voice Registers: Voice 11 ADPCM Repeat Address                               2B/RW
SPULOOP12 equ 0x1CCE ; $1F801CCE..$1F801CCF SPU Voice Registers: Voice 12 ADPCM Repeat Address                               2B/RW
SPULOOP13 equ 0x1CDE ; $1F801CDE..$1F801CDF SPU Voice Registers: Voice 13 ADPCM Repeat Address                               2B/RW
SPULOOP14 equ 0x1CEE ; $1F801CEE..$1F801CEF SPU Voice Registers: Voice 14 ADPCM Repeat Address                               2B/RW
SPULOOP15 equ 0x1CFE ; $1F801CFE..$1F801CFF SPU Voice Registers: Voice 15 ADPCM Repeat Address                               2B/RW
SPULOOP16 equ 0x1D0E ; $1F801D0E..$1F801D0F SPU Voice Registers: Voice 16 ADPCM Repeat Address                               2B/RW
SPULOOP17 equ 0x1D1E ; $1F801D1E..$1F801D1F SPU Voice Registers: Voice 17 ADPCM Repeat Address                               2B/RW
SPULOOP18 equ 0x1D2E ; $1F801D2E..$1F801D2F SPU Voice Registers: Voice 18 ADPCM Repeat Address                               2B/RW
SPULOOP19 equ 0x1D3E ; $1F801D3E..$1F801D3F SPU Voice Registers: Voice 19 ADPCM Repeat Address                               2B/RW
SPULOOP20 equ 0x1D4E ; $1F801D4E..$1F801D4F SPU Voice Registers: Voice 20 ADPCM Repeat Address                               2B/RW
SPULOOP21 equ 0x1D5E ; $1F801D5E..$1F801D5F SPU Voice Registers: Voice 21 ADPCM Repeat Address                               2B/RW
SPULOOP22 equ 0x1D6E ; $1F801D6E..$1F801D6F SPU Voice Registers: Voice 22 ADPCM Repeat Address                               2B/RW
SPULOOP23 equ 0x1D7E ; $1F801D7E..$1F801D7F SPU Voice Registers: Voice 23 ADPCM Repeat Address                               2B/RW

; SPU Control Registers
SPUMVOL  equ 0x1D80 ; $1F801D80..$1F801D83 SPU Control Registers: Main Volume Left/Right                                     4B/RW
SPUEVOL  equ 0x1D84 ; $1F801D84..$1F801D87 SPU Control Registers: Echo/Reverb Output Volume Left/Right                       4B/RW
SPUKON   equ 0x1D88 ; $1F801D88..$1F801D8B SPU Control Registers: Voice 0..23 Key ON  (Start Attack/Decay/Sustain)           4B/W
SPUKOFF  equ 0x1D8C ; $1F801D8C..$1F801D8F SPU Control Registers: Voice 0..23 Key OFF (Start Release)                        4B/W
SPUPMON  equ 0x1D90 ; $1F801D90..$1F801D93 SPU Control Registers: Voice 1..23 Pitch Modulation Enable Flags                  4B/RW
SPUNON   equ 0x1D94 ; $1F801D94..$1F801D97 SPU Control Registers: Voice 0..23 Channel Noise Enable Flags                     4B/RW
SPUEON   equ 0x1D98 ; $1F801D98..$1F801D9B SPU Control Registers: Voice 0..23 Channel Echo/Reverb Enable Flags               4B/RW
SPUENDX  equ 0x1D9C ; $1F801D9C..$1F801D9F SPU Control Registers: Voice 0..23 Channel ON/OFF (Status)                        4B/R
SPUUNK1  equ 0x1DA0 ; $1F801DA0..$1F801DA1 SPU Control Registers: Unknown/Unused                                             2B/RW
SPUEBASE equ 0x1DA2 ; $1F801DA2..$1F801DA3 SPU Control Registers: Sound RAM Echo/Reverb Work Area Start Address              2B/RW
SPUIRQ   equ 0x1DA4 ; $1F801DA4..$1F801DA5 SPU Control Registers: Sound RAM IRQ Address                                      2B/RW
SPUDBASE equ 0x1DA6 ; $1F801DA6..$1F801DA7 SPU Control Registers: Sound RAM Data Transfer Address                            2B/RW
SPUDFIFO equ 0x1DA8 ; $1F801DA8..$1F801DA9 SPU Control Registers: Sound RAM Data Transfer Fifo                               2B/RW
SPUCNT   equ 0x1DAA ; $1F801DAA..$1F801DAB SPU Control Registers: SPU Control Register                                       2B/RW
SPUDCNT  equ 0x1DAC ; $1F801DAC..$1F801DAD SPU Control Registers: Sound RAM Data Transfer Control (Should Be $0004)          2B/RW
SPUSTAT  equ 0x1DAE ; $1F801DAE..$1F801DAF SPU Control Registers: SPU Status Register                                        2B/R
SPUCDVOL equ 0x1DB0 ; $1F801DB0..$1F801DB3 SPU Control Registers: CD Volume Left/Right (CD-DA & Compressed XA-ADPCM)         4B/RW
SPUEXVOL equ 0x1DB4 ; $1F801DB4..$1F801DB7 SPU Control Registers: External Audio Input Volume Left/Right                     4B/RW
SPUCMVOL equ 0x1DB8 ; $1F801DB8..$1F801DBB SPU Control Registers: Current Main Volume Left/Right                             4B/RW
SPUUNK2  equ 0x1DBC ; $1F801DBC..$1F801DBF SPU Control Registers: Unknown/Unused                                             4B/RW

; SPU Echo/Reverb Configuration Area
SPUDAPF1  equ 0x1DC0 ; $1F801DC0..$1F801DC1 SPU Echo/Reverb Configuration Area: APF Offset 1                                 2B/RW
SPUDAPF2  equ 0x1DC2 ; $1F801DC2..$1F801DC3 SPU Echo/Reverb Configuration Area: APF Offset 2                                 2B/RW
SPUVIIR   equ 0x1DC4 ; $1F801DC4..$1F801DC5 SPU Echo/Reverb Configuration Area: Reflection Volume 1                          2B/RW
SPUVCOMB1 equ 0x1DC6 ; $1F801DC6..$1F801DC7 SPU Echo/Reverb Configuration Area: Comb Volume 1                                2B/RW
SPUVCOMB2 equ 0x1DC8 ; $1F801DC8..$1F801DC9 SPU Echo/Reverb Configuration Area: Comb Volume 2                                2B/RW
SPUVCOMB3 equ 0x1DCA ; $1F801DCA..$1F801DCB SPU Echo/Reverb Configuration Area: Comb Volume 3                                2B/RW
SPUVCOMB4 equ 0x1DCC ; $1F801DCC..$1F801DCD SPU Echo/Reverb Configuration Area: Comb Volume 4                                2B/RW
SPUVWALL  equ 0x1DCE ; $1F801DCE..$1F801DCF SPU Echo/Reverb Configuration Area: Reflection Volume 2                          2B/RW
SPUVAPF1  equ 0x1DD0 ; $1F801DD0..$1F801DD1 SPU Echo/Reverb Configuration Area: APF Volume 1                                 2B/RW
SPUVAPF2  equ 0x1DD2 ; $1F801DD2..$1F801DD3 SPU Echo/Reverb Configuration Area: APF Volume 2                                 2B/RW
SPUMSAME  equ 0x1DD4 ; $1F801DD4..$1F801DD7 SPU Echo/Reverb Configuration Area: Same Side Reflection Address 1 Left/Right    4B/RW
SPUMCOMB1 equ 0x1DD8 ; $1F801DD8..$1F801DDB SPU Echo/Reverb Configuration Area: Comb Address 1 Left/Right                    4B/RW
SPUMCOMB2 equ 0x1DDC ; $1F801DDC..$1F801DDF SPU Echo/Reverb Configuration Area: Comb Address 2 Left/Right                    4B/RW
SPUDSAME  equ 0x1DE0 ; $1F801DE0..$1F801DE3 SPU Echo/Reverb Configuration Area: Same Side Reflection Address 2 Left/Right    4B/RW
SPUMDIFF  equ 0x1DE4 ; $1F801DE4..$1F801DE7 SPU Echo/Reverb Configuration Area: Diff Side Reflection Address 1 Left/Right    4B/RW
SPUMCOMB3 equ 0x1DE8 ; $1F801DE8..$1F801DEB SPU Echo/Reverb Configuration Area: Comb Address 3 Left/Right                    4B/RW
SPUMCOMB4 equ 0x1DEC ; $1F801DEC..$1F801DEF SPU Echo/Reverb Configuration Area: Comb Address 4 Left/Right                    4B/RW
SPUDDIFF  equ 0x1DF0 ; $1F801DF0..$1F801DF3 SPU Echo/Reverb Configuration Area: Diff Side Reflection Address 2 Left/Right    4B/RW
SPUMAPF1  equ 0x1DF4 ; $1F801DF4..$1F801DF7 SPU Echo/Reverb Configuration Area: APF Address 1 Left/Right                     4B/RW
SPUMAPF2  equ 0x1DF8 ; $1F801DF8..$1F801DFB SPU Echo/Reverb Configuration Area: APF Address 2 Left/Right                     4B/RW
SPUVIN    equ 0x1DFC ; $1F801DFC..$1F801DFF SPU Echo/Reverb Configuration Area: Input Volume Left/Right                      4B/RW

; SPU Internal Registers
SPUCVOL0  equ 0x1E00 ; $1F801E00..$1F801E03 SPU Internal Registers: Voice 0  Current Volume Left/Right                       4B/RW
SPUCVOL1  equ 0x1E04 ; $1F801E04..$1F801E07 SPU Internal Registers: Voice 1  Current Volume Left/Right                       4B/RW
SPUCVOL2  equ 0x1E08 ; $1F801E08..$1F801E0B SPU Internal Registers: Voice 2  Current Volume Left/Right                       4B/RW
SPUCVOL3  equ 0x1E0C ; $1F801E0C..$1F801E0F SPU Internal Registers: Voice 3  Current Volume Left/Right                       4B/RW
SPUCVOL4  equ 0x1E10 ; $1F801E10..$1F801E13 SPU Internal Registers: Voice 4  Current Volume Left/Right                       4B/RW
SPUCVOL5  equ 0x1E14 ; $1F801E14..$1F801E17 SPU Internal Registers: Voice 5  Current Volume Left/Right                       4B/RW
SPUCVOL6  equ 0x1E18 ; $1F801E18..$1F801E1B SPU Internal Registers: Voice 6  Current Volume Left/Right                       4B/RW
SPUCVOL7  equ 0x1E1C ; $1F801E1C..$1F801E1F SPU Internal Registers: Voice 7  Current Volume Left/Right                       4B/RW
SPUCVOL8  equ 0x1E20 ; $1F801E20..$1F801E23 SPU Internal Registers: Voice 8  Current Volume Left/Right                       4B/RW
SPUCVOL9  equ 0x1E24 ; $1F801E24..$1F801E27 SPU Internal Registers: Voice 9  Current Volume Left/Right                       4B/RW
SPUCVOL10 equ 0x1E28 ; $1F801E28..$1F801E2B SPU Internal Registers: Voice 10 Current Volume Left/Right                       4B/RW
SPUCVOL11 equ 0x1E2C ; $1F801E2C..$1F801E2F SPU Internal Registers: Voice 11 Current Volume Left/Right                       4B/RW
SPUCVOL12 equ 0x1E30 ; $1F801E30..$1F801E33 SPU Internal Registers: Voice 12 Current Volume Left/Right                       4B/RW
SPUCVOL13 equ 0x1E34 ; $1F801E34..$1F801E37 SPU Internal Registers: Voice 13 Current Volume Left/Right                       4B/RW
SPUCVOL14 equ 0x1E38 ; $1F801E38..$1F801E3B SPU Internal Registers: Voice 14 Current Volume Left/Right                       4B/RW
SPUCVOL15 equ 0x1E3C ; $1F801E3C..$1F801E3F SPU Internal Registers: Voice 15 Current Volume Left/Right                       4B/RW
SPUCVOL16 equ 0x1E40 ; $1F801E40..$1F801E43 SPU Internal Registers: Voice 16 Current Volume Left/Right                       4B/RW
SPUCVOL17 equ 0x1E44 ; $1F801E44..$1F801E47 SPU Internal Registers: Voice 17 Current Volume Left/Right                       4B/RW
SPUCVOL18 equ 0x1E48 ; $1F801E48..$1F801E4B SPU Internal Registers: Voice 18 Current Volume Left/Right                       4B/RW
SPUCVOL19 equ 0x1E4C ; $1F801E4C..$1F801E4F SPU Internal Registers: Voice 19 Current Volume Left/Right                       4B/RW
SPUCVOL20 equ 0x1E50 ; $1F801E50..$1F801E53 SPU Internal Registers: Voice 20 Current Volume Left/Right                       4B/RW
SPUCVOL21 equ 0x1E54 ; $1F801E54..$1F801E57 SPU Internal Registers: Voice 21 Current Volume Left/Right                       4B/RW
SPUCVOL22 equ 0x1E58 ; $1F801E58..$1F801E5B SPU Internal Registers: Voice 22 Current Volume Left/Right                       4B/RW
SPUCVOL23 equ 0x1E5C ; $1F801E5C..$1F801E5F SPU Internal Registers: Voice 23 Current Volume Left/Right                       4B/RW

SPUUNK3   equ 0x1E60 ; $1F801E60..$1F801E7F SPU Internal Registers: Unknown/Unused                                          32B/RW
SPUUNK4   equ 0x1E80 ; $1F801E80..$1F801FFF SPU Internal Registers: Unknown/Unused (Read: $FF Filled)                      384B/RW

; Expansion Region 2 (Default 128B, Max 8KB)
EXP2_BASE equ 0x1F802000 ; $1F802000..$1F80207F Expansion Region 2: Expansion Region (8-Bit Data BUS)                      128B/RW

; Expansion Region 3 (Default 1B, Max 2MB)
EXP3_BASE equ 0x1FA00000 ; $1FA00000..$1FA00000 Expansion Region 3: Expansion Region (Not Used By BIOS Or Any PSX Games)     1B/RW

; BIOS Region (Default 512KB, Max 4MB)
BIOS_BASE equ 0x1FC00000 ; $1FC00000..$1FC001FF BIOS Region: BIOS ROM (Reset Entrypoint At $BFC00000)                     512KB/R

; Memory Control 3 (Cache Control)
CACHE_CTRL equ 0xFFFE0130 ; $FFFE0130..$FFFE0133 Memory Control 3: Cache Control                                             4B/RW

; Macros
.macro CLIOH,IOREG ; Clear I/O Port Register Half (A0 = IO_BASE 0x1F800000)
  sh r0,IOREG(a0)  ; I/O Port Register Half = 0
.endmacro

.macro CLIOW,IOREG ; Clear I/O Port Register Word (A0 = IO_BASE 0x1F800000)
  sw r0,IOREG(a0)  ; I/O Port Register Word = 0
.endmacro

.macro RDIOH,IOREG,REG ; Read I/O Port Register Half (A0 = IO_BASE 0x1F800000)
  lhu REG,IOREG(a0)    ; Register = I/O Port Register Half
  nop ; Delay Slot
.endmacro

.macro RDIOW,IOREG,REG ; Read I/O Port Register Word (A0 = IO_BASE 0x1F800000)
  lw REG,IOREG(a0)     ; Register = I/O Port Register Word
  nop ; Delay Slot
.endmacro

.macro WRIOH,IOREG,DATA ; Write I/O Port Register Half (A0 = IO_BASE 0x1F800000)
  li t0,DATA      ; T0 = DATA Half
  sh t0,IOREG(a0) ; I/O Port Register Half = T0
.endmacro

.macro WRIOW,IOREG,DATA ; Write I/O Port Register Word (A0 = IO_BASE 0x1F800000)
  li t0,DATA      ; T0 = DATA Word
  sw t0,IOREG(a0) ; I/O Port Register Word = T0
.endmacro