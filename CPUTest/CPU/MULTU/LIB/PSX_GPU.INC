;=========
; PSX GPU
;=========
;==========================
; GPU Other Commands (GP0)
;==========================
GPUNOP    equ 0x00 ; GPU Other Commands (GP0): $00 - NOP
GPUIRQREQ equ 0x1F ; GPU Other Commands (GP0): $1F - Interrupt Request (IRQ1)

;====================================
; GPU Memory Transfer Commands (GP0)
;====================================
GPUCLRCACHE                    equ 0x01 ; GPU Memory Transfer Commands (GP0): $01 - Clear Cache

GPUFILLRECTVRAM                equ 0x02 ; GPU Memory Transfer Commands (GP0): $02 - Fill Rectangle In VRAM
.macro FillRectVRAM,COLOR,X,Y,WIDTH,HEIGHT ; GP0($02) - Fill Rectangle In VRAM
  WRIOW GP0,(0x02<<24)+(COLOR&0xFFFFFF) ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)          ; Write GP0  Packet Word (Top Left Corner:  X Counted In Halfwords, Steps Of $10)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF) ; Write GP0  Packet Word (Width+Height: Width Counted In Halfwords, Steps Of $10)
.endmacro
.macro FillRectVRAMV,COLOR,X,Y,WIDTH,HEIGHT ; GP0($02) - Fill Rectangle In VRAM Variable
  lui t0,0x0200    ; Fill Rectangle In VRAM Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0 Packet Word (Top Left Corner: X Counted In Halfwords, Steps Of $10)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0 Packet Word (Width+Height: Width Counted In Halfwords, Steps Of $10)
.endmacro

GPUCOPYRECTVRAM                equ 0x80 ; GPU Memory Transfer Commands (GP0): $80 - Copy Rectangle (VRAM To VRAM)
.macro CopyRectVRAM,X1,Y1,X2,Y2,WIDTH,HEIGHT ; GP0($80) - Copy Rectangle (VRAM To VRAM)
  WRIOW GP0,0x80<<24                    ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)        ; Write GP0  Packet Word (Source Coord:      X Counted In Halfwords)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)        ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF) ; Write GP0  Packet Word (Width+Height:  Width Counted In Halfwords)
.endmacro
.macro CopyRectVRAMV,X1,Y1,X2,Y2,WIDTH,HEIGHT ; GP0($80) - Copy Rectangle (VRAM To VRAM) Variable
  lui t0,0x8000 ; Copy Rectangle (VRAM To VRAM) Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Source Coord: X Counted In Halfwords)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0 Packet Word (Width+Height: Width Counted In Halfwords)
.endmacro

GPUCOPYRECTCPU                 equ 0xA0 ; GPU Memory Transfer Commands (GP0): $A0 - Copy Rectangle (CPU To VRAM)
.macro CopyRectCPU,X,Y,WIDTH,HEIGHT     ; GP0($A0) - Copy Rectangle (CPU To VRAM)
  WRIOW GP0,0xA0<<24                    ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)          ; Write GP0  Packet Word (Destination Coord: X Counted In Halfwords)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF) ; Write GP0  Packet Word (Width+Height:  Width Counted In Halfwords)
                                        ; Write GP0  Packet Word (Data)
.endmacro
.macro CopyRectCPUV,X,Y,WIDTH,HEIGHT    ; GP0($A0) - Copy Rectangle (CPU To VRAM) Variable
  lui t0,0xA000 ; Copy Rectangle (CPU To VRAM) Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0 Packet Word (Destination Coord: X Counted In Halfwords)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0 Packet Word (Width+Height: Width Counted In Halfwords)
                    ; Write GP0 Packet Word (Data)
.endmacro

GPUCOPYRECTVRAMCPU             equ 0xC0 ; GPU Memory Transfer Commands (GP0): $C0 - Copy Rectangle (VRAM To CPU)
.macro CopyRectVRAMCPU,X,Y,WIDTH,HEIGHT ; GP0($C0) - Copy Rectangle (VRAM To CPU)
  WRIOW GP0,0xC0<<24                    ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)          ; Write GP0  Packet Word (Source Coord:     X Counted In Halfwords)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF) ; Write GP0  Packet Word (Width+Height: Width Counted In Halfwords)
                                        ; Read  GP0  Packet Word (Data)
.endmacro
.macro CopyRectVRAMCPUV,X,Y,WIDTH,HEIGHT ; GP0($C0) - Copy Rectangle (VRAM To CPU)
  lui t0,0xC000 ; Copy Rectangle (VRAM To CPU) Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0 Packet Word (Source Coord: X Counted In Halfwords)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0 Packet Word (Width+Height: Width Counted In Halfwords)
                    ; Read  GP0 Packet Word (Data)
.endmacro

;===================================
; GPU Render Polygon Commands (GP0)
;===================================
GPUFILLTRI                            equ 0x20 ; GPU Render Polygon Commands (GP0): $20 - Monochrome Triangle, Opaque
.macro FillTri,COLOR,X1,Y1,X2,Y2,X3,Y3         ; GP0($20) - Monochrome Triangle Polygon, Opaque
  WRIOW GP0,(0x20<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
.endmacro
.macro FillTriV,COLOR,X1,Y1,X2,Y2,X3,Y3        ; GP0($20) - Monochrome Triangle Polygon, Opaque Variable
  lui t0,0x2000    ; Monochrome Triangle Polygon, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)
.endmacro

GPUFILLTRIALPHA                       equ 0x22 ; GPU Render Polygon Commands (GP0): $22 - Monochrome Triangle, Semi-Transparent
.macro FillTriAlpha,COLOR,X1,Y1,X2,Y2,X3,Y3    ; GP0($22) - Monochrome Triangle polygon, Semi-Transparent
  WRIOW GP0,(0x22<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
.endmacro
.macro FillTriAlphaV,COLOR,X1,Y1,X2,Y2,X3,Y3   ; GP0($22) - Monochrome Triangle polygon, Semi-Transparent Variable
  lui t0,0x2200    ; Monochrome Triangle Polygon, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)
.endmacro

