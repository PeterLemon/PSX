; PSX 'Bare Metal' 16BPP 320x240 Atari-ST-NICCC Demo by krom (Peter Lemon):
.psx
.create "PSXNICCC.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros
.include "LIB/PSX_INPUT.INC" ; Include PSX Input Definitions & Macros

.org 0x80010000 ; Entry Point Of Code

InitJoy PadBuffer ; Initialise Joypads & Setup VSync Wait Routine Using BIOS: Buffer Address

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup Screen Mode
WRGP1 GPURESET,0  ; Write GP1 Command Word (Reset GPU)
WRGP1 GPUDISPEN,0 ; Write GP1 Command Word (Enable Display)
WRGP1 GPUDISPM,HRES256+VRES240+BPP15+VNTSC ; Write GP1 Command Word (Set Display Mode: 256x240, 15BPP, NTSC)
WRGP1 GPUDISPH,0xC60260 ; Write GP1 Command Word (Horizontal Display Range 608..3168)
WRGP1 GPUDISPV,0x042018 ; Write GP1 Command Word (Vertical Display Range 24..264)

; Setup Drawing Area
WRGP0 GPUDRAWM,0x000400   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BCFF ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=255, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

la a1,SceneData ; A1 = Scene Data Start Address
la a2,Palette   ; A2 = Palette Color Data Address

LoopFrames:
  la a3,PadBuffer ; Load Pad Buffer Address
  Wait:           ; Wait For Vertical Retrace Period & Store XOR Pad Data
    lw t0,0(a3)   ; Load Pad Buffer
    nop           ; Delay Slot
    beqz t0,Wait  ; IF (Pad Buffer == 0) Wait
    nor t0,r0     ; NOR Compliment Pad Data Bits (Delay Slot)
    sw r0,0(a3)   ; Store Zero To Pad Buffer
    la a3,PadData ; Load Pad Data Address
    sw t0,0(a3)   ; Store Pad Data

  ; Swap Display/Render Buffers
  la a3,DoubleBuffer ; A2 = Double Buffer Address
  lw t0,0(a3) ; T0 = Double Buffer Word
  nop ; Delay Slot
  beqz t0,FrameB ; IF (Double Buffer == 0) Frame B
  nop ; Delay Slot
  WRGP1 GPUVRAM,0x000100    ; Write GP1 Command Word (Start Of Display Area: X = 256, Y = 0)
  WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
  WRGP0 GPUDRAWABR,0x03BCFF ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=255, Y2=239)
  WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)
  FillRectVRAM 0x000000, 0,0, 256,240 ; Fill Rectangle In VRAM: Color, X,Y, Width,Height
  sw r0,0(a3) ; Double Buffer = 0
  j FrameEnd
  nop ; Delay Slot

  FrameB:
  WRGP1 GPUVRAM,0x000000    ; Write GP1 Command Word (Start Of Display Area: X = 0, Y = 0)
  WRGP0 GPUDRAWATL,0x000100 ; Write GP0 Command Word (Set Drawing Area Top Left X1=256, Y1=0)
  WRGP0 GPUDRAWABR,0x03B27F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=511, Y2=239)
  WRGP0 GPUDRAWOFS,0x000100 ; Write GP0 Command Word (Set Drawing Offset X=256, Y=0)
  FillRectVRAM 0x000000, 256,0, 256,240 ; Fill Rectangle In VRAM: Color, X,Y, Width,Height
  li t0,1 ; T0 = 1
  sw t0,0(a3) ; Double Buffer = 1
  FrameEnd:

  lbu t0,0(a1) ; T0 = Frame Data Byte Flags (Bit 0: Frame Clear Screen, Bit 1: Frame Contains Palette Data, Bit 2: Frame Indexed Mode)
  addiu a1,1   ; Increment Scene Data Address

  andi t1,t0,2 ; T1 = Frame Data Byte Flags Bit 1 (Frame Contains Palette Data)
  beqz t1,SkipPalette
  nop ; Delay Slot

  ; Frame Palette
  lbu t1,0(a1) ; T1 = Palette Bitmask HI Byte
  lbu t2,1(a1) ; T2 = Palette Bitmask LO Byte
  sll t1,8     ; T1 <<= 8
  or t1,t2     ; T1 = Palette Bitmask Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  ; Palette Color 0
  andi t2,t1,0x8000 ; T2 = Palette Bitmask Bit 15 (Palette Color 0 Flag)
  beqz t2,PaletteColor1
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4    ; T2 = Palette Color (BGR888)
  sw t2,0(a2) ; Store Palette Color 0

