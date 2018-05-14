; PSX 'Bare Metal' CPU Word Shift Left Logical Variable (0..31) Test Demo by krom (Peter Lemon):
.psx
.create "CPUSLLV.bin", 0x80010000

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


PrintString 8,24, 8,8, FontRed,SLLV,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,0    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTWORD0,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK0 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS0 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND0
nop ; Delay Slot
SLLVPASS0:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND0:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,1    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,32, 8,8, FontBlack,TEXTWORD1,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK1 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS1 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND1
nop ; Delay Slot
SLLVPASS1:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND1:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,2    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,40, 8,8, FontBlack,TEXTWORD2,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK2 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS2 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND2
nop ; Delay Slot
SLLVPASS2:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND2:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,3    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,48, 8,8, FontBlack,TEXTWORD3,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK3 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS3 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND3
nop ; Delay Slot
SLLVPASS3:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND3:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,4    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,56, 8,8, FontBlack,TEXTWORD4,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK4 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS4 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND4
nop ; Delay Slot
SLLVPASS4:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND4:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,5    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,64, 8,8, FontBlack,TEXTWORD5,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK5 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS5 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND5
nop ; Delay Slot
SLLVPASS5:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND5:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,6    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,72, 8,8, FontBlack,TEXTWORD6,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK6 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS6 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND6
nop ; Delay Slot
SLLVPASS6:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND6:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,7    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,80, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,80, 8,8, FontBlack,TEXTWORD7,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,80, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,80, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK7 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS7 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,80, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND7
nop ; Delay Slot
SLLVPASS7:
PrintString 280,80, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND7:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,8    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,88, 8,8, FontBlack,TEXTWORD8,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK8 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS8 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND8
nop ; Delay Slot
SLLVPASS8:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND8:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,9    ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,96, 8,8, FontBlack,TEXTWORD9,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD     ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Word Data
la a1,SLLVCHECK9 ; A1 = Word Check Data Offset
lw t1,0(a1)      ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS9 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND9
nop ; Delay Slot
SLLVPASS9:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND9:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,10   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,104, 8,8, FontBlack,TEXTWORD10,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK10 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS10 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND10
nop ; Delay Slot
SLLVPASS10:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND10:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,11   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,112, 8,8, FontBlack,TEXTWORD11,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK11 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS11 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND11
nop ; Delay Slot
SLLVPASS11:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND11:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,12   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,120, 8,8, FontBlack,TEXTWORD12,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK12 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS12 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND12
nop ; Delay Slot
SLLVPASS12:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND12:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,13   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,128, 8,8, FontBlack,TEXTWORD13,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK13 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS13 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND13
nop ; Delay Slot
SLLVPASS13:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND13:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,14   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,136, 8,8, FontBlack,TEXTWORD14,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK14 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS14 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND14
nop ; Delay Slot
SLLVPASS14:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND14:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,15   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,144, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,144, 8,8, FontBlack,TEXTWORD15,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,144, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK15 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS15 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,144, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND15
nop ; Delay Slot
SLLVPASS15:
PrintString 280,144, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND15:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,16   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,152, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,152, 8,8, FontBlack,TEXTWORD16,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,152, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,152, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK16 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS16 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,152, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND16
nop ; Delay Slot
SLLVPASS16:
PrintString 280,152, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND16:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,17   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,160, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,160, 8,8, FontBlack,TEXTWORD17,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,160, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK17 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS17 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,160, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND17
nop ; Delay Slot
SLLVPASS17:
PrintString 280,160, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND17:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,18   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,168, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,168, 8,8, FontBlack,TEXTWORD18,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,168, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK18 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS18 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,168, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND18
nop ; Delay Slot
SLLVPASS18:
PrintString 280,168, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND18:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,19   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,176, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,176, 8,8, FontBlack,TEXTWORD19,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,176, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK19 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS19 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,176, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND19
nop ; Delay Slot
SLLVPASS19:
PrintString 280,176, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND19:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,20   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,184, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,184, 8,8, FontBlack,TEXTWORD20,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,184, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK20 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS20 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,184, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND20
nop ; Delay Slot
SLLVPASS20:
PrintString 280,184, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND20:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,21   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,192, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,192, 8,8, FontBlack,TEXTWORD21,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,192, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK21 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS21 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,192, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND21
nop ; Delay Slot
SLLVPASS21:
PrintString 280,192, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND21:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,22   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,200, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,200, 8,8, FontBlack,TEXTWORD22,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,200, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK22 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS22 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,200, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND22
nop ; Delay Slot
SLLVPASS22:
PrintString 280,200, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND22:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,23   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,208, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,208, 8,8, FontBlack,TEXTWORD23,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,208, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK23 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS23 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,208, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND23
nop ; Delay Slot
SLLVPASS23:
PrintString 280,208, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND23:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,24   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,216, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,216, 8,8, FontBlack,TEXTWORD24,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,216, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK24 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS24 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,216, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND24
nop ; Delay Slot
SLLVPASS24:
PrintString 280,216, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND24:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,25   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,224, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,224, 8,8, FontBlack,TEXTWORD25,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,224, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,224, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK25 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS25 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,224, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND25
nop ; Delay Slot
SLLVPASS25:
PrintString 280,224, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND25:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,26   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,232, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,232, 8,8, FontBlack,TEXTWORD26,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,232, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,232, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK26 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS26 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,232, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND26
nop ; Delay Slot
SLLVPASS26:
PrintString 280,232, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND26:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,27   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,240, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,240, 8,8, FontBlack,TEXTWORD27,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,240, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,240, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK27 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS27 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,240, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND27
nop ; Delay Slot
SLLVPASS27:
PrintString 280,240, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND27:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,28   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,248, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,248, 8,8, FontBlack,TEXTWORD28,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,248, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,248, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK28 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS28 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,248, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND28
nop ; Delay Slot
SLLVPASS28:
PrintString 280,248, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND28:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,29   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,256, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,256, 8,8, FontBlack,TEXTWORD29,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,256, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,256, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK29 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS29 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,256, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND29
nop ; Delay Slot
SLLVPASS29:
PrintString 280,256, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND29:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,30   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,264, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,264, 8,8, FontBlack,TEXTWORD30,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,264, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,264, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK30 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS30 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,264, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND30
nop ; Delay Slot
SLLVPASS30:
PrintString 280,264, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND30:

la a1,VALUEWORD ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,RDWORD ; A1 = RDWORD Offset
li t1,31   ; T1 = Shift Amount
sllv t0,t1 ; T0 = Test Word Data
sw t0,0(a1) ; RDWORD = Word Data
PrintString 40,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,272, 8,8, FontBlack,VALUEWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,272, 8,8, FontBlack,TEXTWORD31,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,272, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,272, 8,8, FontBlack,RDWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RDWORD      ; A1 = Word Data Offset
lw t0,0(a1)       ; T0 = Word Data
la a1,SLLVCHECK31 ; A1 = Word Check Data Offset
lw t1,0(a1)       ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SLLVPASS31 ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,272, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SLLVEND31
nop ; Delay Slot
SLLVPASS31:
PrintString 280,272, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SLLVEND31:


PrintString 0,280, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

SLLV:
  .db "SLLV"

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

SLLVCHECK0:
  .dw 0xF8A432EB
SLLVCHECK1:
  .dw 0xF14865D6
SLLVCHECK2:
  .dw 0xE290CBAC
SLLVCHECK3:
  .dw 0xC5219758
SLLVCHECK4:
  .dw 0x8A432EB0
SLLVCHECK5:
  .dw 0x14865D60
SLLVCHECK6:
  .dw 0x290CBAC0
SLLVCHECK7:
  .dw 0x52197580
SLLVCHECK8:
  .dw 0xA432EB00
SLLVCHECK9:
  .dw 0x4865D600
SLLVCHECK10:
  .dw 0x90CBAC00
SLLVCHECK11:
  .dw 0x21975800
SLLVCHECK12:
  .dw 0x432EB000
SLLVCHECK13:
  .dw 0x865D6000
SLLVCHECK14:
  .dw 0x0CBAC000
SLLVCHECK15:
  .dw 0x19758000
SLLVCHECK16:
  .dw 0x32EB0000
SLLVCHECK17:
  .dw 0x65D60000
SLLVCHECK18:
  .dw 0xCBAC0000
SLLVCHECK19:
  .dw 0x97580000
SLLVCHECK20:
  .dw 0x2EB00000
SLLVCHECK21:
  .dw 0x5D600000
SLLVCHECK22:
  .dw 0xBAC00000
SLLVCHECK23:
  .dw 0x75800000
SLLVCHECK24:
  .dw 0xEB000000
SLLVCHECK25:
  .dw 0xD6000000
SLLVCHECK26:
  .dw 0xAC000000
SLLVCHECK27:
  .dw 0x58000000
SLLVCHECK28:
  .dw 0xB0000000
SLLVCHECK29:
  .dw 0x60000000
SLLVCHECK30:
  .dw 0xC0000000
SLLVCHECK31:
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