GPUTEXTRIBLEND                        equ 0x24 ; GPU Render Polygon Commands (GP0): $24 - Textured Triangle, Opaque, Texture-Blending
.macro TexTriBlend,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($24) - Textured Triangle, Opaque, Texture-Blending
  WRIOW GP0,(0x24<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro TexTriBlendV,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($24) - Textured Triangle, Opaque, Texture-Blending Variable
  lui t0,0x2400    ; Textured Triangle, Opaque, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUTEXTRIRAW                          equ 0x25 ; GPU Render Polygon Commands (GP0): $25 - Textured Triangle, Opaque, Raw-Texture
.macro TexTriRaw,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($25) - Textured Triangle, Opaque, Raw-Texture
  WRIOW GP0,(0x25<<24)                         ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro TexTriRawV,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($25) - Textured Triangle, Opaque, Raw-Texture Variable
  lui t0,0x2500 ; Textured Triangle, Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUTEXTRIBLENDALPHA                   equ 0x26 ; GPU Render Polygon Commands (GP0): $26 - Textured Triangle, Semi-Transparent, Texture-Blending
.macro TexTriBlendAlpha,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($26) - Textured Triangle, Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x26<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro TexTriBlendAlphaV,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($26) - Textured Triangle, Semi-Transparent, Texture-Blending Variable
  lui t0,0x2600    ; Textured Triangle, Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUTEXTRIRAWALPHA                     equ 0x27 ; GPU Render Polygon Commands (GP0): $27 - Textured Triangle, Semi-Transparent, Raw-Texture
.macro TexTriRawAlpha,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($27) - Textured Triangle, Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x27<<24)                         ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro TexTriRawAlphaV,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3 ; GP0($27) - Textured Triangle, Semi-Transparent, Raw-Texture Variable
  lui t0,0x2700 ; Textured Triangle, Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUFILLQUAD                           equ 0x28 ; GPU Render Polygon Commands (GP0): $28 - Monochrome Quad, Opaque
.macro FillQuad,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4  ; GP0($28) - Monochrome Quad, Opaque
  WRIOW GP0,(0x28<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
.endmacro
.macro FillQuadV,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($28) - Monochrome Quad, Opaque Variable
  lui t0,0x2800    ; Monochrome Quad, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)
.endmacro

GPUFILLQUADALPHA                      equ 0x2A ; GPU Render Polygon Commands (GP0): $2A - Monochrome Quad, Semi-Transparent
.macro FillQuadAlpha,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($2A) - Monochrome Quad, Semi-Transparent
  WRIOW GP0,(0x2A<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
.endmacro
.macro FillQuadAlphaV,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($2A) - Monochrome Quad, Semi-Transparent Variable
  lui t0,0x2A00    ; Monochrome Quad, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)
.endmacro

GPUTEXQUADBLEND                       equ 0x2C ; GPU Render Polygon Commands (GP0): $2C - Textured Quad, Opaque, Texture-Blending
.macro TexQuadBlend,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2C) - Textured Quad, Opaque, Texture-Blending
  WRIOW GP0,(0x2C<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro TexQuadBlendV,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2C) - Textured Quad, Opaque, Texture-Blending Variable
  lui t0,0x2C00    ; Monochrome Quad, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

GPUTEXQUADRAW                         equ 0x2D ; GPU Render Polygon Commands (GP0): $2D - Textured Quad, Opaque, Raw-Texture
.macro TexQuadRaw,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2D) - Textured Quad, Opaque, Raw-Texture
  WRIOW GP0,(0x2D<<24)                         ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro TexQuadRawV,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2D) - Textured Quad, Opaque, Raw-Texture Variable
  lui t0,0x2D00 ; Textured Quad, Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

GPUTEXQUADBLENDALPHA                  equ 0x2E ; GPU Render Polygon Commands (GP0): $2E - Textured Quad, Semi-Transparent, Texture-Blending
.macro TexQuadBlendAlpha,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2E) - Textured Quad, Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x2E<<24)+(COLOR&0xFFFFFF)        ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro TexQuadBlendAlphaV,COLOR,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2E) - Textured Quad, Semi-Transparent, Texture-Blending Variable
  lui t0,0x2E00    ; Textured Quad, Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

GPUTEXQUADRAWALPHA                    equ 0x2F ; GPU Render Polygon Commands (GP0): $2F - Textured Quad, Semi-Transparent, Raw-Texture
.macro TexQuadRawAlpha,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2F) - Textured Quad, Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x2F<<24)                         ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro TexQuadRawAlphaV,X1,Y1,U1,V1,PAL,X2,Y2,U2,V2,TEX,X3,Y3,U3,V3,X4,Y4,U4,V4 ; GP0($2F) - Textured Quad, Semi-Transparent, Raw-Texture Variable
  lui t0,0x2F00 ; Textured Quad, Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

GPUSHADETRI                           equ 0x30 ; GPU Render Polygon Commands (GP0): $30 - Shaded Triangle, Opaque
.macro ShadeTri,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($30) - Shaded Triangle, Opaque
  WRIOW GP0,(0x30<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
.endmacro
.macro ShadeTriV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($30) - Shaded Triangle, Opaque Variable
  lui t0,0x3000     ; Shaded Triangle, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)
.endmacro

GPUSHADETRIALPHA                      equ 0x32 ; GPU Render Polygon Commands (GP0): $32 - Shaded Triangle, Semi-Transparent
.macro ShadeTriAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($32) - Shaded Triangle, Semi-Transparent
  WRIOW GP0,(0x32<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
.endmacro
.macro ShadeTriAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($32) - Shaded Triangle, Semi-Transparent Variable
  lui t0,0x3200     ; Shaded Triangle, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)
.endmacro

GPUSHADETEXTRI                        equ 0x34 ; GPU Render Polygon Commands (GP0): $34 - Shaded Textured Triangle, Opaque, Texture-Blending
.macro ShadeTexTri,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3 ; GP0($34) - Shaded Textured Triangle, Opaque, Texture-Blending
  WRIOW GP0,(0x34<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro ShadeTexTriV,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3 ; GP0($34) - Shaded Textured Triangle, Opaque, Texture-Blending Variable
  lui t0,0x3400     ; Shaded Textured Triangle, Opaque, Texture-Blending Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUSHADETEXTRIALPHA                   equ 0x36 ; GPU Render Polygon Commands (GP0): $36 - Shaded Textured Triangle, Semi-Transparent, Texture-Blending
.macro ShadeTexTriAlpha,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3 ; GP0($36) - Shaded Textured Triangle, Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x36<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
.endmacro
.macro ShadeTexTriAlphaV,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3 ; GP0($36) - Shaded Textured Triangle, Semi-Transparent, Texture-Blending Variable
  lui t0,0x3600     ; Shaded Textured Triangle, Semi-Transparent, Texture-Blending Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)
.endmacro

