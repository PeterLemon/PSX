; PSX 'Bare Metal' CPU Load Halfword Test Demo by krom (Peter Lemon):
.psx
.create "CPULH.bin", 0x80010000

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
PrintString  40,8, 8,8, FontRed,WORDHEX,7 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 128,8, 8,8, FontRed,WORDDEC,7 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,8, 8,8, FontRed,RTHEX,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 280,8, 8,8, FontRed,TEST,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 0,16, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 8,24, 8,8, FontRed,LH,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEHALFA ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEHALFA,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTHALFA,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDA
nop ; Delay Slot
LHPASSA:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDA:

la a1,VALUEHALFB ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEHALFB,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,32, 8,8, FontBlack,TEXTHALFB,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDB
nop ; Delay Slot
LHPASSB:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDB:

la a1,VALUEHALFC ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEHALFC,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 160,40, 8,8, FontBlack,TEXTHALFC,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDC
nop ; Delay Slot
LHPASSC:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDC:

la a1,VALUEHALFD ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEHALFD,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,48, 8,8, FontBlack,TEXTHALFD,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDD
nop ; Delay Slot
LHPASSD:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDD:

la a1,VALUEHALFE ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEHALFE,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,56, 8,8, FontBlack,TEXTHALFE,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDE
nop ; Delay Slot
LHPASSE:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDE:

la a1,VALUEHALFF ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEHALFF,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,64, 8,8, FontBlack,TEXTHALFF,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDF
nop ; Delay Slot
LHPASSF:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDF:

la a1,VALUEHALFG ; A1 = Halfword Data Offset
lh t0,0(a1)      ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEHALFG,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,72, 8,8, FontBlack,TEXTHALFG,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LHCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHENDG
nop ; Delay Slot
LHPASSG:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHENDG:


PrintString 8,88, 8,8, FontRed,LHU,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEHALFA ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEHALFA,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,88, 8,8, FontBlack,TEXTHALFA,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDA
nop ; Delay Slot
LHUPASSA:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDA:

la a1,VALUEHALFB ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEHALFB,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,96, 8,8, FontBlack,TEXTHALFB,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDB
nop ; Delay Slot
LHUPASSB:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDB:

la a1,VALUEHALFC ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEHALFC,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 160,104, 8,8, FontBlack,TEXTHALFC,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDC
nop ; Delay Slot
LHUPASSC:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDC:

la a1,VALUEHALFD ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEHALFD,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,112, 8,8, FontBlack,TEXTHALFD,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDD
nop ; Delay Slot
LHUPASSD:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDD:

la a1,VALUEHALFE ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEHALFE,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,120, 8,8, FontBlack,TEXTHALFE,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDE
nop ; Delay Slot
LHUPASSE:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDE:

la a1,VALUEHALFF ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEHALFF,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 152,128, 8,8, FontBlack,TEXTHALFF,4 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDF
nop ; Delay Slot
LHUPASSF:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDF:

la a1,VALUEHALFG ; A1 = Halfword Data Offset
lhu t0,0(a1)     ; T0 = Test Halfword Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEHALFG,1 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,136, 8,8, FontBlack,TEXTHALFG,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LHUCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LHUPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LHUENDG
nop ; Delay Slot
LHUPASSG:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LHUENDG:


PrintString 0,144, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

LH:
  .db "LH"
LHU:
  .db "LHU"

RTHEX:
  .db "RT Hex"
WORDHEX:
  .db "HALF Hex"
WORDDEC:
  .db "HALF Dec"
TEST:
  .db "Test"
FAIL:
  .db "FAIL"
PASS:
  .db "PASS"

DOLLAR:
  .db "$"

TEXTHALFA:
  .db "0"
TEXTHALFB:
  .db "12345"
TEXTHALFC:
  .db "1234"
TEXTHALFD:
  .db "12341"
TEXTHALFE:
  .db "-12341"
TEXTHALFF:
  .db "-1234"
TEXTHALFG:
  .db "-12345"

PAGEBREAK:
  .db "----------------------------------------"

.align 2 ; Align 16-Bit
VALUEHALFA:
  .dh 0
VALUEHALFB:
  .dh 12345
VALUEHALFC:
  .dh 1234
VALUEHALFD:
  .dh 12341
VALUEHALFE:
  .dh -12341
VALUEHALFF:
  .dh -1234
VALUEHALFG:
  .dh -12345

.align 4 ; Align 32-Bit
LHCHECKA:
  .dw 0x00000000
LHCHECKB:
  .dw 0x00003039
LHCHECKC:
  .dw 0x000004D2
LHCHECKD:
  .dw 0x00003035
LHCHECKE:
  .dw 0xFFFFCFCB
LHCHECKF:
  .dw 0xFFFFFB2E
LHCHECKG:
  .dw 0xFFFFCFC7

LHUCHECKA:
  .dw 0x00000000
LHUCHECKB:
  .dw 0x00003039
LHUCHECKC:
  .dw 0x000004D2
LHUCHECKD:
  .dw 0x00003035
LHUCHECKE:
  .dw 0x0000CFCB
LHUCHECKF:
  .dw 0x0000FB2E
LHUCHECKG:
  .dw 0x0000CFC7

RTWORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close