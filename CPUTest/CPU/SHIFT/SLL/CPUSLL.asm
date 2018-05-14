; PSX 'Bare Metal' CPU Word Shift Left Logical (0..31) Test Demo by krom (Peter Lemon):
.psx
.create "CPUSLL.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros

.macro PrintString,X,Y,WIDTH,HEIGHT,FONT,STRING,LENGTH ; Print Text String To VRAM Using Width,Height Font At X,Y Position
  la a1,FONT   ; A1 = Font Address
  la a2,STRING ; A2 = Text Address
  li t0,LENGTH ; T0 = Number of Text Characters to Print
  li t1,X ; T1 = X Position
  li t2,Y ; T2 = Y Position

  DrawChars:
    ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height

    ; Write GP0 Command Word (Command)
    li t3,0xA0<<24 ; T3 = DATA Word
    sw t3,GP0(a0) ; I/O Port Register Word = T3

    ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)
    sll t3,t2,16 ; T3 = Y<<16
    addu t3,t1 ; T3 = DATA Word (Y<<16)+X
    sw t3,GP0(a0) ; I/O Port Register Word = T3

    ; Write GP0  Packet Word (Width+Height:  Width Counted In Halfwords)
    li t3,(HEIGHT<<16)+WIDTH ; T3 = DATA Word
    sw t3,GP0(a0) ; I/O Port Register Word = T3

    ; Write GP0  Packet Word (Data)
    lbu a3,0(a2) ; A3 = Next Text Character
    li t3,(WIDTH*HEIGHT/2)-1 ; T3 = Data Copy Word Count
    sll a3,7 ; A3 *= 128
    addu a3,a1 ; A3 = Texture RAM Font Offset
    CopyTexture:
      lw t4,0(a3) ; T4 = DATA Word
      addiu a3,4  ; A3 += 4 (Delay Slot)
      sw t4,GP0(a0) ; Write GP0 Packet Word
      bnez t3,CopyTexture ; IF (T3 != 0) Copy Texture
      subiu t3,1 ; T3-- (Delay Slot)

    addiu a2,1 ; Increment Text Offset
    addiu t1,WIDTH ; Add Width To X Position
    bnez t0,DrawChars ; Continue to Print Characters
    subiu t0,1 ; Subtract Number of Text Characters to Print (Delay Slot)
.endmacro

.macro PrintValue,X,Y,WIDTH,HEIGHT,FONT,STRING,LENGTH ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
  la a1,FONT   ; A1 = Font Address
  la a2,STRING+LENGTH ; A2 = Text Address
  li t0,LENGTH ; T0 = Number of Text Characters to Print
  li t1,X ; T1 = X Position
  li t2,Y ; T2 = Y Position

  DrawHEXChars:
    lbu t3,0(a2) ; T3 = Next 2 HEX Chars
    subiu a2,1 ; Decrement Text Offset

    srl t4,t3,4 ; T4 = 2nd Nibble
    andi t4,0xF
    subiu t5,t4,9
    bgtz t5,HEXLetters
    addiu t4,0x30 ; Delay Slot
    j HEXEnd
    nop ; Delay Slot

    HEXLetters:
    addiu t4,7
    HEXEnd:

    sll a3,t4,7 ; Add Shift to Correct Position in Font (*128: WIDTH*HEIGHT*BYTES_PER_PIXEL)
    addu a3,a1 ; A3 = Texture RAM Font Offset

    ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height

    ; Write GP0 Command Word (Command)
    li t4,0xA0<<24 ; T4 = DATA Word
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)
    sll t4,t2,16 ; T4 = Y<<16
    addu t4,t1 ; T4 = DATA Word (Y<<16)+X
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Width+Height:  Width Counted In Halfwords)
    li t4,(HEIGHT<<16)+WIDTH ; T4 = DATA Word
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Data)
    li t4,(WIDTH*HEIGHT/2)-1 ; T4 = Data Copy Word Count
    CopyTextureA:
      lw t5,0(a3) ; T5 = DATA Word
      addiu a3,4  ; A3 += 4 (Delay Slot)
      sw t5,GP0(a0) ; Write GP0 Packet Word
      bnez t4,CopyTextureA ; IF (T4 != 0) Copy Texture A
      subiu t4,1 ; T4-- (Delay Slot)

    addiu t1,WIDTH ; Add Width To X Position

    andi t4,t3,0xF ; T4 = 1st Nibble
    subiu t5,t4,9
    bgtz t5,HEXLettersB
    addiu t4,0x30 ; Delay Slot
    j HEXEndB
    nop ; Delay Slot

    HEXLettersB:
    addiu t4,7
    HEXEndB:

    sll a3,t4,7 // Add Shift to Correct Position in Font (*128: WIDTH*HEIGHT*BYTES_PER_PIXEL)
    addu a3,a1 ; A3 = Texture RAM Font Offset

    ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height

    ; Write GP0 Command Word (Command)
    li t4,0xA0<<24 ; T4 = DATA Word
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)
    sll t4,t2,16 ; T4 = Y<<16
    addu t4,t1 ; T4 = DATA Word (Y<<16)+X
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Width+Height:  Width Counted In Halfwords)
    li t4,(HEIGHT<<16)+WIDTH ; T4 = DATA Word
    sw t4,GP0(a0) ; I/O Port Register Word = T4

    ; Write GP0  Packet Word (Data)
    li t4,(WIDTH*HEIGHT/2)-1 ; T4 = Data Copy Word Count
    CopyTextureB:
      lw t5,0(a3) ; T5 = DATA Word
      addiu a3,4  ; A3 += 4 (Delay Slot)
      sw t5,GP0(a0) ; Write GP0 Packet Word
      bnez t4,CopyTextureB ; IF (T4 != 0) Copy Texture B
      subiu t4,1 ; T4-- (Delay Slot)

    addiu t1,WIDTH ; Add Width To X Position
    bnez t0,DrawHEXChars ; Continue to Print Characters
    subiu t0,1 ; Subtract Number of Text Characters to Print (Delay Slot)
.endmacro

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup Screen Mode
WRGP1 GPURESET,0  ; Write GP1 Command Word (Reset GPU)
WRGP1 GPUDISPEN,0 ; Write GP1 Command Word (Enable Display)
WRGP1 GPUDISPM,HRES320+VRES240+BPP15+VNTSC ; Write GP1 Command Word (Set Display Mode: 320x240, 15BPP, NTSC)
WRGP1 GPUDISPH,0xC60260 ; Write GP1 Command Word (Horizontal Display Range 608..3168)
WRGP1 GPUDISPV,0x042018 ; Write GP1 Command Word (Vertical Display Range 24..264)

; Setup Drawing Area
WRGP0 GPUDRAWM,0x000400   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BD3F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=319, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

; Clear Screen
FillRectVRAM 0x000000, 0,0, 319,239 ; Fill Rectangle In VRAM: Color, X,Y, Width,Height

; Print Header Text
PrintString  40,8, 8,8, FontRed,RTHEX,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 144,8, 8,8, FontRed,SADEC,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,8, 8,8, FontRed,RDHEX,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 280,8, 8,8, FontRed,TEST,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 0,16, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 8,24, 8,8, FontRed,SLL,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,0 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTWORD0,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK0 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS0 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND0
nop // Delay Slot
SLLPASS0:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND0:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,32, 8,8, FontBlack,TEXTWORD1,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK1 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS1 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND1
nop // Delay Slot
SLLPASS1:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND1:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,2 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,40, 8,8, FontBlack,TEXTWORD2,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK2 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS2 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND2
nop // Delay Slot
SLLPASS2:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND2:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,3 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,48, 8,8, FontBlack,TEXTWORD3,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK3 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS3 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND3
nop // Delay Slot
SLLPASS3:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND3:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,4 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,56, 8,8, FontBlack,TEXTWORD4,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK4 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS4 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND4
nop // Delay Slot
SLLPASS4:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND4:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,5 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,64, 8,8, FontBlack,TEXTWORD5,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK5 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS5 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND5
nop // Delay Slot
SLLPASS5:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND5:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,6 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,72, 8,8, FontBlack,TEXTWORD6,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK6 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS6 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND6
nop // Delay Slot
SLLPASS6:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND6:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,7 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,80, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,80, 8,8, FontBlack,TEXTWORD7,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,80, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK7 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS7 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,80, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND7
nop // Delay Slot
SLLPASS7:
PrintString 280,80, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND7:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,8 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,88, 8,8, FontBlack,TEXTWORD8,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK8 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS8 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND8
nop // Delay Slot
SLLPASS8:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND8:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,9 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,96, 8,8, FontBlack,TEXTWORD9,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SLLCHECK9 ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS9 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND9
nop // Delay Slot
SLLPASS9:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND9:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,10 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,104, 8,8, FontBlack,TEXTWORD10,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK10 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS10 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND10
nop // Delay Slot
SLLPASS10:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND10:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,11 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,112, 8,8, FontBlack,TEXTWORD11,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK11 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS11 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND11
nop // Delay Slot
SLLPASS11:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND11:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,12 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,120, 8,8, FontBlack,TEXTWORD12,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK12 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS12 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND12
nop // Delay Slot
SLLPASS12:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND12:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,13 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,128, 8,8, FontBlack,TEXTWORD13,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK13 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS13 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND13
nop // Delay Slot
SLLPASS13:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND13:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,14 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,136, 8,8, FontBlack,TEXTWORD14,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK14 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS14 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND14
nop // Delay Slot
SLLPASS14:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND14:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,15 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,144, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,144, 8,8, FontBlack,TEXTWORD15,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,144, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK15 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS15 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,144, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND15
nop // Delay Slot
SLLPASS15:
PrintString 280,144, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND15:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,16 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,152, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,152, 8,8, FontBlack,TEXTWORD16,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,152, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK16 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS16 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,152, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND16
nop // Delay Slot
SLLPASS16:
PrintString 280,152, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND16:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,17 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,160, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,160, 8,8, FontBlack,TEXTWORD17,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,160, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK17 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS17 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,160, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND17
nop // Delay Slot
SLLPASS17:
PrintString 280,160, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND17:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,18 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,168, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,168, 8,8, FontBlack,TEXTWORD18,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,168, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK18 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS18 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,168, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND18
nop // Delay Slot
SLLPASS18:
PrintString 280,168, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND18:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,19 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,176, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,176, 8,8, FontBlack,TEXTWORD19,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,176, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK19 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS19 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,176, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND19
nop // Delay Slot
SLLPASS19:
PrintString 280,176, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND19:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,20 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,184, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,184, 8,8, FontBlack,TEXTWORD20,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,184, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK20 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS20 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,184, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND20
nop // Delay Slot
SLLPASS20:
PrintString 280,184, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND20:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,21 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,192, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,192, 8,8, FontBlack,TEXTWORD21,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,192, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK21 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS21 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,192, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND21
nop // Delay Slot
SLLPASS21:
PrintString 280,192, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND21:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,22 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,200, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,200, 8,8, FontBlack,TEXTWORD22,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,200, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK22 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS22 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,200, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND22
nop // Delay Slot
SLLPASS22:
PrintString 280,200, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND22:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,23 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,208, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,208, 8,8, FontBlack,TEXTWORD23,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,208, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK23 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS23 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,208, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND23
nop // Delay Slot
SLLPASS23:
PrintString 280,208, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND23:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,24 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,216, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,216, 8,8, FontBlack,TEXTWORD24,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,216, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK24 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS24 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,216, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND24
nop // Delay Slot
SLLPASS24:
PrintString 280,216, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND24:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,25 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,224, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,224, 8,8, FontBlack,TEXTWORD25,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,224, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK25 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS25 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,224, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND25
nop // Delay Slot
SLLPASS25:
PrintString 280,224, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND25:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,26 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,232, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,232, 8,8, FontBlack,TEXTWORD26,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,232, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK26 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS26 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,232, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND26
nop // Delay Slot
SLLPASS26:
PrintString 280,232, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND26:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,27 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,240, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,240, 8,8, FontBlack,TEXTWORD27,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,240, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK27 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS27 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,240, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND27
nop // Delay Slot
SLLPASS27:
PrintString 280,240, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND27:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,28 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,248, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,248, 8,8, FontBlack,TEXTWORD28,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,248, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK28 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS28 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,248, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND28
nop // Delay Slot
SLLPASS28:
PrintString 280,248, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND28:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,29 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,256, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,256, 8,8, FontBlack,TEXTWORD29,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,256, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK29 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS29 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,256, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND29
nop // Delay Slot
SLLPASS29:
PrintString 280,256, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND29:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,30 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,264, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,264, 8,8, FontBlack,TEXTWORD30,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,264, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK30 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS30 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,264, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND30
nop // Delay Slot
SLLPASS30:
PrintString 280,264, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND30:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
sll t0,31 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,272, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,272, 8,8, FontBlack,TEXTWORD31,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,272, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLCHECK31 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLPASS31 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,272, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLEND31
nop // Delay Slot
SLLPASS31:
PrintString 280,272, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLEND31:


