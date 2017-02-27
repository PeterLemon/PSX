; PSX 'Bare Metal' 16BPP Render Polygon Demo by krom (Peter Lemon):
.psx
.create "RenderPolygon16BPP.bin", 0

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

; Render Polygons
FillTri 0x0000FF, 64,8, 96,56, 32,56    ; Fill Triangle: Color, X1,Y1, X2,Y2, X3,Y3
FillTri 0x00FF00, 160,8, 192,56, 128,56 ; Fill Triangle: Color, X1,Y1, X2,Y2, X3,Y3
FillTri 0xFF0000, 256,8, 288,56, 224,56 ; Fill Triangle: Color, X1,Y1, X2,Y2, X3,Y3

FillTriAlpha 0x00FF00, 64,32, 96,80, 32,80    ; Fill Triangle Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillTriAlpha 0xFF0000, 160,32, 192,80, 128,80 ; Fill Triangle Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillTriAlpha 0x0000FF, 256,32, 288,80, 224,80 ; Fill Triangle Alpha: Color, X1,Y1, X2,Y2, X3,Y3

FillQuad 0x0000FF, 32,88, 74,88, 32,124, 74,124     ; Fill Quad: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuad 0x00FF00, 128,88, 170,88, 128,124, 170,124 ; Fill Quad: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuad 0xFF0000, 224,88, 266,88, 224,124, 266,124 ; Fill Quad: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4

FillQuadAlpha 0x00FF00, 54,106, 96,106, 54,142, 96,142     ; Fill Quad Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillQuadAlpha 0xFF0000, 150,106, 192,106, 150,142, 192,142 ; Fill Quad Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillQuadAlpha 0x0000FF, 246,106, 288,106, 246,142, 288,142 ; Fill Quad Alpha: Color, X1,Y1, X2,Y2, X3,Y3

ShadeTri 0x0000FF,64,148, 0x00FF00,96,196, 0xFF0000,32,196 ; Shaded Triangle: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3

ShadeTriAlpha 0x00FF00,96,164, 0xFF0000,128,212, 0x0000FF,64,212 ; Shaded Triangle Alpha: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3

ShadeQuad 0x0000FF,224,148, 0x00FF00,266,148, 0xFF0000,224,184, 0xFF00FF,266,184 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

ShadeQuadAlpha 0x00FF00,246,166, 0xFF00FF,288,166, 0x0000FF,246,202, 0xFF0000,288,202 ; Shaded Quad Alpha: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

Loop:
  b Loop
  nop ; Delay Slot

.close