PaletteColor1: ; Palette Color 1
  andi t2,t1,0x4000 ; T2 = Palette Bitmask Bit 14 (Palette Color 1 Flag)
  beqz t2,PaletteColor2
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4    ; T2 = Palette Color (BGR888)
  sw t2,4(a2) ; Store Palette Color 1

PaletteColor2: ; Palette Color 2
  andi t2,t1,0x2000 ; T2 = Palette Bitmask Bit 13 (Palette Color 2 Flag)
  beqz t2,PaletteColor3
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4    ; T2 = Palette Color (BGR888)
  sw t2,8(a2) ; Store Palette Color 2

PaletteColor3: ; Palette Color 3
  andi t2,t1,0x1000 ; T2 = Palette Bitmask Bit 12 (Palette Color 3 Flag)
  beqz t2,PaletteColor4
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,12(a2) ; Store Palette Color 3

PaletteColor4: ; Palette Color 4
  andi t2,t1,0x0800 ; T2 = Palette Bitmask Bit 11 (Palette Color 4 Flag)
  beqz t2,PaletteColor5
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,16(a2) ; Store Palette Color 4

PaletteColor5: ; Palette Color 5
  andi t2,t1,0x0400 ; T2 = Palette Bitmask Bit 10 (Palette Color 5 Flag)
  beqz t2,PaletteColor6
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,20(a2) ; Store Palette Color 5

PaletteColor6: ; Palette Color 6
  andi t2,t1,0x0200 ; T2 = Palette Bitmask Bit 9 (Palette Color 6 Flag)
  beqz t2,PaletteColor7
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,24(a2) ; Store Palette Color 6

PaletteColor7: ; Palette Color 7
  andi t2,t1,0x0100 ; T2 = Palette Bitmask Bit 8 (Palette Color 7 Flag)
  beqz t2,PaletteColor8
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,28(a2) ; Store Palette Color 7

PaletteColor8: ; Palette Color 8
  andi t2,t1,0x0080 ; T2 = Palette Bitmask Bit 7 (Palette Color 8 Flag)
  beqz t2,PaletteColor9
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,32(a2) ; Store Palette Color 8

PaletteColor9: ; Palette Color 9
  andi t2,t1,0x0040 ; T2 = Palette Bitmask Bit 6 (Palette Color 9 Flag)
  beqz t2,PaletteColor10
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,36(a2) ; Store Palette Color 9

PaletteColor10: ; Palette Color 10
  andi t2,t1,0x0020 ; T2 = Palette Bitmask Bit 5 (Palette Color 10 Flag)
  beqz t2,PaletteColor11
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,40(a2) ; Store Palette Color 10

PaletteColor11: ; Palette Color 11
  andi t2,t1,0x0010 ; T2 = Palette Bitmask Bit 4 (Palette Color 11 Flag)
  beqz t2,PaletteColor12
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,44(a2) ; Store Palette Color 11

PaletteColor12: ; Palette Color 12
  andi t2,t1,0x0008 ; T2 = Palette Bitmask Bit 3 (Palette Color 12 Flag)
  beqz t2,PaletteColor13
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,48(a2) ; Store Palette Color 12

PaletteColor13: ; Palette Color 13
  andi t2,t1,0x0004 ; T2 = Palette Bitmask Bit 2 (Palette Color 13 Flag)
  beqz t2,PaletteColor14
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,52(a2) ; Store Palette Color 13

