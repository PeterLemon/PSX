; PSX 'Bare Metal' GPU 16BPP Render Line Demo by krom (Peter Lemon):
.psx
.create "RenderLine16BPP.bin", 0

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

; Render Lines
FillLine 0x0000FF, 33,8, 54,29   ; Fill Line: Color, X1,Y1, X2,Y2
FillLine 0x00FF00, 129,8, 150,29 ; Fill Line: Color, X1,Y1, X2,Y2
FillLine 0xFF0000, 225,8, 246,29 ; Fill Line: Color, X1,Y1, X2,Y2

FillLineAlpha 0x00FF00, 32,29, 53,8   ; Fill Line Alpha: Color, X1,Y1, X2,Y2
FillLineAlpha 0xFF0000, 128,29, 149,8 ; Fill Line Alpha: Color, X1,Y1, X2,Y2
FillLineAlpha 0x0000FF, 224,29, 245,8 ; Fill Line Alpha: Color, X1,Y1, X2,Y2

FillPolyLine 0x0000FF, 33,33, 54,54 ; Fill Poly-Line: Color, X1,Y1, X2,Y2
FillPolyLineVert 74,33              ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                         ; Poly-Line: Termination Code

FillPolyLine 0x00FF00, 129,33, 150,54 ; Fill Poly-Line: Color, X1,Y1, X2,Y2
FillPolyLineVert 170,33               ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                           ; Poly-Line: Termination Code

FillPolyLine 0xFF0000, 225,33, 246,54 ; Fill Poly-Line: Color, X1,Y1, X2,Y2
FillPolyLineVert 266,33               ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                           ; Poly-Line: Termination Code

FillPolyLineAlpha 0x00FF00, 32,54, 53,33 ; Fill Poly-Line Alpha: Color, X1,Y1, X2,Y2
FillPolyLineVert 74,54                   ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                              ; Poly-Line: Termination Code

FillPolyLineAlpha 0xFF0000, 128,54, 149,33 ; Fill Poly-Line Alpha: Color, X1,Y1, X2,Y2
FillPolyLineVert 170,54                    ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                                ; Poly-Line: Termination Code

FillPolyLineAlpha 0x0000FF, 224,54, 245,33 ; Fill Poly-Line Alpha: Color, X1,Y1, X2,Y2
FillPolyLineVert 266,54                    ; Fill Poly-Line Vertex: X,Y
PolyLineEnd                                ; Poly-Line: Termination Code

FillTriLine 0x0000FF, 64,54, 96,104, 32,104    ; Fill Triangle Line: Color, X1,Y1, X2,Y2, X3,Y3
FillTriLine 0x00FF00, 160,54, 192,104, 128,104 ; Fill Triangle Line: Color, X1,Y1, X2,Y2, X3,Y3
FillTriLine 0xFF0000, 256,54, 288,104, 224,104 ; Fill Triangle Line: Color, X1,Y1, X2,Y2, X3,Y3

FillTriLineAlpha 0x00FF00, 64,62, 96,112, 32,112    ; Fill Triangle Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillTriLineAlpha 0xFF0000, 160,62, 192,112, 128,112 ; Fill Triangle Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3
FillTriLineAlpha 0x0000FF, 256,62, 288,112, 224,112 ; Fill Triangle Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3

FillQuadLine 0x0000FF, 32,116, 74,116, 74,152, 32,152     ; Fill Quad Line: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuadLine 0x00FF00, 128,116, 170,116, 170,152, 128,152 ; Fill Quad Line: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuadLine 0xFF0000, 224,116, 266,116, 266,152, 224,152 ; Fill Quad Line: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4

FillQuadLineAlpha 0x00FF00, 54,124, 96,124, 96,160, 54,160     ; Fill Quad Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuadLineAlpha 0xFF0000, 150,124, 192,124, 192,160, 150,160 ; Fill Quad Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4
FillQuadLineAlpha 0x0000FF, 246,124, 288,124, 288,160, 246,160 ; Fill Quad Line Alpha: Color, X1,Y1, X2,Y2, X3,Y3, X4,Y4

ShadeLine 0x0000FF,33,164, 0x00FF00,54,185 ; Shaded Line: Color1,X1,Y1, Color2,X2,Y2

ShadeLineAlpha 0xFF0000,32,185, 0x0000FF,53,164 ; Shaded Line Alpha: Color1,X1,Y1, Color2,X2,Y2

ShadePolyLine 0x0000FF,74,164, 0x00FF00,95,185 ; Shaded Poly-Line: Color1,X1,Y1, Color2,X2,Y2
ShadePolyLineVert 0xFF0000,115,164               ; Shaded Poly-Line Vertex: Color,X,Y
PolyLineEnd                                      ; Poly-Line: Termination Code

ShadePolyLineAlpha 0x00FF00,73,185, 0xFF0000,94,164 ; Shaded Poly-Line Alpha: Color1,X1,Y1, Color2,X2,Y2
ShadePolyLineVert 0x0000FF,115,185                  ; Shaded Poly-Line Vertex: Color,X,Y
PolyLineEnd                                         ; Poly-Line: Termination Code

ShadeTriLine 0x0000FF,160,164, 0x00FF00,192,214, 0xFF0000,128,214 ; Shaded Triangle Line: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3

ShadeTriLineAlpha 0x00FF00,168,168, 0xFF0000,200,218, 0x0000FF,136,218 ; Shaded Triangle Line Alpha: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3

ShadeQuadLine 0x0000FF,224,164, 0x00FF00,266,164, 0xFF0000,266,200, 0x00FFFF,224,200 ; Shaded Quad Line: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

ShadeQuadLineAlpha 0x00FF00,246,172, 0xFF0000,288,172, 0xFFFF00,288,208, 0x0000FF,246,208 ; Shaded Quad Line Alpha: Color1,X1,Y1, Color2,X2,Y2, Color3,X3,Y3, Color4,X4,Y4

Loop:
  b Loop
  nop ; Delay Slot

.close