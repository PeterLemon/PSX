; PSX 'Bare Metal' CPU Word Shift Right Arithmetic Variable (0..31) Test Demo by krom (Peter Lemon):
.psx
.create "CPUSRAV.bin", 0x80010000

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
PrintString 144,8, 8,8, FontRed,RSDEC,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,8, 8,8, FontRed,RDHEX,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 280,8, 8,8, FontRed,TEST,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 0,16, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 8,24, 8,8, FontRed,SRAV,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,0    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTWORD0,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK0 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS0 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND0
nop ; Delay Slot
SRAVPASS0:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND0:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,1    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,32, 8,8, FontBlack,TEXTWORD1,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK1 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS1 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND1
nop ; Delay Slot
SRAVPASS1:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND1:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,2    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,40, 8,8, FontBlack,TEXTWORD2,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK2 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS2 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND2
nop ; Delay Slot
SRAVPASS2:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND2:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,3    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,48, 8,8, FontBlack,TEXTWORD3,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK3 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS3 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND3
nop ; Delay Slot
SRAVPASS3:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND3:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,4    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,56, 8,8, FontBlack,TEXTWORD4,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK4 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS4 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND4
nop ; Delay Slot
SRAVPASS4:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND4:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,5    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,64, 8,8, FontBlack,TEXTWORD5,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK5 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS5 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND5
nop ; Delay Slot
SRAVPASS5:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND5:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,6    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,72, 8,8, FontBlack,TEXTWORD6,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK6 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS6 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND6
nop ; Delay Slot
SRAVPASS6:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND6:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,7    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,80, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,80, 8,8, FontBlack,TEXTWORD7,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,80, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK7 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS7 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,80, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND7
nop ; Delay Slot
SRAVPASS7:
PrintString 280,80, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND7:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,8    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,88, 8,8, FontBlack,TEXTWORD8,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK8 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS8 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND8
nop ; Delay Slot
SRAVPASS8:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND8:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,9    ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,96, 8,8, FontBlack,TEXTWORD9,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SRAVCHECK9 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS9 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND9
nop ; Delay Slot
SRAVPASS9:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND9:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,10   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,104, 8,8, FontBlack,TEXTWORD10,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK10 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS10 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND10
nop ; Delay Slot
SRAVPASS10:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND10:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,11   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,112, 8,8, FontBlack,TEXTWORD11,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK11 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS11 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND11
nop ; Delay Slot
SRAVPASS11:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND11:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,12   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,120, 8,8, FontBlack,TEXTWORD12,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK12 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS12 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND12
nop ; Delay Slot
SRAVPASS12:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND12:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,13   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,128, 8,8, FontBlack,TEXTWORD13,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK13 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS13 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND13
nop ; Delay Slot
SRAVPASS13:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND13:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,14   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,136, 8,8, FontBlack,TEXTWORD14,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK14 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS14 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND14
nop ; Delay Slot
SRAVPASS14:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND14:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,15   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,144, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,144, 8,8, FontBlack,TEXTWORD15,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,144, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK15 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS15 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,144, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND15
nop ; Delay Slot
SRAVPASS15:
PrintString 280,144, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND15:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,16   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,152, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,152, 8,8, FontBlack,TEXTWORD16,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,152, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK16 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS16 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,152, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND16
nop ; Delay Slot
SRAVPASS16:
PrintString 280,152, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND16:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,17   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,160, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,160, 8,8, FontBlack,TEXTWORD17,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,160, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK17 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS17 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,160, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND17
nop ; Delay Slot
SRAVPASS17:
PrintString 280,160, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND17:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,18   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,168, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,168, 8,8, FontBlack,TEXTWORD18,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,168, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK18 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS18 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,168, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND18
nop ; Delay Slot
SRAVPASS18:
PrintString 280,168, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND18:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,19   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,176, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,176, 8,8, FontBlack,TEXTWORD19,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,176, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK19 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS19 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,176, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND19
nop ; Delay Slot
SRAVPASS19:
PrintString 280,176, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND19:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,20   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,184, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,184, 8,8, FontBlack,TEXTWORD20,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,184, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK20 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS20 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,184, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND20
nop ; Delay Slot
SRAVPASS20:
PrintString 280,184, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND20:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,21   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,192, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,192, 8,8, FontBlack,TEXTWORD21,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,192, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK21 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS21 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,192, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND21
nop ; Delay Slot
SRAVPASS21:
PrintString 280,192, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND21:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,22   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,200, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,200, 8,8, FontBlack,TEXTWORD22,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,200, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK22 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS22 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,200, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND22
nop ; Delay Slot
SRAVPASS22:
PrintString 280,200, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND22:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,23   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,208, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,208, 8,8, FontBlack,TEXTWORD23,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,208, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK23 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS23 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,208, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND23
nop ; Delay Slot
SRAVPASS23:
PrintString 280,208, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND23:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,24   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,216, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,216, 8,8, FontBlack,TEXTWORD24,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,216, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK24 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS24 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,216, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND24
nop ; Delay Slot
SRAVPASS24:
PrintString 280,216, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND24:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,25   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,224, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,224, 8,8, FontBlack,TEXTWORD25,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,224, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK25 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS25 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,224, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND25
nop ; Delay Slot
SRAVPASS25:
PrintString 280,224, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND25:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,26   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,232, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,232, 8,8, FontBlack,TEXTWORD26,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,232, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK26 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS26 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,232, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND26
nop ; Delay Slot
SRAVPASS26:
PrintString 280,232, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND26:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,27   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,240, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,240, 8,8, FontBlack,TEXTWORD27,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,240, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK27 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS27 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,240, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND27
nop ; Delay Slot
SRAVPASS27:
PrintString 280,240, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND27:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,28   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,248, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,248, 8,8, FontBlack,TEXTWORD28,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,248, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK28 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS28 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,248, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND28
nop ; Delay Slot
SRAVPASS28:
PrintString 280,248, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND28:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,29   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,256, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,256, 8,8, FontBlack,TEXTWORD29,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,256, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK29 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS29 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,256, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND29
nop ; Delay Slot
SRAVPASS29:
PrintString 280,256, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND29:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,30   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,264, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,264, 8,8, FontBlack,TEXTWORD30,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,264, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK30 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS30 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,264, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND30
nop ; Delay Slot
SRAVPASS30:
PrintString 280,264, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND30:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,31   ; T1 = Shift Amount
srav t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,272, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,272, 8,8, FontBlack,TEXTWORD31,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,272, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SRAVCHECK31 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SRAVPASS31 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,272, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SRAVEND31
nop ; Delay Slot
SRAVPASS31:
PrintString 280,272, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SRAVEND31:


PrintString 0,280, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

SRAV:
  .db "SRAV"

RDHEX:
  .db "RD Hex"
RTHEX:
  .db "RT Hex"
RSDEC:
  .db "RS Dec"
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

SRAVCHECK0:
  .dw 0xF8A432EB
SRAVCHECK1:
  .dw 0xFC521975
SRAVCHECK2:
  .dw 0xFE290CBA
SRAVCHECK3:
  .dw 0xFF14865D
SRAVCHECK4:
  .dw 0xFF8A432E
SRAVCHECK5:
  .dw 0xFFC52197
SRAVCHECK6:
  .dw 0xFFE290CB
SRAVCHECK7:
  .dw 0xFFF14865
SRAVCHECK8:
  .dw 0xFFF8A432
SRAVCHECK9:
  .dw 0xFFFC5219
SRAVCHECK10:
  .dw 0xFFFE290C
SRAVCHECK11:
  .dw 0xFFFF1486
SRAVCHECK12:
  .dw 0xFFFF8A43
SRAVCHECK13:
  .dw 0xFFFFC521
SRAVCHECK14:
  .dw 0xFFFFE290
SRAVCHECK15:
  .dw 0xFFFFF148
SRAVCHECK16:
  .dw 0xFFFFF8A4
SRAVCHECK17:
  .dw 0xFFFFFC52
SRAVCHECK18:
  .dw 0xFFFFFE29
SRAVCHECK19:
  .dw 0xFFFFFF14
SRAVCHECK20:
  .dw 0xFFFFFF8A
SRAVCHECK21:
  .dw 0xFFFFFFC5
SRAVCHECK22:
  .dw 0xFFFFFFE2
SRAVCHECK23:
  .dw 0xFFFFFFF1
SRAVCHECK24:
  .dw 0xFFFFFFF8
SRAVCHECK25:
  .dw 0xFFFFFFFC
SRAVCHECK26:
  .dw 0xFFFFFFFE
SRAVCHECK27:
  .dw 0xFFFFFFFF
SRAVCHECK28:
  .dw 0xFFFFFFFF
SRAVCHECK29:
  .dw 0xFFFFFFFF
SRAVCHECK30:
  .dw 0xFFFFFFFF
SRAVCHECK31:
  .dw 0xFFFFFFFF

RDWORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close