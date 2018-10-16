; PSX 'Bare Metal' GPU 16BPP Render Texture Polygon 15BPP Dither Demo by krom (Peter Lemon):
.psx
.create "RenderTexturePolygon15BPPDither.bin", 0x80010000

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
WRGP0 GPUDRAWM,0x000508   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BD3F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=319, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

; Copy Textures To VRAM
CopyRectCPU 512,0, 32,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,127 ; T0 = Data Copy Word Count
la a1,Texture8x8 ; A1 = Texture RAM Offset
CopyTexture8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTexture8x8 ; IF (T0 != 0) Copy Texture8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Polygons
WRGP0 GPUDRAWM,0x000508 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x000080,8,8, 0,0, 0, 0x000080,312,8, 32,0, 0x108, 0x000080,8,32, 0,8, 0x000080,312,32, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

WRGP0 GPUDRAWM,0x000708 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x000080,8,40, 0,0, 0, 0x000080,312,40, 32,0, 0x108, 0x000080,8,64, 0,8, 0x000080,312,64, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

WRGP0 GPUDRAWM,0x000508 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x008000,8,72, 0,0, 0, 0x008000,312,72, 32,0, 0x108, 0x008000,8,96, 0,8, 0x008000,312,96, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

WRGP0 GPUDRAWM,0x000708 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x008000,8,104, 0,0, 0, 0x008000,312,104, 32,0, 0x108, 0x008000,8,128, 0,8, 0x008000,312,128, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

WRGP0 GPUDRAWM,0x000508 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Disabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x800000,8,136, 0,0, 0, 0x800000,312,136, 32,0, 0x108, 0x800000,8,160, 0,8, 0x800000,312,160, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

WRGP0 GPUDRAWM,0x000708 ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Dither Enabled Bit 9, Texture Page Colors = 15BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
ShadeTexQuad 0x800000,8,168, 0,0, 0, 0x800000,312,168, 32,0, 0x108, 0x800000,8,192, 0,8, 0x800000,312,192, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

ShadeTexQuad 0x000080,8,200, 0,0, 0, 0x008000,312,200, 32,0, 0x108, 0x800000,8,224, 0,8, 0x008080,312,224, 32,8 ; Shaded Texture Quad: Color1,X1,Y1, U1,V1, PAL, Color2,X2,Y2, U2,V2, TEX, Color3,X3,Y3, U3,V3, Color4,X4,Y4, U4,V4

Loop:
  b Loop
  nop ; Delay Slot

Texture8x8:
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF // 32x8x16B = 512 Bytes
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
  dh 0x8000,0x8421,0x8842,0x8C63,0x9084,0x94A5,0x98C6,0x9CE7,0xA108,0xA529,0xA94A,0xAD6B,0xB18C,0xB5AD,0xB9CE,0xBDEF,0xC210,0xC631,0xCA52,0xCE73,0xD294,0xD6B5,0xDAD6,0xDEF7,0xE318,0xE739,0xEB5A,0xEF7B,0xF39C,0xF7BD,0xFBDE,0xFFFF
.close