PaletteColor14: ; Palette Color 14
  andi t2,t1,0x0002 ; T2 = Palette Bitmask Bit 1 (Palette Color 14 Flag)
  beqz t2,PaletteColor15
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,56(a2) ; Store Palette Color 14

PaletteColor15: ; Palette Color 15
  andi t2,t1,0x0001 ; T2 = Palette Bitmask Bit 0 (Palette Color 15 Flag)
  beqz t2,SkipPalette
  nop ; Delay Slot

  lbu t2,0(a1) ; T2 = Palette Color HI Byte
  lbu t3,1(a1) ; T3 = Palette Color LO Byte
  sll t2,8     ; T2 <<= 8
  or t2,t3     ; T2 = Palette Color Word (16-Bits)
  addiu a1,2   ; Increment Scene Data Address

  andi t3,t2,0x000F ; T3 = Blue (4 Bits)
  sll t3,20         ; T3 <<= 20
  andi t4,t2,0x00F0 ; T4 = Green (4 Bits)
  sll t4,8          ; T4 <<= 8
  andi t2,0x0F00    ; T2 = Red (4 Bits)
  srl t2,4          ; T2 >>= 4

  or t2,t3
  or t2,t4     ; T2 = Palette Color (BGR888)
  sw t2,60(a2) ; Store Palette Color 15

SkipPalette:
  andi t1,t0,4 ; T1 = Frame Data Byte Flags Bit 2 (Frame Indexed Mode)
  beqz t1,FrameNonIndexed
  nop ; Delay Slot

  ; Frame Indexed Mode
  lbu t0,0(a1) ; T0 = Number Of Vertices
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1 (Multiply By 2 For X/Y Bytes Length)
  or a3,a1,r0  ; A3 = Vertex Index Data Address
  addu a1,t0   ; Scene Data Address += X/Y Bytes Length

FrameIndexLoop:
  lbu t0,0(a1) ; T0 = Poly-Descripter Byte (Bits 4..7 = Color-Index, Bits 0..3 = Number Of Polygon Vertices (3..15))
               ; (0xFF = End Of Frame, 0xFE = End Of Frame & Skip To Next 64KB Block, 0xFD = End Of Stream)
  addiu a1,1   ; Increment Scene Data Address

  ori t1,r0,0x00FF ; T1 = End Of Frame Byte Code (0xFF)
  beq t0,t1,EndOfFrame
  nop ; Delay Slot

  ori t1,r0,0x00FE ; T1 = End Of Frame & Skip To Next 64KB Block Byte Code (0xFE)
  beq t0,t1,EndOfFrameSkipBlock
  nop ; Delay Slot

  ori t1,r0,0x00FD ; T1 = End Of Stream Byte Code (0xFD)
  beq t0,t1,EndOfStream
  nop ; Delay Slot

  andi t1,t0,0x000F ; T1 = Number Of Polygon Vertices (3..15)

  ori t2,r0,3          ; T2 = 3
  beq t1,t2,IndexPoly3 ; Indexed Polygon (3 Vertices)
  nop ; Delay Slot

  ori t2,r0,4          ; T2 = 4
  beq t1,t2,IndexPoly4 ; Indexed Polygon (4 Vertices)
  nop ; Delay Slot

  ori t2,r0,5          ; T2 = 5
  beq t1,t2,IndexPoly5 ; Indexed Polygon (5 Vertices)
  nop ; Delay Slot

  ori t2,r0,6          ; T2 = 6
  beq t1,t2,IndexPoly6 ; Indexed Polygon (6 Vertices)
  nop ; Delay Slot

  ori t2,r0,7          ; T2 = 7
  beq t1,t2,IndexPoly7 ; Indexed Polygon (7 Vertices)
  nop ; Delay Slot

IndexPoly3: ; Indexed Polygon (3 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 0
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0) ; S0 = Vertex X0
  lbu s1,1(t0) ; S1 = Vertex Y0

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 1
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0) ; S2 = Vertex X1
  lbu s3,1(t0) ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 2
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameIndexLoop ; Frame Index Loop
  nop ; Delay Slot

