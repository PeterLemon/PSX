; PSX 'Bare Metal' CPU Store Word Test Demo by krom (Peter Lemon):
.psx
.create "CPUSW.bin", 0x80010000

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
PrintString 144,8, 8,8, FontRed,RTDEC,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,8, 8,8, FontRed,WORDHEX,7 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 280,8, 8,8, FontRed,TEST,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 0,16, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


PrintString 8,24, 8,8, FontRed,SW,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDA ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEWORDA,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTWORDA,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDA
nop ; Delay Slot
SWPASSA:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,32, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDB
nop ; Delay Slot
SWPASSB:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDB:

la a1,VALUEWORDC ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEWORDC,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 144,40, 8,8, FontBlack,TEXTWORDC,5 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDC
nop ; Delay Slot
SWPASSC:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDC:

la a1,VALUEWORDD ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEWORDD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,48, 8,8, FontBlack,TEXTWORDD,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDD
nop ; Delay Slot
SWPASSD:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDD:

la a1,VALUEWORDE ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEWORDE,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,56, 8,8, FontBlack,TEXTWORDE,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDE
nop ; Delay Slot
SWPASSE:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDE:

la a1,VALUEWORDF ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEWORDF,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 136,64, 8,8, FontBlack,TEXTWORDF,6 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDF
nop ; Delay Slot
SWPASSF:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD  ; A1 = WORD Offset
sw t0,0(a1) ; WORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,72, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SWCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWENDG
nop ; Delay Slot
SWPASSG:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWENDG:


PrintString 8,88, 8,8, FontRed,SWL,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,0(a1) ; WORD = Word Data
PrintString 40,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,88, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,88, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,88, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,88, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,88, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDA
nop ; Delay Slot
SWLPASSA:
PrintString 280,88, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,1(a1) ; WORD = Word Data
PrintString 40,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,96, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,96, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,96, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,96, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,96, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDB
nop ; Delay Slot
SWLPASSB:
PrintString 280,96, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDB:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,2(a1) ; WORD = Word Data
PrintString 40,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,104, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,104, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,104, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,104, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,104, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDC
nop ; Delay Slot
SWLPASSC:
PrintString 280,104, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDC:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,3(a1) ; WORD = Word Data
PrintString 40,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,112, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,112, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,112, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,112, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,112, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDD
nop ; Delay Slot
SWLPASSD:
PrintString 280,112, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDD:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,0(a1) ; WORD = Word Data
PrintString 40,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,120, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,120, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,120, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,120, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD    ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,120, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDE
nop ; Delay Slot
SWLPASSE:
PrintString 280,120, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDE:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,1(a1) ; WORD = Word Data
PrintString 40,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,128, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,128, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,128, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,128, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,128, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDF
nop ; Delay Slot
SWLPASSF:
PrintString 280,128, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,2(a1) ; WORD = Word Data
PrintString 40,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,136, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,136, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,136, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,136, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,136, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDG
nop ; Delay Slot
SWLPASSG:
PrintString 280,136, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDG:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swl t0,3(a1) ; WORD = Word Data
PrintString 40,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,144, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,144, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,144, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,144, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWLCHECKH ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWLPASSH ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,144, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWLENDH
nop ; Delay Slot
SWLPASSH:
PrintString 280,144, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWLENDH:


PrintString 8,160, 8,8, FontRed,SWR,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,0(a1) ; WORD = Word Data
PrintString 40,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,160, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,160, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,160, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,160, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,160, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDA
nop ; Delay Slot
SWRPASSA:
PrintString 280,160, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDA:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,1(a1) ; WORD = Word Data
PrintString 40,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,168, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,168, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,168, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,168, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,168, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDB
nop ; Delay Slot
SWRPASSB:
PrintString 280,168, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDB:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,2(a1) ; WORD = Word Data
PrintString 40,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,176, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,176, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,176, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,176, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,176, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDC
nop ; Delay Slot
SWRPASSC:
PrintString 280,176, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDC:

la a1,VALUEWORDB ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,3(a1) ; WORD = Word Data
PrintString 40,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,184, 8,8, FontBlack,VALUEWORDB,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 120,184, 8,8, FontBlack,TEXTWORDB,8 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,184, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,184, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,184, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDD
nop ; Delay Slot
SWRPASSD:
PrintString 280,184, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDD:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,0(a1) ; WORD = Word Data
PrintString 40,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,192, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,192, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,192, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,192, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,192, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDE
nop ; Delay Slot
SWRPASSE:
PrintString 280,192, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDE:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,1(a1) ; WORD = Word Data
PrintString 40,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,200, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,200, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,200, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,200, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,200, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDF
nop ; Delay Slot
SWRPASSF:
PrintString 280,200, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDF:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,2(a1) ; WORD = Word Data
PrintString 40,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,208, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,208, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,208, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,208, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,208, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDG
nop ; Delay Slot
SWRPASSG:
PrintString 280,208, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDG:

la a1,VALUEWORDG ; A1 = Word Data Offset
lw t0,0(a1)      ; T0 = Test Word Data
la a1,WORD   ; A1 = WORD Offset
swr t0,3(a1) ; WORD = Word Data
PrintString 40,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,216, 8,8, FontBlack,VALUEWORDG,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 112,216, 8,8, FontBlack,TEXTWORDG,9 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,216, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,216, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD      ; A1 = Word Data Offset
lw t0,0(a1)     ; T0 = Word Data
la a1,SWRCHECKH ; A1 = Word Check Data Offset
lw t1,0(a1)     ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SWRPASSH ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,216, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SWRENDH
nop ; Delay Slot
SWRPASSH:
PrintString 280,216, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SWRENDH:


PrintString 0,224, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

SW:
  .db "SW"
SWL:
  .db "SWL"
SWR:
  .db "SWR"

WORDHEX:
  .db "WORD Hex"
RTHEX:
  .db "RT Hex"
RTDEC:
  .db "RT Dec"
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

SWCHECKA:
  .dw 0x00000000
SWCHECKB:
  .dw 0x075BCD15
SWCHECKC:
  .dw 0x0001E240
SWCHECKD:
  .dw 0x075BB762
SWCHECKE:
  .dw 0xF8A4489E
SWCHECKF:
  .dw 0xFFFE1DC0
SWCHECKG:
  .dw 0xF8A432EB

SWLCHECKA:
  .dw 0xF8A43207
SWLCHECKB:
  .dw 0xF8A4075B
SWLCHECKC:
  .dw 0xF8075BCD
SWLCHECKD:
  .dw 0x075BCD15
SWLCHECKE:
  .dw 0x075BCDF8
SWLCHECKF:
  .dw 0x075BF8A4
SWLCHECKG:
  .dw 0x07F8A432
SWLCHECKH:
  .dw 0xF8A432EB

SWRCHECKA:
  .dw 0x075BCD15
SWRCHECKB:
  .dw 0x5BCD1515
SWRCHECKC:
  .dw 0xCD151515
SWRCHECKD:
  .dw 0x15151515
SWRCHECKE:
  .dw 0xF8A432EB
SWRCHECKF:
  .dw 0xA432EBEB
SWRCHECKG:
  .dw 0x32EBEBEB
SWRCHECKH:
  .dw 0xEBEBEBEB

WORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close