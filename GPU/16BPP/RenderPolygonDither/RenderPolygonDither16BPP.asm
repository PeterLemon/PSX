; PSX 'Bare Metal' GPU 16BPP Render Polygon Dither Demo by krom (Peter Lemon):
.psx
.create "RenderPolygonDither16BPP.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros

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

; Render Polygons
WRGP0 GPUDRAWM,0x000400 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9)
ShadeQuad 0x000000,8,8, 0x0000FF,312,8, 0x000000,8,32, 0x0000FF,312,32 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

WRGP0 GPUDRAWM,0x000600 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9)
ShadeQuad 0x000000,8,40, 0x0000FF,312,40, 0x000000,8,64, 0x0000FF,312,64 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

WRGP0 GPUDRAWM,0x000400 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9)
ShadeQuad 0x000000,8,72, 0x00FF00,312,72, 0x000000,8,96, 0x00FF00,312,96 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

WRGP0 GPUDRAWM,0x000600 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9)
ShadeQuad 0x000000,8,104, 0x00FF00,312,104, 0x000000,8,128, 0x00FF00,312,128 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

WRGP0 GPUDRAWM,0x000400 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9)
ShadeQuad 0x000000,8,136, 0xFF0000,312,136, 0x000000,8,160, 0xFF0000,312,160 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

WRGP0 GPUDRAWM,0x000600 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9)
ShadeQuad 0x000000,8,168, 0xFF0000,312,168, 0x000000,8,192, 0xFF0000,312,192 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

ShadeQuad 0x0000FF,8,200, 0x00FF00,312,200, 0xFF0000,8,224, 0x00FFFF,312,224 ; Shaded Quad: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

Loop:
  b Loop
  nop ; Delay Slot

.close