IndexPoly4: ; Indexed Polygon (4 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 0
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0) ; S0 = Vertex X0
  lbu s1,1(t0) ; S1 = Vertex Y0

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 1
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0) ; S2 = Vertex X1
  lbu s3,1(t0) ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 2
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-3(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 2
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 3
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameIndexLoop ; Frame Index Loop
  nop ; Delay Slot

IndexPoly5: ; Indexed Polygon (5 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 0
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0) ; S0 = Vertex X0
  lbu s1,1(t0) ; S1 = Vertex Y0

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 1
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0) ; S2 = Vertex X1
  lbu s3,1(t0) ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 2
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-3(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 2
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 3
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-4(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 3
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 4
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameIndexLoop ; Frame Index Loop
  nop ; Delay Slot

IndexPoly6: ; Indexed Polygon (6 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 0
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0) ; S0 = Vertex X0
  lbu s1,1(t0) ; S1 = Vertex Y0

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 1
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0) ; S2 = Vertex X1
  lbu s3,1(t0) ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 2
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-3(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 2
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 3
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-4(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 3
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 4
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-5(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 4
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 5
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameIndexLoop ; Frame Index Loop
  nop ; Delay Slot

IndexPoly7: ; Indexed Polygon (7 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 0
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0) ; S0 = Vertex X0
  lbu s1,1(t0) ; S1 = Vertex Y0

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 1
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0) ; S2 = Vertex X1
  lbu s3,1(t0) ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 2
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-3(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 2
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 3
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-4(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 3
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 4
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-5(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 4
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 5
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu t0,-6(a1) ; T0 = Polygon Vertex Index 0
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s0,0(t0)  ; S0 = Vertex X0
  lbu s1,1(t0)  ; S1 = Vertex Y0

  lbu t0,-1(a1) ; T0 = Polygon Vertex Index 5
  nop           ; Delay Slot
  sll t0,1      ; T0 <<= 1
  add t0,a3     ; T0 += A3 (Vertex Index Data Address)
  lbu s2,0(t0)  ; S2 = Vertex X1
  lbu s3,1(t0)  ; S3 = Vertex Y1

  lbu t0,0(a1) ; T0 = Polygon Vertex Index 6
  addiu a1,1   ; Increment Scene Data Address
  sll t0,1     ; T0 <<= 1
  add t0,a3    ; T0 += A3 (Vertex Index Data Address)
  lbu s4,0(t0) ; S4 = Vertex X2
  lbu s5,1(t0) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameIndexLoop ; Frame Index Loop
  nop ; Delay Slot


FrameNonIndexed: ; Frame Non-Indexed Mode
  lbu t0,0(a1) ; T0 = Poly-Descripter Byte (Bits 4..7 = Color-Index, Bits 0..3 = Number Of Polygon Vertices (3..15))
               ; (0xFF = End Of Frame, 0xFE = End Of Frame & Skip To Next 64KB Block, 0xFD = End Of Stream)
  addiu a1,1   ; Increment Scene Data Address

  ori t1,r0,0x00FF ; T1 = End Of Frame Byte Code (0xFF)
  beq t0,t1,EndOfFrame
  nop ; Delay Slot

  ori t1,r0,0x00FE ; T1 = End Of Frame & Skip To Next 64KB Block Byte Code (0xFE)
  beq t0,t1,EndOfFrameSkipBlock
  nop ; Delay Slot

  ori t1,r0,0x00FD ; T1 = End Of Stream Byte Code (0xFD)
  beq t0,t1,EndOfStream
  nop ; Delay Slot

  andi t1,t0,0x000F ; T1 = Number Of Polygon Vertices (3..15)

  ori t2,r0,3             ; T2 = 3
  beq t1,t2,NonIndexPoly3 ; Non-Indexed Polygon (3 Vertices)
  nop ; Delay Slot

  ori t2,r0,4             ; T2 = 4
  beq t1,t2,NonIndexPoly4 ; Non-Indexed Polygon (4 Vertices)
  nop ; Delay Slot

  ori t2,r0,5             ; T2 = 5
  beq t1,t2,NonIndexPoly5 ; Non-Indexed Polygon (5 Vertices)
  nop ; Delay Slot

  ori t2,r0,6             ; T2 = 6
  beq t1,t2,NonIndexPoly6 ; Non-Indexed Polygon (6 Vertices)
  nop ; Delay Slot

  ori t2,r0,7             ; T2 = 7
  beq t1,t2,NonIndexPoly7 ; Non-Indexed Polygon (7 Vertices)
  nop ; Delay Slot

NonIndexPoly3: ; Non-Indexed Polygon (3 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,2(a1) ; S2 = Vertex X1
  lbu s3,3(a1) ; S3 = Vertex Y1
  lbu s4,4(a1) ; S4 = Vertex X2
  lbu s5,5(a1) ; S5 = Vertex Y2
  addiu a1,6   ; Increment Scene Data Address

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameNonIndexed ; Frame Non-Index Loop
  nop ; Delay Slot

NonIndexPoly4: ; Non-Indexed Polygon (4 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,2(a1) ; S2 = Vertex X1
  lbu s3,3(a1) ; S3 = Vertex Y1
  lbu s4,4(a1) ; S4 = Vertex X2
  lbu s5,5(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,4(a1) ; S2 = Vertex X1
  lbu s3,5(a1) ; S3 = Vertex Y1
  lbu s4,6(a1) ; S4 = Vertex X2
  lbu s5,7(a1) ; S5 = Vertex Y2
  addiu a1,8   ; Increment Scene Data Address

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameNonIndexed ; Frame Non-Index Loop
  nop ; Delay Slot

NonIndexPoly5: ; Non-Indexed Polygon (5 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,2(a1) ; S2 = Vertex X1
  lbu s3,3(a1) ; S3 = Vertex Y1
  lbu s4,4(a1) ; S4 = Vertex X2
  lbu s5,5(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,4(a1) ; S2 = Vertex X1
  lbu s3,5(a1) ; S3 = Vertex Y1
  lbu s4,6(a1) ; S4 = Vertex X2
  lbu s5,7(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,6(a1) ; S2 = Vertex X1
  lbu s3,7(a1) ; S3 = Vertex Y1
  lbu s4,8(a1) ; S4 = Vertex X2
  lbu s5,9(a1) ; S5 = Vertex Y2
  addiu a1,10  ; Increment Scene Data Address

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameNonIndexed ; Frame Non-Index Loop
  nop ; Delay Slot

NonIndexPoly6: ; Non-Indexed Polygon (6 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,2(a1) ; S2 = Vertex X1
  lbu s3,3(a1) ; S3 = Vertex Y1
  lbu s4,4(a1) ; S4 = Vertex X2
  lbu s5,5(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,4(a1) ; S2 = Vertex X1
  lbu s3,5(a1) ; S3 = Vertex Y1
  lbu s4,6(a1) ; S4 = Vertex X2
  lbu s5,7(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,6(a1) ; S2 = Vertex X1
  lbu s3,7(a1) ; S3 = Vertex Y1
  lbu s4,8(a1) ; S4 = Vertex X2
  lbu s5,9(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1)  ; S0 = Vertex X0
  lbu s1,1(a1)  ; S1 = Vertex Y0
  lbu s2,8(a1)  ; S2 = Vertex X1
  lbu s3,9(a1)  ; S3 = Vertex Y1
  lbu s4,10(a1) ; S4 = Vertex X2
  lbu s5,11(a1) ; S5 = Vertex Y2
  addiu a1,12   ; Increment Scene Data Address

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameNonIndexed ; Frame Non-Index Loop
  nop ; Delay Slot

NonIndexPoly7: ; Non-Indexed Polygon (7 Vertices)
  srl t0,4    ; T0 >>= 4 (Polygon Palette Color Index)
  sll t0,2    ; T0 <<= 2
  add t0,a2   ; T0 += Palette Color Data Address
  lw s6,0(t0) ; S6 = Palette Color

  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,2(a1) ; S2 = Vertex X1
  lbu s3,3(a1) ; S3 = Vertex Y1
  lbu s4,4(a1) ; S4 = Vertex X2
  lbu s5,5(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,4(a1) ; S2 = Vertex X1
  lbu s3,5(a1) ; S3 = Vertex Y1
  lbu s4,6(a1) ; S4 = Vertex X2
  lbu s5,7(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1) ; S0 = Vertex X0
  lbu s1,1(a1) ; S1 = Vertex Y0
  lbu s2,6(a1) ; S2 = Vertex X1
  lbu s3,7(a1) ; S3 = Vertex Y1
  lbu s4,8(a1) ; S4 = Vertex X2
  lbu s5,9(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1)  ; S0 = Vertex X0
  lbu s1,1(a1)  ; S1 = Vertex Y0
  lbu s2,8(a1)  ; S2 = Vertex X1
  lbu s3,9(a1)  ; S3 = Vertex Y1
  lbu s4,10(a1) ; S4 = Vertex X2
  lbu s5,11(a1) ; S5 = Vertex Y2

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot


  lbu s0,0(a1)  ; S0 = Vertex X0
  lbu s1,1(a1)  ; S1 = Vertex Y0
  lbu s2,10(a1) ; S2 = Vertex X1
  lbu s3,11(a1) ; S3 = Vertex Y1
  lbu s4,12(a1) ; S4 = Vertex X2
  lbu s5,13(a1) ; S5 = Vertex Y2
  addiu a1,14   ; Increment Scene Data Address

  jal PlotFillTriangle ; Plot Fill Triangle
  nop ; Delay Slot

  j FrameNonIndexed ; Frame Non-Index Loop
  nop ; Delay Slot


EndOfFrame: ; End Of Frame
  b LoopFrames
  nop ; Delay Slot

EndOfFrameSkipBlock: ; End Of Frame & Skip To Next 64KB Block
  lui t0,0xFFFF ; T0 = $FFFF0000
  and a1,t0     ; Scene Data Address &= 0xFFFF0000
  lui t0,0x0001 ; T0 = 0x00010000
  addu a1,t0    ; Scene Data Address += 0x00010000 (Next 64KB Block)

  b LoopFrames
  nop ; Delay Slot

EndOfStream: ; End Of Stream
  la a1,SceneData ; A1 = Scene Data Start Address

  b LoopFrames
  nop ; Delay Slot

PlotFillTriangle: ; Plot Fill Triangle (S0=X0, S1=Y0, S2=X1, S3=Y1, S4=X2, S5=Y2, S6 = Color)
  lui t0,0x2000 ; T0 = 0x20000000 (Monochrome Triangle, Opaque Command)
  or t0,s6      ; T0 = Color+Command (Command Word)
  sw t0,GP0(a0) ; I/O Port Register Word = T0

  sll t0,s1,16  ; T0 = Y0 << 16
  or t0,s0      ; T0 = X0+Y0 (Vertex1 Packet Word)
  sw t0,GP0(a0) ; I/O Port Register Word = T0

  sll t0,s3,16  ; T0 = Y1 << 16
  or t0,s2      ; T0 = X1+Y1 (Vertex2 Packet Word)
  sw t0,GP0(a0) ; I/O Port Register Word = T0

  sll t0,s5,16  ; T0 = Y2 << 16
  or t0,s4      ; T0 = X2+Y2 (Vertex3 Packet Word)
  sw t0,GP0(a0) ; I/O Port Register Word = T0

  jr ra ; Return
  nop   ; Delay Slot

DoubleBuffer:
  dw 0 ; Double Buffer (0 = Frame Buffer A, 1 = Frame Buffer B)

PadBuffer:
  dw 0 ; Pad Buffer (Automatically Stored Every Frame)
PadData:
  dw 0 ; Pad Data (Read From VSync Routine)

Palette:
  dw 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ; Palette Color Data (16 Colors, RGBA8888, 64 Bytes)

.align 65536 ; Align 64KB Block
SceneData:
  .incbin "scene1.bin" ; Scene Data (1800 Frames, 639976 Bytes)

.close