GPUSHADEQUAD                          equ 0x38 ; GPU Render Polygon Commands (GP0): $38 - Shaded Quad, Opaque
.macro ShadeQuad,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($38) - Shaded Quad, Opaque
  WRIOW GP0,(0x38<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR4&0xFFFFFF)                  ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
.endmacro
.macro ShadeQuadV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($38) - Shaded Quad, Opaque Variable
  lui t0,0x3800     ; Shaded Quad, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR4,GP0(a0) ; Write GP0 Packet Word (Color4)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)
.endmacro

GPUSHADEQUADALPHA                     equ 0x3A ; GPU Render Polygon Commands (GP0): $3A - Shaded Quad, Semi-Transparent
.macro ShadeQuadAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($3A) - Shaded Quad, Semi-Transparent
  WRIOW GP0,(0x3A<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR4&0xFFFFFF)                  ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
.endmacro
.macro ShadeQuadAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($3A) - Shaded Quad, Semi-Transparent Variable
  lui t0,0x3A00     ; Shaded Quad, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR4,GP0(a0) ; Write GP0 Packet Word (Color4)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)
.endmacro

GPUSHADETEXQUAD                       equ 0x3C ; GPU Render Polygon Commands (GP0): $3C - Shaded Textured Quad, Opaque, Texture-Blending
.macro ShadeTexQuad,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3,COLOR4,X4,Y4,U4,V4 ; GP0($3C) - Shaded Textured Quad, Opaque, Texture-Blending
  WRIOW GP0,(0x3C<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(COLOR4&0xFFFFFF)                  ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro ShadeTexQuadV,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3,COLOR4,X4,Y4,U4,V4 ; GP0($3C) - Shaded Textured Quad, Opaque, Texture-Blending Variable
  lui t0,0x3C00     ; Shaded Textured Quad, Opaque, Texture-Blending Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sw COLOR4,GP0(a0) ; Write GP0 Packet Word (Color4)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

GPUSHADETEXQUADALPHA                  equ 0x3E ; GPU Render Polygon Commands (GP0): $3E - Shaded Textured Quad, Semi-Transparent, Texture-Blending
.macro ShadeTexQuadAlpha,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3,COLOR4,X4,Y4,U4,V4 ; GP0($3E) - Shaded Textured Quad, Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x3E<<24)+(COLOR1&0xFFFFFF)       ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)               ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(PAL<<16)+((V1&0xFF)<<8)+(U1&0xFF) ; Write GP0  Packet Word (Texcoord1+Palette)
  WRIOW GP0,(COLOR2&0xFFFFFF)                  ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)               ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(TEX<<16)+((V2&0xFF)<<8)+(U2&0xFF) ; Write GP0  Packet Word (Texcoord2+Texpage)
  WRIOW GP0,(COLOR3&0xFFFFFF)                  ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)               ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,((V3&0xFF)<<8)+(U3&0xFF)           ; Write GP0  Packet Word (Texcoord3)
  WRIOW GP0,(COLOR4&0xFFFFFF)                  ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)               ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,((V4&0xFF)<<8)+(U4&0xFF)           ; Write GP0  Packet Word (Texcoord4)
.endmacro
.macro ShadeTexQuadAlphaV,COLOR1,X1,Y1,U1,V1,PAL,COLOR2,X2,Y2,U2,V2,TEX,COLOR3,X3,Y3,U3,V3,COLOR4,X4,Y4,U4,V4 ; GP0($3E) - Shaded Textured Quad, Semi-Transparent, Texture-Blending Variable
  lui t0,0x3E00     ; Shaded Textured Quad, Semi-Transparent, Texture-Blending Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll PAL,16     ; PAL <<= 16
  andi U1,0xFF   ; U1 &= 0xFF
  andi V1,0xFF   ; V1 &= 0xFF
  sll V1,8       ; V1 <<= 8
  or U1,V1       ; U1 |= V1
  or U1,PAL      ; U1 |= PAL
  sw U1,GP0(a0)  ; Write GP0 Packet Word (Texcoord1+Palette)

  sw COLOR2,GP0(a0) ; Write GP0 Packet Word (Color2)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll TEX,16     ; TEX <<= 16
  andi U2,0xFF   ; U2 &= 0xFF
  andi V2,0xFF   ; V2 &= 0xFF
  sll V2,8       ; V2 <<= 8
  or U2,V2       ; U2 |= V2
  or U2,TEX      ; U2 |= TEX
  sw U2,GP0(a0)  ; Write GP0 Packet Word (Texcoord2+Texpage)

  sw COLOR3,GP0(a0) ; Write GP0 Packet Word (Color3)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  andi U3,0xFF   ; U3 &= 0xFF
  andi V3,0xFF   ; V3 &= 0xFF
  sll V3,8       ; V3 <<= 8
  or U3,V3       ; U3 |= V3
  sw U3,GP0(a0)  ; Write GP0 Packet Word (Texcoord3)

  sw COLOR4,GP0(a0) ; Write GP0 Packet Word (Color4)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  andi U4,0xFF   ; U4 &= 0xFF
  andi V4,0xFF   ; V4 &= 0xFF
  sll V4,8       ; V4 <<= 8
  or U4,V4       ; U4 |= V4
  sw U4,GP0(a0)  ; Write GP0 Packet Word (Texcoord4)
.endmacro

;================================
; GPU Render Line Commands (GP0)
;================================
GPUFILLLINE                     equ 0x40 ; GPU Render Line Commands (GP0): $40 - Monochrome Line, Opaque
.macro FillLine,COLOR,X1,Y1,X2,Y2        ; GP0($40) - Monochrome Line, Opaque
  WRIOW GP0,(0x40<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
.endmacro
.macro FillLineV,COLOR,X1,Y1,X2,Y2       ; GP0($40) - Monochrome Line, Opaque Variable
  lui t0,0x4000    ; Monochrome Line, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
.endmacro

GPUFILLLINEALPHA                equ 0x42 ; GPU Render Line Commands (GP0): $42 - Monochrome Line, Semi-Transparent
.macro FillLineAlpha,COLOR,X1,Y1,X2,Y2   ; GP0($42) - Monochrome Line, Semi-Transparent
  WRIOW GP0,(0x42<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
.endmacro
.macro FillLineAlphaV,COLOR,X1,Y1,X2,Y2  ; GP0($42) - Monochrome Line, Semi-Transparent Variable
  lui t0,0x4200    ; Monochrome Line, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
.endmacro

GPUFILLPOLYLINE                 equ 0x48 ; GPU Render Line Commands (GP0): $48 - Monochrome Poly-Line, Opaque
.macro FillPolyLine,COLOR,X1,Y1,X2,Y2    ; GP0($48) - Monochrome Poly-Line, Opaque
  WRIOW GP0,(0x48<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
                                         ; Write GP0  Packet Word (VertexN)
                                         ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillPolyLineV,COLOR,X1,Y1,X2,Y2    ; GP0($48) - Monochrome Poly-Line, Opaque Variable
  lui t0,0x4800    ; Monochrome Poly-Line, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
                 ; Write GP0 Packet Word (VertexN)
                 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro FillTriLine,COLOR,X1,Y1,X2,Y2,X3,Y3 ; GP0($48) - Monochrome Triangle-Line, Opaque
  WRIOW GP0,(0x48<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillTriLineV,COLOR,X1,Y1,X2,Y2,X3,Y3 ; GP0($48) - Monochrome Triangle-Line, Opaque Variable
  lui t0,0x4800    ; Monochrome Poly-Line, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  WRIOW GP0,0x55555555 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro FillQuadLine,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($48) - Monochrome Quad-Line, Opaque
  WRIOW GP0,(0x48<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex5)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillQuadLineV,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($48) - Monochrome Quad-Line, Opaque Variable
  lui t0,0x4800    ; Monochrome Poly-Line, Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex5)

  WRIOW GP0,0x55555555 ; Write GP0 Packet Word (Termination Code)
.endmacro

GPUFILLPOLYLINEALPHA            equ 0x4A ; GPU Render Line Commands (GP0): $4A - Monochrome Poly-Line, Semi-Transparent
.macro FillPolyLineAlpha,COLOR,X1,Y1,X2,Y2 ; GP0($4A) - Monochrome Poly-Line, Semi-Transparent
  WRIOW GP0,(0x4A<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
                                         ; Write GP0  Packet Word (VertexN)
                                         ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillPolyLineAlphaV,COLOR,X1,Y1,X2,Y2 ; GP0($4A) - Monochrome Poly-Line, Semi-Transparent Variable
  lui t0,0x4A00    ; Monochrome Poly-Line, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
                 ; Write GP0 Packet Word (VertexN)
                 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro FillTriLineAlpha,COLOR,X1,Y1,X2,Y2,X3,Y3 ; GP0($4A) - Monochrome Triangle-Line, Semi-Transparent
  WRIOW GP0,(0x4A<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillTriLineAlphaV,COLOR,X1,Y1,X2,Y2,X3,Y3 ; GP0($4A) - Monochrome Triangle-Line, Semi-Transparent Variable
  lui t0,0x4A00    ; Monochrome Poly-Line, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  WRIOW GP0,0x55555555 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro FillQuadLineAlpha,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($4A) - Monochrome Quad-Line, Semi-Transparent
  WRIOW GP0,(0x4A<<24)+(COLOR&0xFFFFFF)  ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex5)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillQuadLineAlphaV,COLOR,X1,Y1,X2,Y2,X3,Y3,X4,Y4 ; GP0($4A) - Monochrome Quad-Line, Semi-Transparent Variable
  lui t0,0x4A00    ; Monochrome Poly-Line, Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex5)

  WRIOW GP0,0x55555555 ; Write GP0 Packet Word (Termination Code)
.endmacro

GPUSHADELINE                    equ 0x50 ; GPU Render Line Commands (GP0): $50 - Shaded Line, Opaque
.macro ShadeLine,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($50) - Shaded Line, Opaque
  WRIOW GP0,(0x50<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
.endmacro
.macro ShadeLineV,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($50) - Shaded Line, Opaque Variable
  lui t0,0x5000     ; Shaded Line, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
.endmacro

GPUSHADELINEALPHA               equ 0x52 ; GPU Render Line Commands (GP0): $52 - Shaded Line, Semi-Transparent
.macro ShadeLineAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($52) - Shaded Line, Semi-Transparent
  WRIOW GP0,(0x52<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
.endmacro
.macro ShadeLineAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($52) - Shaded Line, Semi-Transparent Variable
  lui t0,0x5200     ; Shaded Line, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
.endmacro

GPUSHADEPOLYLINE                equ 0x58 ; GPU Render Line Commands (GP0): $58 - Shaded Poly-Line, Opaque
.macro ShadePolyLine,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($58) - Shaded Poly-Line, Opaque
  WRIOW GP0,(0x58<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
                                         ; Write GP0  Packet Word (ColorN)
                                         ; Write GP0  Packet Word (VertexN)
                                         ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadePolyLineV,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($58) - Shaded Poly-Line, Opaque Variable
  lui t0,0x5800     ; Shaded Poly-Line, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
                 ; Write GP0 Packet Word (ColorN)
                 ; Write GP0 Packet Word (VertexN)
                 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro ShadeTriLine,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($58) - Shaded Triangle-Line, Opaque
  WRIOW GP0,(0x58<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)            ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR1&0xFFFFFF)            ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadeTriLineV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($58) - Shaded Triangle-Line, Opaque Variable
  lui t0,0x5800     ; Shaded Poly-Line, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Command Word (Color3+Command)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color4+Command)

  sw X1,GP0(a0) ; Write GP0 Packet Word (Vertex4)

  WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)
.endmacro

.macro ShadeQuadLine,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($58) - Shaded Quad-Line, Opaque
  WRIOW GP0,(0x58<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)            ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR4&0xFFFFFF)            ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,(COLOR1&0xFFFFFF)            ; Write GP0  Packet Word (Color5)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex5)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadeQuadLineV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($58) - Shaded Quad-Line, Opaque Variable
  lui t0,0x5800     ; Shaded Poly-Line, Opaque Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Command Word (Color3+Command)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR4,GP0(a0) ; Write GP0 Command Word (Color4+Command)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color5+Command)

  sw X1,GP0(a0) ; Write GP0 Packet Word (Vertex5)

  WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)
.endmacro

GPUSHADEPOLYLINEALPHA           equ 0x5A ; GPU Render Line Commands (GP0): $5A - Shaded Poly-Line, Semi-Transparent
.macro ShadePolyLineAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($5A) - Shaded Poly-Line, Semi-Transparent
  WRIOW GP0,(0x5A<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
                                         ; Write GP0  Packet Word (ColorN)
                                         ; Write GP0  Packet Word (VertexN)
                                         ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadePolyLineAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2 ; GP0($5A) - Shaded Poly-Line, Semi-Transparent Variable
  lui t0,0x5A00     ; Shaded Poly-Line, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)
                 ; Write GP0 Packet Word (ColorN)
                 ; Write GP0 Packet Word (VertexN)
                 ; Write GP0 Packet Word (Termination Code)
.endmacro

.macro ShadeTriLineAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($5A) - Shaded Triangle-Line, Semi-Transparent
  WRIOW GP0,(0x5A<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)            ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR1&0xFFFFFF)            ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadeTriLineAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3 ; GP0($5A) - Shaded Triangle-Line, Semi-Transparent Variable
  lui t0,0x5A00     ; Shaded Poly-Line, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Command Word (Color3+Command)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color4+Command)

  sw X1,GP0(a0) ; Write GP0 Packet Word (Vertex4)

  WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)
.endmacro

.macro ShadeQuadLineAlpha,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($5A) - Shaded Quad-Line, Semi-Transparent
  WRIOW GP0,(0x5A<<24)+(COLOR1&0xFFFFFF) ; Write GP0 Command Word (Color1+Command)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex1)
  WRIOW GP0,(COLOR2&0xFFFFFF)            ; Write GP0  Packet Word (Color2)
  WRIOW GP0,(Y2<<16)+(X2&0xFFFF)         ; Write GP0  Packet Word (Vertex2)
  WRIOW GP0,(COLOR3&0xFFFFFF)            ; Write GP0  Packet Word (Color3)
  WRIOW GP0,(Y3<<16)+(X3&0xFFFF)         ; Write GP0  Packet Word (Vertex3)
  WRIOW GP0,(COLOR4&0xFFFFFF)            ; Write GP0  Packet Word (Color4)
  WRIOW GP0,(Y4<<16)+(X4&0xFFFF)         ; Write GP0  Packet Word (Vertex4)
  WRIOW GP0,(COLOR1&0xFFFFFF)            ; Write GP0  Packet Word (Color5)
  WRIOW GP0,(Y1<<16)+(X1&0xFFFF)         ; Write GP0  Packet Word (Vertex5)
  WRIOW GP0,0x55555555                   ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro ShadeQuadLineAlphaV,COLOR1,X1,Y1,COLOR2,X2,Y2,COLOR3,X3,Y3,COLOR4,X4,Y4 ; GP0($5A) - Shaded Quad-Line, Semi-Transparent Variable
  lui t0,0x5A00     ; Shaded Poly-Line, Semi-Transparent Command
  or COLOR1,t0      ; COLOR1 |= Command
  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color1+Command)

  sll Y1,16      ; Y1 <<= 16
  andi X1,0xFFFF ; X1 &= 0xFFFF
  or X1,Y1       ; X1 |= Y1
  sw X1,GP0(a0)  ; Write GP0 Packet Word (Vertex1)

  sw COLOR2,GP0(a0) ; Write GP0 Command Word (Color2+Command)

  sll Y2,16      ; Y2 <<= 16
  andi X2,0xFFFF ; X2 &= 0xFFFF
  or X2,Y2       ; X2 |= Y2
  sw X2,GP0(a0)  ; Write GP0 Packet Word (Vertex2)

  sw COLOR3,GP0(a0) ; Write GP0 Command Word (Color3+Command)

  sll Y3,16      ; Y3 <<= 16
  andi X3,0xFFFF ; X3 &= 0xFFFF
  or X3,Y3       ; X3 |= Y3
  sw X3,GP0(a0)  ; Write GP0 Packet Word (Vertex3)

  sw COLOR4,GP0(a0) ; Write GP0 Command Word (Color4+Command)

  sll Y4,16      ; Y4 <<= 16
  andi X4,0xFFFF ; X4 &= 0xFFFF
  or X4,Y4       ; X4 |= Y4
  sw X4,GP0(a0)  ; Write GP0 Packet Word (Vertex4)

  sw COLOR1,GP0(a0) ; Write GP0 Command Word (Color5+Command)

  sw X1,GP0(a0) ; Write GP0 Packet Word (Vertex5)

  WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)
.endmacro

GPUPOLYLINEEND      equ 0x55555555 ; GPU Render Line Commands (GP0): $55555555 - Poly-Line, Termination Code
.macro PolyLineEnd                 ; GP0($55555555) - Poly-Line, Termination Code
  WRIOW GP0,0x55555555             ; Write GP0  Packet Word (Termination Code)
.endmacro
.macro FillPolyLineVert,X,Y        ; GP0(X,Y) - Monochrome Poly-Line, Vertex
  WRIOW GP0,(Y<<16)+(X&0xFFFF)     ; Write GP0  Packet Word (Vertex)
.endmacro
.macro ShadePolyLineVert,COLOR,X,Y ; GP0(COL,X,Y) - Shaded Poly-Line, Vertex
  WRIOW GP0,(COLOR&0xFFFFFF)       ; Write GP0  Packet Word (Color)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)     ; Write GP0  Packet Word (Vertex)
.endmacro

