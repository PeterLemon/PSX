ShadeTexCubeQuad: ; X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3,X4,Y4,Z4,COMMAND+COLOR1,COLOR2,COLOR3,COLOR4,U1,V1,PAL,U2,V2,TEX,U3,V3,U4,V4
  dw -2560, -2560, -2560 ; X1,Y1,Z1: Quad 1 Front Top Left
  dw  2560, -2560, -2560 ; X2,Y2,Z2: Quad 1 Front Top Right
  dw -2560,  2560, -2560 ; X3,Y3,Z3: Quad 1 Front Bottom Left
  dw  2560,  2560, -2560 ; X4,Y4,Z4: Quad 1 Front Bottom Right
  dw 0x3C808080 ; Quad 1 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x808080 ; Quad 1 Color2: B,G,R
  dw 0x808080 ; Quad 1 Color3: B,G,R
  dw 0x808080 ; Quad 1 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 1 Front Top Left
  dh 0x000   ; PAL: Quad 1 Front
  db 255,0   ; U2,V2: Quad 1 Front Top Right
  dh 0x108   ; TEX: Quad 1 Front
  db   0,255 ; U3,V3: Quad 1 Front Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 1 Front Bottom Right
  dh 0       ; Padding

  dw  2560, -2560, -2560 ; X1,Y1,Z1: Quad 2 Right Top Left
  dw  2560, -2560,  2560 ; X2,Y2,Z2: Quad 2 Right Top Right
  dw  2560,  2560, -2560 ; X3,Y3,Z3: Quad 2 Right Bottom Left
  dw  2560,  2560,  2560 ; X4,Y4,Z4: Quad 2 Right Bottom Right
  dw 0x3C808080 ; Quad 2 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x202020 ; Quad 2 Color2: B,G,R
  dw 0x808080 ; Quad 2 Color3: B,G,R
  dw 0x202020 ; Quad 2 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 2 Right Top Left
  dh 0x000   ; PAL: Quad 2 Right
  db 255,0   ; U2,V2: Quad 2 Right Top Right
  dh 0x10C   ; TEX: Quad 2 Right
  db   0,255 ; U3,V3: Quad 2 Right Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 2 Right Bottom Right
  dh 0       ; Padding

  dw  2560, -2560,  2560 ; X1,Y1,Z1: Quad 3 Back Top Left
  dw -2560, -2560,  2560 ; X2,Y2,Z2: Quad 3 Back Top Right
  dw  2560,  2560,  2560 ; X3,Y3,Z3: Quad 3 Back Bottom Left
  dw -2560,  2560,  2560 ; X4,Y4,Z4: Quad 3 Back Bottom Right
  dw 0x3C202020 ; Quad 3 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x202020 ; Quad 3 Color2: B,G,R
  dw 0x202020 ; Quad 3 Color3: B,G,R
  dw 0x202020 ; Quad 3 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 3 Back Top Left
  dh 0x000   ; PAL: Quad 3 Back
  db 255,0   ; U2,V2: Quad 3 Back Top Right
  dh 0x110   ; TEX: Quad 3 Back
  db   0,255 ; U3,V3: Quad 3 Back Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 3 Back Bottom Right
  dh 0       ; Padding

  dw -2560, -2560,  2560 ; X1,Y1,Z1: Quad 4 Left Top Left
  dw -2560, -2560, -2560 ; X2,Y2,Z2: Quad 4 Left Top Right
  dw -2560,  2560,  2560 ; X3,Y3,Z3: Quad 4 Left Bottom Left
  dw -2560,  2560, -2560 ; X4,Y4,Z4: Quad 4 Left Bottom Right
  dw 0x3C202020 ; Quad 4 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x808080 ; Quad 4 Color2: B,G,R
  dw 0x202020 ; Quad 4 Color3: B,G,R
  dw 0x808080 ; Quad 4 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 4 Left Top Left
  dh 0x000   ; PAL: Quad 4 Left
  db 255,0   ; U2,V2: Quad 4 Left Top Right
  dh 0x114   ; TEX: Quad 4 Left
  db   0,255 ; U3,V3: Quad 4 Left Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 4 Left Bottom Right
  dh 0       ; Padding

  dw -2560, -2560,  2560 ; X1,Y1,Z1: Quad 5 Top Top Left
  dw  2560, -2560,  2560 ; X2,Y2,Z2: Quad 5 Top Top Right
  dw -2560, -2560, -2560 ; X3,Y3,Z3: Quad 5 Top Bottom Left
  dw  2560, -2560, -2560 ; X4,Y4,Z4: Quad 5 Top Bottom Right
  dw 0x3C202020 ; Quad 5 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x202020 ; Quad 5 Color2: B,G,R
  dw 0x808080 ; Quad 5 Color3: B,G,R
  dw 0x808080 ; Quad 5 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 5 Top Top Left
  dh 0x000   ; PAL: Quad 5 Top
  db 255,0   ; U2,V2: Quad 5 Top Top Right
  dh 0x118   ; TEX: Quad 5 Top
  db   0,255 ; U3,V3: Quad 5 Top Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 5 Top Bottom Right
  dh 0       ; Padding

  dw -2560,  2560, -2560 ; X1,Y1,Z1: Quad 6 Bottom Top Left
  dw  2560,  2560, -2560 ; X2,Y2,Z2: Quad 6 Bottom Top Right
  dw -2560,  2560,  2560 ; X3,Y3,Z3: Quad 6 Bottom Bottom Left
  dw  2560,  2560,  2560 ; X4,Y4,Z4: Quad 6 Bottom Bottom Right
  dw 0x3C808080 ; Quad 6 Command+Color1: ShadeTexQuad+B,G,R
  dw 0x808080 ; Quad 6 Color2: B,G,R
  dw 0x202020 ; Quad 6 Color3: B,G,R
  dw 0x202020 ; Quad 6 Color4: B,G,R
  db   0,0   ; U1,V1: Quad 6 Bottom Top Left
  dh 0x000   ; PAL: Quad 6 Bottom
  db 255,0   ; U2,V2: Quad 6 Bottom Top Right
  dh 0x11C   ; TEX: Quad 6 Bottom
  db   0,255 ; U3,V3: Quad 6 Bottom Bottom Left
  dh 0       ; Padding
  db 255,255 ; U4,V4: Quad 6 Bottom Bottom Right
  dh 0       ; Padding
ShadeTexCubeQuadEnd: