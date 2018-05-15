; PSX 'Bare Metal' CPU Store Byte Test Demo by krom (Peter Lemon):
.psx
.create "CPUSB.bin", 0x80010000

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


PrintString 8,24, 8,8, FontRed,SB,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
la a1,VALUEBYTEA ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,24, 8,8, FontBlack,VALUEBYTEA,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,24, 8,8, FontBlack,TEXTBYTEA,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,24, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,24, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKA ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSA ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,24, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDA
nop ; Delay Slot
SBPASSA:
PrintString 280,24, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDA:

la a1,VALUEBYTEB ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,32, 8,8, FontBlack,VALUEBYTEB,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 184,32, 8,8, FontBlack,TEXTBYTEB,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,32, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,32, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKB ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSB ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,32, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDB
nop ; Delay Slot
SBPASSB:
PrintString 280,32, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDB:

la a1,VALUEBYTEC ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,40, 8,8, FontBlack,VALUEBYTEC,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,40, 8,8, FontBlack,TEXTBYTEC,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,40, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,40, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKC ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSC ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,40, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDC
nop ; Delay Slot
SBPASSC:
PrintString 280,40, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDC:

la a1,VALUEBYTED ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,48, 8,8, FontBlack,VALUEBYTED,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 168,48, 8,8, FontBlack,TEXTBYTED,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,48, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,48, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKD ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSD ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,48, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDD
nop ; Delay Slot
SBPASSD:
PrintString 280,48, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDD:

la a1,VALUEBYTEE ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,56, 8,8, FontBlack,VALUEBYTEE,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 176,56, 8,8, FontBlack,TEXTBYTEE,1 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,56, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,56, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKE ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSE ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,56, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDE
nop ; Delay Slot
SBPASSE:
PrintString 280,56, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDE:

la a1,VALUEBYTEF ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,64, 8,8, FontBlack,VALUEBYTEF,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 168,64, 8,8, FontBlack,TEXTBYTEF,2 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,64, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,64, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKF ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSF ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,64, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDF
nop ; Delay Slot
SBPASSF:
PrintString 280,64, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDF:

la a1,VALUEBYTEG ; A1 = Byte Data Offset
lb t0,0(a1)      ; T0 = Test Byte Data
la a1,WORD  ; A1 = WORD Offset
sb t0,0(a1) ; WORD = Word Data
PrintString 40,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  48,72, 8,8, FontBlack,VALUEBYTEG,0 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
PrintString 160,72, 8,8, FontBlack,TEXTBYTEG,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintString 200,72, 8,8, FontBlack,DOLLAR,0 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
PrintValue  208,72, 8,8, FontBlack,WORD,3 ; Print HEX Chars To VRAM Using Width,Height Font At X,Y Position
la a1,WORD     ; A1 = Word Data Offset
lw t0,0(a1)    ; T0 = Word Data
la a1,SBCHECKG ; A1 = Word Check Data Offset
lw t1,0(a1)    ; T1 = Word Check Data
nop ; Delay Slot
beq t0,t1,SBPASSG ; Compare Result Equality With Check Data
nop ; Delay Slot
PrintString 280,72, 8,8, FontRed,FAIL,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
j SBENDG
nop ; Delay Slot
SBPASSG:
PrintString 280,72, 8,8, FontGreen,PASS,3 ; Print Text String To VRAM Using Width,Height Font At X,Y Position
SBENDG:


PrintString 0,80, 8,8, FontBlack,PAGEBREAK,39 ; Print Text String To VRAM Using Width,Height Font At X,Y Position


Loop:
  b Loop
  nop ; Delay Slot

SB:
  .db "SB"

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

TEXTBYTEA:
  .db "0"
TEXTBYTEB:
  .db "1"
TEXTBYTEC:
  .db "12"
TEXTBYTED:
  .db "123"
TEXTBYTEE:
  .db "-1"
TEXTBYTEF:
  .db "-12"
TEXTBYTEG:
  .db "-123"

PAGEBREAK:
  .db "----------------------------------------"

VALUEBYTEA:
  .db 0
VALUEBYTEB:
  .db 1
VALUEBYTEC:
  .db 12
VALUEBYTED:
  .db 123
VALUEBYTEE:
  .db -1
VALUEBYTEF:
  .db -12
VALUEBYTEG:
  .db -123

.align 4 ; Align 32-Bit
SBCHECKA:
  .dw 0x00000000
SBCHECKB:
  .dw 0x00000001
SBCHECKC:
  .dw 0x0000000C
SBCHECKD:
  .dw 0x0000007B
SBCHECKE:
  .dw 0x000000FF
SBCHECKF:
  .dw 0x000000F4
SBCHECKG:
  .dw 0x00000085

WORD:
  .dw 0

FontBlack:
  .incbin "FontBlack8x8.bin"
FontGreen:
  .incbin "FontGreen8x8.bin"
FontRed:
  .incbin "FontRed8x8.bin"

.close