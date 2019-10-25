; PSX 'Bare Metal' CPU Load Word Test Demo by krom (Peter Lemon):
.psx
.create "CPULW.bin", 0x80010000

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


PrintString 8,24, 8,8, FontRed,LW,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDA ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEWORDA,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTWORDA,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDA
nop ; Delay Slot
LWPASSA:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,32, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDB
nop ; Delay Slot
LWPASSB:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDB:

la a1,VALUEWORDC ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEWORDC,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,40, 8,8, FontBlack,TEXTWORDC,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDC
nop ; Delay Slot
LWPASSC:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDC:

la a1,VALUEWORDD ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEWORDD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,48, 8,8, FontBlack,TEXTWORDD,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDD
nop ; Delay Slot
LWPASSD:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDD:

la a1,VALUEWORDE ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEWORDE,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,56, 8,8, FontBlack,TEXTWORDE,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDE
nop ; Delay Slot
LWPASSE:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDE:

la a1,VALUEWORDF ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEWORDF,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 136,64, 8,8, FontBlack,TEXTWORDF,6 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDF
nop ; Delay Slot
LWPASSF:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,72, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD   ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,LWCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWENDG
nop ; Delay Slot
LWPASSG:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWENDG:


PrintString 8,88, 8,8, FontRed,LWL,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDB ; A1 = Word Data Offset
lwl t0,0(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,88, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDA
nop ; Delay Slot
LWLPASSA:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwl t0,1(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,96, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDB
nop ; Delay Slot
LWLPASSB:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDB:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwl t0,2(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,104, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDC
nop ; Delay Slot
LWLPASSC:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDC:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwl t0,3(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,112, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDD
nop ; Delay Slot
LWLPASSD:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDD:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwl t0,0(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,120, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDE
nop ; Delay Slot
LWLPASSE:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDE:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwl t0,1(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,128, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDF
nop ; Delay Slot
LWLPASSF:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwl t0,2(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,136, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDG
nop ; Delay Slot
LWLPASSG:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDG:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwl t0,3(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,144, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,144, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,144, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWLCHECKH ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWLPASSH ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,144, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWLENDH
nop ; Delay Slot
LWLPASSH:
PrintString 280,144, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWLENDH:


PrintString 8,160, 8,8, FontRed,LWR,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDB ; A1 = Word Data Offset
lwr t0,0(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,160, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,160, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,160, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,160, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDA
nop ; Delay Slot
LWRPASSA:
PrintString 280,160, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwr t0,1(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,168, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,168, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,168, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,168, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDB
nop ; Delay Slot
LWRPASSB:
PrintString 280,168, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDB:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwr t0,2(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,176, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,176, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,176, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,176, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDC
nop ; Delay Slot
LWRPASSC:
PrintString 280,176, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDC:

la a1,VALUEWORDB ; A1 = Word Data Offset
lwr t0,3(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,184, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,184, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,184, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,184, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDD
nop ; Delay Slot
LWRPASSD:
PrintString 280,184, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDD:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwr t0,0(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,192, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,192, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,192, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,192, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDE
nop ; Delay Slot
LWRPASSE:
PrintString 280,192, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDE:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwr t0,1(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,200, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,200, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,200, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,200, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDF
nop ; Delay Slot
LWRPASSF:
PrintString 280,200, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwr t0,2(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,208, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,208, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,208, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,208, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDG
nop ; Delay Slot
LWRPASSG:
PrintString 280,208, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDG:

la a1,VALUEWORDG ; A1 = Word Data Offset
lwr t0,3(a1)     ; T0 = Test Word Data
la a1,RTWORD ; A1 = RTWORD Offset
sw t0,0(a1)  ; RTWORD = Word Data
PrintString 40,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,216, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,216, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,216, 8,8, FontBlack,RTWORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,RTWORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,LWRCHECKH ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,LWRPASSH ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,216, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j LWRENDH
nop ; Delay Slot
LWRPASSH:
PrintString 280,216, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
LWRENDH:


PrintString 0,224, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

LW:
  .db "LW"
LWL:
  .db "LWL"
LWR:
  .db "LWR"

RTHEX:
  .db "RT Hex"
WORDHEX:
  .db "WORD Hex"
WORDDEC:
  .db "WORD Dec"
TEST:
  .db "Test"
FAIL:
  .db "FAIL"
PASS:
  .db "PASS"

DOLLAR:
  .db "$"

TEXTWORDA:
  .db "0"
TEXTWORDB:
  .db "123456789"
TEXTWORDC:
  .db "123456"
TEXTWORDD:
  .db "123451234"
TEXTWORDE:
  .db "-123451234"
TEXTWORDF:
  .db "-123456"
TEXTWORDG:
  .db "-123456789"

PAGEBREAK:
  .db "----------------------------------------"

.align 4 ; Align 32-Bit
VALUEWORDA:
  .dw 0
VALUEWORDB:
  .dw 123456789
VALUEWORDC:
  .dw 123456
VALUEWORDD:
  .dw 123451234
VALUEWORDE:
  .dw -123451234
VALUEWORDF:
  .dw -123456
VALUEWORDG:
  .dw -123456789

LWCHECKA:
  .dw 0x00000000
LWCHECKB:
  .dw 0x075BCD15
LWCHECKC:
  .dw 0x0001E240
LWCHECKD:
  .dw 0x075BB762
LWCHECKE:
  .dw 0xF8A4489E
LWCHECKF:
  .dw 0xFFFE1DC0
LWCHECKG:
  .dw 0xF8A432EB

LWLCHECKA:
  .dw 0x15FFFFFF
LWLCHECKB:
  .dw 0xCD15FFFF
LWLCHECKC:
  .dw 0x5BCD15FF
LWLCHECKD:
  .dw 0x075BCD15
LWLCHECKE:
  .dw 0xEBFFFFFF
LWLCHECKF:
  .dw 0x32EBFFFF
LWLCHECKG:
  .dw 0xA432EBFF
LWLCHECKH:
  .dw 0xF8A432EB

LWRCHECKA:
  .dw 0x075BCD15
LWRCHECKB:
  .dw 0xFF075BCD
LWRCHECKC:
  .dw 0xFFFF075B
LWRCHECKD:
  .dw 0xFFFFFF07
LWRCHECKE:
  .dw 0xF8A432EB
LWRCHECKF:
  .dw 0xFFF8A432
LWRCHECKG:
  .dw 0xFFFFF8A4
LWRCHECKH:
  .dw 0xFFFFFFF8

RTWORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close