;=====================================
; GPU Render Rectangle Commands (GP0)
;=====================================
GPUFILLRECT                         equ 0x60 ; GPU Render Rectangle Commands (GP0): $60 - Monochrome Rectangle (Variable Size), Opaque
.macro FillRect,COLOR,X,Y,WIDTH,HEIGHT       ; GP0($60) - Monochrome Rectangle (Variable Size), Opaque
  WRIOW GP0,(0x60<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro FillRectV,COLOR,X,Y,WIDTH,HEIGHT      ; GP0($60) - Monochrome Rectangle (Variable Size), Opaque Variable
  lui t0,0x6000    ; Monochrome Rectangle (Variable Size), Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUFILLRECTALPHA                    equ 0x62 ; GPU Render Rectangle Commands (GP0): $62 - Monochrome Rectangle (Variable Size), Semi-Transparent
.macro FillRectAlpha,COLOR,X,Y,WIDTH,HEIGHT  ; GP0($62) - Monochrome Rectangle (Variable Size), Semi-Transparent
  WRIOW GP0,(0x62<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro FillRectAlphaV,COLOR,X,Y,WIDTH,HEIGHT ; GP0($62) - Monochrome Rectangle (Variable Size), Semi-Transparent Variable
  lui t0,0x6200    ; Monochrome Rectangle (Variable Size), Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUTEXRECTBLEND                     equ 0x64 ; GPU Render Rectangle Commands (GP0): $64 - Textured Rectangle (Variable Size), Opaque, Texture-Blending
.macro TexRectBlend,COLOR,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($64) - Textured Rectangle (Variable Size), Opaque, Texture-Blending
  WRIOW GP0,(0x64<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro TexRectBlendV,COLOR,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($64) - Textured Rectangle (Variable Size), Opaque, Texture-Blending Variable
  lui t0,0x6400    ; Textured Rectangle (Variable Size), Opaque, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUTEXRECTRAW                       equ 0x65 ; GPU Render Rectangle Commands (GP0): $65 - Textured Rectangle (Variable Size), Opaque, Raw-Texture
.macro TexRectRaw,X,Y,U,V,PAL,WIDTH,HEIGHT   ; GP0($65) - Textured Rectangle (Variable Size), Opaque, Raw-Texture
  WRIOW GP0,(0x65<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro TexRectRawV,X,Y,U,V,PAL,WIDTH,HEIGHT  ; GP0($65) - Textured Rectangle (Variable Size), Opaque, Raw-Texture Variable
  lui t0,0x6500 ; Textured Rectangle (Variable Size), Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUTEXRECTBLENDALPHA                equ 0x66 ; GPU Render Rectangle Commands (GP0): $66 - Textured Rectangle (Variable Size), Semi-Transparent, Texture-Blending
.macro TexRectBlendAlpha,COLOR,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($66) - Textured Rectangle (Variable Size), Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x66<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro TexRectBlendAlphaV,COLOR,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($66) - Textured Rectangle (Variable Size), Semi-Transparent, Texture-Blending Variable
  lui t0,0x6600    ; Textured Rectangle (Variable Size), Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUTEXRECTRAWALPHA                  equ 0x67 ; GPU Render Rectangle Commands (GP0): $67 - Textured Rectangle (Variable Size), Semi-Transparent, Raw-Texture
.macro TexRectRawAlpha,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($67) - Textured Rectangle (Variable Size), Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x67<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
  WRIOW GP0,(HEIGHT<<16)+(WIDTH&0xFFFF)      ; Write GP0  Packet Word (Width+Height)
.endmacro
.macro TexRectRawAlphaV,X,Y,U,V,PAL,WIDTH,HEIGHT ; GP0($67) - Textured Rectangle (Variable Size), Semi-Transparent, Raw-Texture Variable
  lui t0,0x6700 ; Textured Rectangle (Variable Size), Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)

  sll HEIGHT,16     ; HEIGHT <<= 16
  andi WIDTH,0xFFFF ; WIDTH &= 0xFFFF
  or WIDTH,HEIGHT   ; WIDTH |= HEIGHT
  sw WIDTH,GP0(a0)  ; Write GP0  Packet Word (Width+Height)
.endmacro

GPUFILLRECT1X1                      equ 0x68 ; GPU Render Rectangle Commands (GP0): $68 - Monochrome Rectangle Dot (1x1), Opaque
.macro FillRect1x1,COLOR,X,Y                 ; GP0($68) - Monochrome Rectangle Dot (1x1), Opaque
  WRIOW GP0,(0x68<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRect1x1V,COLOR,X,Y                ; GP0($68) - Monochrome Rectangle Dot (1x1), Opaque Variable
  lui t0,0x6800    ; Monochrome Rectangle Dot (1x1), Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUFILLRECTALPHA1X1                 equ 0x6A ; GPU Render Rectangle Commands (GP0): $6A - Monochrome Rectangle Dot (1x1), Semi-Transparent
.macro FillRectAlpha1x1,COLOR,X,Y            ; GP0($6A) - Monochrome Rectangle Dot (1x1), Semi-Transparent
  WRIOW GP0,(0x6A<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRectAlpha1x1V,COLOR,X,Y           ; GP0($6A) - Monochrome Rectangle Dot (1x1), Semi-Transparent Variable
  lui t0,0x6A00    ; Monochrome Rectangle Dot (1x1), Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUTEXRECTBLEND1X1                  equ 0x6C ; GPU Render Rectangle Commands (GP0): $6C - Textured Rectangle Dot (1x1), Opaque, Texture-Blending
.macro TexRectBlend1x1,COLOR,X,Y,U,V,PAL     ; GP0($6C) - Textured Rectangle Dot (1x1), Opaque, Texture-Blending
  WRIOW GP0,(0x6C<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlend1x1V,COLOR,X,Y,U,V,PAL    ; GP0($6C) - Textured Rectangle Dot (1x1), Opaque, Texture-Blending Variable
  lui t0,0x6C00    ; Textured Rectangle Dot (1x1), Opaque, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAW1X1                    equ 0x6D ; GPU Render Rectangle Commands (GP0): $6D - Textured Rectangle Dot (1x1), Opaque, Raw-Texture
.macro TexRectRaw1x1,X,Y,U,V,PAL             ; GP0($6D) - Textured Rectangle Dot (1x1), Opaque, Raw-Texture
  WRIOW GP0,(0x6D<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRaw1x1V,X,Y,U,V,PAL            ; GP0($6D) - Textured Rectangle Dot (1x1), Opaque, Raw-Texture Variable
  lui t0,0x6D00 ; Textured Rectangle Dot (1x1), Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTBLENDALPHA1X1             equ 0x6E ; GPU Render Rectangle Commands (GP0): $6E - Textured Rectangle Dot (1x1), Semi-Transparent, Texture-Blending
.macro TexRectBlendAlpha1x1,COLOR,X,Y,U,V,PAL ; GP0($6E) - Textured Rectangle Dot (1x1), Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x6E<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlendAlpha1x1V,COLOR,X,Y,U,V,PAL ; GP0($6E) - Textured Rectangle Dot (1x1), Semi-Transparent, Texture-Blending Variable
  lui t0,0x6E00    ; Textured Rectangle Dot (1x1), Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAWALPHA1X1               equ 0x6F ; GPU Render Rectangle Commands (GP0): $6F - Textured Rectangle Dot (1x1), Semi-Transparent, Raw-Texture
.macro TexRectRawAlpha1x1,X,Y,U,V,PAL        ; GP0($6F) - Textured Rectangle Dot (1x1), Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x6F<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRawAlpha1x1V,X,Y,U,V,PAL       ; GP0($6F) - Textured Rectangle Dot (1x1), Semi-Transparent, Raw-Texture Variable
  lui t0,0x6F00 ; Textured Rectangle Dot (1x1), Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUFILLRECT8X8                      equ 0x70 ; GPU Render Rectangle Commands (GP0): $70 - Monochrome Rectangle (8x8), Opaque
.macro FillRect8x8,COLOR,X,Y                 ; GP0($70) - Monochrome Rectangle (8x8), Opaque
  WRIOW GP0,(0x70<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRect8x8V,COLOR,X,Y                ; GP0($70) - Monochrome Rectangle (8x8), Opaque Variable
  lui t0,0x7000    ; Monochrome Rectangle (8x8), Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUFILLRECTALPHA8X8                 equ 0x72 ; GPU Render Rectangle Commands (GP0): $72 - Monochrome Rectangle (8x8), Semi-Transparent
.macro FillRectAlpha8x8,COLOR,X,Y            ; GP0($72) - Monochrome Rectangle (8x8), Semi-Transparent
  WRIOW GP0,(0x72<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRectAlpha8x8V,COLOR,X,Y           ; GP0($72) - Monochrome Rectangle (8x8), Semi-Transparent Variable
  lui t0,0x7200    ; Monochrome Rectangle (8x8), Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUTEXRECTBLEND8X8                  equ 0x74 ; GPU Render Rectangle Commands (GP0): $74 - Textured Rectangle (8x8), Opaque, Texture-Blending
.macro TexRectBlend8x8,COLOR,X,Y,U,V,PAL     ; GP0($74) - Textured Rectangle (8x8), Opaque, Texture-Blending
  WRIOW GP0,(0x74<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlend8x8V,COLOR,X,Y,U,V,PAL    ; GP0($74) - Textured Rectangle (8x8), Opaque, Texture-Blending Variable
  lui t0,0x7400    ; Textured Rectangle (8x8), Opaque, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAW8X8                    equ 0x75 ; GPU Render Rectangle Commands (GP0): $75 - Textured Rectangle (8x8), Opaque, Raw-Texture
.macro TexRectRaw8x8,X,Y,U,V,PAL             ; GP0($75) - Textured Rectangle (8x8), Opaque, Raw-Texture
  WRIOW GP0,(0x75<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRaw8x8V,X,Y,U,V,PAL            ; GP0($75) - Textured Rectangle (8x8), Opaque, Raw-Texture Variable
  lui t0,0x7500 ; Textured Rectangle (8x8), Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTBLENDALPHA8X8             equ 0x76 ; GPU Render Rectangle Commands (GP0): $76 - Textured Rectangle (8x8), Semi-Transparent, Texture-Blending
.macro TexRectBlendAlpha8x8,COLOR,X,Y,U,V,PAL ; GP0($76) - Textured Rectangle (8x8), Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x76<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlendAlpha8x8V,COLOR,X,Y,U,V,PAL ; GP0($76) - Textured Rectangle (8x8), Semi-Transparent, Texture-Blending Variable
  lui t0,0x7600    ; Textured Rectangle (8x8), Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAWALPHA8X8               equ 0x77 ; GPU Render Rectangle Commands (GP0): $77 - Textured Rectangle (8x8), Semi-Transparent, Raw-Texture
.macro TexRectRawAlpha8x8,X,Y,U,V,PAL        ; GP0($77) - Textured Rectangle (8x8), Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x77<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRawAlpha8x8V,X,Y,U,V,PAL       ; GP0($77) - Textured Rectangle (8x8), Semi-Transparent, Raw-Texture Variable
  lui t0,0x7700 ; Textured Rectangle (8x8), Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUFILLRECT16X16                    equ 0x78 ; GPU Render Rectangle Commands (GP0): $78 - Monochrome Rectangle (16x16), Opaque
.macro FillRect16x16,COLOR,X,Y               ; GP0($78) - Monochrome Rectangle (16x16), Opaque
  WRIOW GP0,(0x78<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRect16x16V,COLOR,X,Y              ; GP0($78) - Monochrome Rectangle (16x16), Opaque Variable
  lui t0,0x7800    ; Monochrome Rectangle (16x16), Opaque Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUFILLRECTALPHA16X16               equ 0x7A ; GPU Render Rectangle Commands (GP0): $7A - Monochrome Rectangle (16x16), Semi-Transparent
.macro FillRectAlpha16x16,COLOR,X,Y          ; GP0($7A) - Monochrome Rectangle (16x16), Semi-Transparent
  WRIOW GP0,(0x7A<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
.endmacro
.macro FillRectAlpha16x16V,COLOR,X,Y         ; GP0($7A) - Monochrome Rectangle (16x16), Semi-Transparent Variable
  lui t0,0x7A00    ; Monochrome Rectangle (16x16), Semi-Transparent Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)
.endmacro

GPUTEXRECTBLEND16X16                equ 0x7C ; GPU Render Rectangle Commands (GP0): $7C - Textured Rectangle (16x16), Opaque, Texture-Blending
.macro TexRectBlend16x16,COLOR,X,Y,U,V,PAL   ; GP0($7C) - Textured Rectangle (16x16), Opaque, Texture-Blending
  WRIOW GP0,(0x7C<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlend16x16V,COLOR,X,Y,U,V,PAL  ; GP0($7C) - Textured Rectangle (16x16), Opaque, Texture-Blending Variable
  lui t0,0x7C00    ; Textured Rectangle (16x16), Opaque, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAW16X16                  equ 0x7D ; GPU Render Rectangle Commands (GP0): $7D - Textured Rectangle (16x16), Opaque, Raw-Texture
.macro TexRectRaw16x16,X,Y,U,V,PAL           ; GP0($7D) - Textured Rectangle (16x16), Opaque, Raw-Texture
  WRIOW GP0,(0x7D<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRaw16x16V,X,Y,U,V,PAL          ; GP0($7D) - Textured Rectangle (16x16), Opaque, Raw-Texture Variable
  lui t0,0x7D00 ; Textured Rectangle (16x16), Opaque, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTBLENDALPHA16X16           equ 0x7E ; GPU Render Rectangle Commands (GP0): $7E - Textured Rectangle (16x16), Semi-Transparent, Texture-Blending
.macro TexRectBlendAlpha16x16,COLOR,X,Y,U,V,PAL ; GP0($7E) - Textured Rectangle (16x16), Semi-Transparent, Texture-Blending
  WRIOW GP0,(0x7E<<24)+(COLOR&0xFFFFFF)      ; Write GP0 Command Word (Color+Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectBlendAlpha16x16V,COLOR,X,Y,U,V,PAL ; GP0($7E) - Textured Rectangle (16x16), Semi-Transparent, Texture-Blending Variable
  lui t0,0x7E00    ; Textured Rectangle (16x16), Semi-Transparent, Texture-Blending Command
  or COLOR,t0      ; COLOR |= Command
  sw COLOR,GP0(a0) ; Write GP0 Command Word (Color+Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

GPUTEXRECTRAWALPHA16X16             equ 0x7F ; GPU Render Rectangle Commands (GP0): $7F - Textured Rectangle (16x16), Semi-Transparent, Raw-Texture
.macro TexRectRawAlpha16x16,X,Y,U,V,PAL      ; GP0($7F) - Textured Rectangle (16x16), Semi-Transparent, Raw-Texture
  WRIOW GP0,(0x7F<<24)                       ; Write GP0 Command Word (Command)
  WRIOW GP0,(Y<<16)+(X&0xFFFF)               ; Write GP0  Packet Word (Vertex)
  WRIOW GP0,(PAL<<16)+((V&0xFF)<<8)+(U&0xFF) ; Write GP0  Packet Word (Texcoord+Palette)
.endmacro
.macro TexRectRawAlpha16x16V,X,Y,U,V,PAL     ; GP0($7F) - Textured Rectangle (16x16), Semi-Transparent, Raw-Texture Variable
  lui t0,0x7F00 ; Textured Rectangle (16x16), Semi-Transparent, Raw-Texture Command
  sw t0,GP0(a0) ; Write GP0 Command Word (Command)

  sll Y,16      ; Y <<= 16
  andi X,0xFFFF ; X &= 0xFFFF
  or X,Y        ; X |= Y
  sw X,GP0(a0)  ; Write GP0  Packet Word (Vertex)

  sll PAL,16   ; PAL <<= 16
  andi U,0xFF  ; U &= 0xFF
  andi V,0xFF  ; V &= 0xFF
  sll V,8      ; V <<= 8
  or U,V       ; U |= V
  or U,PAL     ; U |= PAL
  sw U,GP0(a0) ; Write GP0 Packet Word (Texcoord+Palette)
.endmacro

;================================
; GPU Rendering Attributes (GP0)
;================================
GPUDRAWM   equ 0xE1 ; GPU Rendering Attributes (GP0): $E1 - Draw Mode Setting (AKA "Texpage")
GPUTEXWIN  equ 0xE2 ; GPU Rendering Attributes (GP0): $E2 - Texture Window Setting
GPUDRAWATL equ 0xE3 ; GPU Rendering Attributes (GP0): $E3 - Set Drawing Area Top Left (X1,Y1)
GPUDRAWABR equ 0xE4 ; GPU Rendering Attributes (GP0): $E4 - Set Drawing Area Bottom Right (X2,Y2)
GPUDRAWOFS equ 0xE5 ; GPU Rendering Attributes (GP0): $E5 - Set Drawing Offset (X,Y)
GPUMASKBIT equ 0xE6 ; GPU Rendering Attributes (GP0): $E6 - Mask Bit Setting

;==================================================================================
; GPU Display Control Commands (GP1) (Command = 8-Bit MSB, Parameter = 24-Bit LSB)
;==================================================================================
GPURESET   equ 0x00 ; GPU Display Control Commands (GP1): $00 - Reset GPU
GPURESETCB equ 0x01 ; GPU Display Control Commands (GP1): $01 - Reset Command Buffer
GPUIRQACK  equ 0x02 ; GPU Display Control Commands (GP1): $02 - Acknowledge GPU Interrupt (IRQ1)
GPUDISPEN  equ 0x03 ; GPU Display Control Commands (GP1): $03 - Display Enable
GPUDMA     equ 0x04 ; GPU Display Control Commands (GP1): $04 - DMA Direction / Data Request
GPUVRAM    equ 0x05 ; GPU Display Control Commands (GP1): $05 - Start Of Display Area (In VRAM)
GPUDISPH   equ 0x06 ; GPU Display Control Commands (GP1): $06 - Horizontal Display Range (On Screen)
GPUDISPV   equ 0x07 ; GPU Display Control Commands (GP1): $07 -   Vertical Display Range (On Screen)

GPUDISPM   equ 0x08 ; GPU Display Control Commands (GP1): $08 - Display Mode
   HRES256 equ 0x00 ; Display Mode: Horizontal Resolution 1 256 (Bit 0..1)
   HRES320 equ 0x01 ; Display Mode: Horizontal Resolution 1 320 (Bit 0..1)
   HRES512 equ 0x02 ; Display Mode: Horizontal Resolution 1 512 (Bit 0..1)
   HRES640 equ 0x03 ; Display Mode: Horizontal Resolution 1 640 (Bit 0..1)
   VRES240 equ 0x00 ; Display Mode: Vertical Resolution 240 (Bit 2)
   VRES480 equ 0x04 ; Display Mode: Vertical Resolution 480 (When Vertical Interlace Bit 5 = 1) (Bit 2)
     VNTSC equ 0x00 ; Display Mode: Video Mode NTSC 60Hz (Bit 3)
      VPAL equ 0x08 ; Display Mode: Video Mode  PAL 50Hz (Bit 3)
     BPP15 equ 0x00 ; Display Mode: Display Area Color Depth 15BPP R5/G5/B5 (Bit 4)
     BPP24 equ 0x10 ; Display Mode: Display Area Color Depth 24BPP R8/G8/B8 (Bit 4)
 INTERLACE equ 0x20 ; Display Mode: Vertical Interlace (Bit 5)
   HRES360 equ 0x40 ; Display Mode: Horizontal Resolution 2 360 (Bit 6)
   DISTORT equ 0x80 ; Display Mode: Reverse Flag (Bit 7)

GPUTEXEN   equ 0x09 ; GPU Display Control Commands (GP1): $09 - Texture Disable (Newer GPU)
                    ; GPU Display Control Commands (GP1): $0A..$0F - N/A
GPUINFO    equ 0x10 ; GPU Display Control Commands (GP1): $10 - Get GPU Info
                    ; GPU Display Control Commands (GP1): $11..$1F - Get GPU Info (Mirror Of $10)
GPUTEXENA  equ 0x20 ; GPU Display Control Commands (GP1): $20 - Texture Disable Ancient (Older GPU)
                    ; GPU Display Control Commands (GP1): $21..$3F - N/A
                    ; GPU Display Control Commands (GP1): $40..$FF - N/A (Mirrors Of $00..$3F)

;============
; GPU Macros
;============
.macro WRGP0,COMMAND,PARAMETER                 ; Write GP0 Command/Packet (Rendering & VRAM Access)
  WRIOW GP0,(COMMAND<<24)+(PARAMETER&0xFFFFFF) ; Write GP0 Command/Packet Word
.endmacro

.macro WRGP1,COMMAND,PARAMETER      ; Write GP1 Command (Display Control)
  WRIOW GP1,(COMMAND<<24)+(PARAMETER&0xFFFFFF) ; Write GP1 Command Word
.endmacro