PrintString 0,280, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

SLL:
  .db "SLL"

RDHEX:
  .db "RD Hex"
RTHEX:
  .db "RT Hex"
SADEC:
  .db "SA Dec"
TEST:
  .db "Test"
FAIL:
  .db "FAIL"
PASS:
  .db "PASS"

DOLLAR:
  .db "$"

TEXTWORD0:
  .db "0"
TEXTWORD1:
  .db "1"
TEXTWORD2:
  .db "2"
TEXTWORD3:
  .db "3"
TEXTWORD4:
  .db "4"
TEXTWORD5:
  .db "5"
TEXTWORD6:
  .db "6"
TEXTWORD7:
  .db "7"
TEXTWORD8:
  .db "8"
TEXTWORD9:
  .db "9"
TEXTWORD10:
  .db "10"
TEXTWORD11:
  .db "11"
TEXTWORD12:
  .db "12"
TEXTWORD13:
  .db "13"
TEXTWORD14:
  .db "14"
TEXTWORD15:
  .db "15"
TEXTWORD16:
  .db "16"
TEXTWORD17:
  .db "17"
TEXTWORD18:
  .db "18"
TEXTWORD19:
  .db "19"
TEXTWORD20:
  .db "20"
TEXTWORD21:
  .db "21"
TEXTWORD22:
  .db "22"
