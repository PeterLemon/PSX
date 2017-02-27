; PSX 'Bare Metal' 16BPP Render Rectangle Demo by krom (Peter Lemon):
.psx
.create "RenderRectangle16BPP.bin", 0

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup Screen Mode
WRGP1 GPURESET,0  ; Write GP1 Command Word (Reset GPU)
WRGP1 GPUDISPEN,0 ; Write GP1 Command Word (Enable Display)
WRGP1 GPUDISPM,HRES320+VRES240+BPP15+VNTSC ; Write GP1 Command Word (Set Display Mode: 320x240, 15BPP, NTSC)
WRGP1 GPUDISPH,0xC60260 ; Write GP1 Command Word (Horizontal Display Range 608..3168)
WRGP1 GPUDISPV,0x041C17 ; Write GP1 Command Word (Vertical Display Range 23..263)

; Setup Drawing Area
WRGP0 GPUDRAWM,0x000400   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BD3F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=319, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

; Render Rectangles
FillRect 0x0000FF, 32,8, 42,36  ; Fill Rectangle (Variable Size): Color, X,Y, Width,Height
FillRect 0x00FF00, 128,8, 42,36 ; Fill Rectangle (Variable Size): Color, X,Y, Width,Height
FillRect 0xFF0000, 224,8, 42,36 ; Fill Rectangle (Variable Size): Color, X,Y, Width,Height

FillRectAlpha 0x00FF00, 54,26, 42,36  ; Fill Rectangle Alpha (Variable Size): Color, X,Y, Width,Height
FillRectAlpha 0xFF0000, 150,26, 42,36 ; Fill Rectangle Alpha (Variable Size): Color, X,Y, Width,Height
FillRectAlpha 0x0000FF, 246,26, 42,36 ; Fill Rectangle Alpha (Variable Size): Color, X,Y, Width,Height

FillRect1x1 0x0000FF, 85,72  ; Fill Rectangle Dot (1x1): Color, X,Y
FillRect1x1 0x00FF00, 181,72 ; Fill Rectangle Dot (1x1): Color, X,Y
FillRect1x1 0xFF0000, 277,72 ; Fill Rectangle Dot (1x1): Color, X,Y

FillRectAlpha1x1 0x0000FF, 85,52  ; Fill Rectangle Dot Alpha (1x1): Color, X,Y
FillRectAlpha1x1 0x00FF00, 181,52 ; Fill Rectangle Dot Alpha (1x1): Color, X,Y
FillRectAlpha1x1 0xFF0000, 277,52 ; Fill Rectangle Dot Alpha (1x1): Color, X,Y

FillRect8x8 0x0000FF, 32,92  ; Fill Rectangle (8x8): Color, X,Y
FillRect8x8 0x00FF00, 128,92 ; Fill Rectangle (8x8): Color, X,Y
FillRect8x8 0xFF0000, 224,92 ; Fill Rectangle (8x8): Color, X,Y

FillRectAlpha8x8 0x00FF00, 36,96  ; Fill Rectangle Alpha (8x8): Color, X,Y
FillRectAlpha8x8 0xFF0000, 132,96 ; Fill Rectangle Alpha (8x8): Color, X,Y
FillRectAlpha8x8 0x0000FF, 228,96 ; Fill Rectangle Alpha (8x8): Color, X,Y

FillRect16x16 0x0000FF, 32,126  ; Fill Rectangle (16x16): Color, X,Y
FillRect16x16 0x00FF00, 128,126 ; Fill Rectangle (16x16): Color, X,Y
FillRect16x16 0xFF0000, 224,126 ; Fill Rectangle (16x16): Color, X,Y

FillRectAlpha16x16 0x00FF00, 40,134  ; Fill Rectangle Alpha (16x16): Color, X,Y
FillRectAlpha16x16 0xFF0000, 136,134 ; Fill Rectangle Alpha (16x16): Color, X,Y
FillRectAlpha16x16 0x0000FF, 232,134 ; Fill Rectangle Alpha (16x16): Color, X,Y

Loop:
  b Loop
  nop ; Delay Slot

.close