TEXTWORD23:
  .db "23"
TEXTWORD24:
  .db "24"
TEXTWORD25:
  .db "25"
TEXTWORD26:
  .db "26"
TEXTWORD27:
  .db "27"
TEXTWORD28:
  .db "28"
TEXTWORD29:
  .db "29"
TEXTWORD30:
  .db "30"
TEXTWORD31:
  .db "31"

PAGEBREAK:
  .db "----------------------------------------"

.align 4 ; Align 32-Bit
VALUEWORD:
  .dw -123456789

SLLCHECK0:
  .dw 0xF8A432EB
SLLCHECK1:
  .dw 0xF14865D6
SLLCHECK2:
  .dw 0xE290CBAC
SLLCHECK3:
  .dw 0xC5219758
SLLCHECK4:
  .dw 0x8A432EB0
SLLCHECK5:
  .dw 0x14865D60
SLLCHECK6:
  .dw 0x290CBAC0
SLLCHECK7:
  .dw 0x52197580
SLLCHECK8:
  .dw 0xA432EB00
SLLCHECK9:
  .dw 0x4865D600
SLLCHECK10:
  .dw 0x90CBAC00
SLLCHECK11:
  .dw 0x21975800
SLLCHECK12:
  .dw 0x432EB000
SLLCHECK13:
  .dw 0x865D6000
SLLCHECK14:
  .dw 0x0CBAC000
SLLCHECK15:
  .dw 0x19758000
SLLCHECK16:
  .dw 0x32EB0000
SLLCHECK17:
  .dw 0x65D60000
SLLCHECK18:
  .dw 0xCBAC0000
SLLCHECK19:
  .dw 0x97580000
SLLCHECK20:
  .dw 0x2EB00000
SLLCHECK21:
  .dw 0x5D600000
SLLCHECK22:
  .dw 0xBAC00000
SLLCHECK23:
  .dw 0x75800000
SLLCHECK24:
  .dw 0xEB000000
SLLCHECK25:
  .dw 0xD6000000
SLLCHECK26:
  .dw 0xAC000000
SLLCHECK27:
  .dw 0x58000000
SLLCHECK28:
  .dw 0xB0000000
SLLCHECK29:
  .dw 0x60000000
SLLCHECK30:
  .dw 0xC0000000
SLLCHECK31:
  .dw 0x80000000

RDWORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close