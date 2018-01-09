.psx
.create "PRINTGPU.bin", 0x80010000

;-------------------------------------------------------------
;GPU print example.                          1999 doomed/padua
;-syntax of spasm.
;-uses the caetla 8*16 font, but can be any 8*16 font (FONT.BIN)
;-prints fixed width. If you want proportional, just add the char
; width after a plot to X, instead of 8.
;
;PrintGPU_Dma routine makes a linked list. Useful in stuff
;             that plot lots off primitive.
;PrintGPU     routine waits for gpu ready and plots
;             without DMA. More useful for debugging.. less
;             speedy, but does not require a variable amount
;             of memory for the list.
;-------------------------------------------------------------
;Quick explanation:
;Simply converts character codes to UV coordinates. So if you
;want ASCII, put the characters on the texture page in ascii
;order. (32 chars of 8pixels wide besides eachother, then fill
;up the rows)
;
;PrintGPU_dma quits on a control codes (<$20) as first character
;-------------------------------------------------------------

.org 0x80010000

; Init stuff.
lui a0,0x0800    ; initialise the GPU
jal InitGPU      ; command 8
ori a0,a0,0x0009 ; bit $00,$01= %01 -> screen width:320
                 ; bit $03 = 1 -> video mode = pal
la a0,back       ; draw a nice backdrop
jal SendList     ;
nop              ;
jal Loadfont     ; upload font data.
nop              ;


; Plot with DMA.
la a0,text1      ; a0 = pointer to string.
li a1,0x00200018 ; a1 : y<<16|x (y=$20,x=$20)
li a2,0x00808080 ; a2 : bgr = 808080 = white.
jal PrintGPU_dma ; print
nop              ;
jal SendList     ; plot text.
or a0,zero,v0    ;

; Plot without DMA
la a0,text2      ; a0 = pointer to string.
li a1,0x00600018 ; a1 : y<<16|x (y=$20,x=$20)
li a2,0x00208080 ; a2 : bgr = 808080 = white.
jal PrintGPU     ; print
nop              ;

self:
  j self
  nop


text1:
  db "Here",0x27,"s yar stuff printed.",0x0A,0x0D,"Next line"
  db 0x0A,"Just a linefeed..",0x00
text2:
  db "And here",0x27,"s some more text in yellow",0x2C
  db 0x0D,0x0A,"printed without DMA."

.align 4
dw 0

;-------------------------------------------------------------
.include "SYS2.ASM" ; GPU routines
;-------------------------------------------------------------
txtDMA  equ 0x0000  ; primitive offsets
txtRGB  equ 0x0004
txtX    equ 0x0008
txtY    equ 0x000A
txtU    equ 0x000C
txtV    equ 0x000D
txtClut equ 0x000E

;-------------------------------------------------------------
; PrintGPU_dma
; in:  a0 : pointer to string (0 terminated)
;      a1 : hiword Y, loword X
;      a2 : color
; out: v0 : pointer to list
;-------------------------------------------------------------
PrintGPU_dma:
  sw a1,cXY            ; save XY
  lui v0,0x7400        ; type $70 = 8*8 sprite
  or t2,a2,v0          ; t2 = type + color
  li t3,511<<6         ; t3 = clut (0,511)

  la t0,list2          ; t0 = pointer to list entry 2, first is
                       ; setting of texture page.

  lui v0,0x00FF        ; t1 = DMA link to next entry
  ori v0,v0,0xFFFF
  and t1,t0,v0        
  lui v0,0x0300
  or t1,t1,v0
  addiu t1,t1,0x0010

  lh t5,cY             ; t5 = Y1
  lh t4,cX             ; t4 = X
  addiu t6,t5,0x08     ; t6 = Y2


  lbu v0,0x0000(a0)    ; get character
  nop
  slti v1,v0,0x0020
  beq v1,zero,pgpuloop ; first char command -> quit.
  nop

  jr ra                ; quit, return -1.
  addiu v0,zero,-1     ;

pgpuloop:
                    ; find texture coords

  andi v1,v0,0x001F ; get loworder 5 bits
  sll v1,v1,3       ; * 8 = u

  srl v0,v0,5       ; get highorder 3 bits
  sll v0,v0,4       ; * 16 = v


  sw t1,txtDMA(t0)  ; set DMA pointer       top half
  sw t2,txtRGB(t0)  ; set type + RGB

  sh t4,txtX(t0)    ; set xy
  sh t5,txtY(t0)    ;

  sb v1,txtU(t0)    ; set uv
  sb v0,txtV(t0)    ;

  sh t3,txtClut(t0) ; set clut

  addiu t0,t0,0x10  ; next list entry
  addiu t1,t1,0x10  ;
  addiu v0,v0,0x08  ; y=y+8

  sw t1,txtDMA(t0)  ; set DMA pointer      bottom half
  sw t2,txtRGB(t0)  ; set type + RGB

  sh t4,txtX(t0)    ; set xy
  sh t6,txtY(t0)    ;

  sb v1,txtU(t0)    ; set uv
  sb v0,txtV(t0)    ;

  addiu t4,t4,0x08  ; x = x + 8  <- for proportional
                    ;               feed the char width here.

  addiu a0,a0,0x01  ; next character
  lbu v0,0x0000(a0)

  sh t3,txtClut(t0) ; set clut


pgpuchk:
  slti v1,v0,0x0020   ; command char?
  bgtz v1,pgpucommand ; go to handler.
  nop

  addiu t0,t0,0x10    ; update pointers
  j pgpuloop          ;
  addiu t1,t1,0x10    ;

pgpudone:
  li t1,0x03FFFFFF ; end of list
  sw t1,txtDMA(t0) ;

  la v0,list       ; return address of list.
  jr ra            ;
  nop              ;

pgpucommand:
  beq v0,zero,pgpudone ; done yet?
  ori v1,zero,0x000A   ; linefeed = $0a
  beq v0,v1,pgpuLF     ;
  ori v1,zero,0x000D   ; carriage return = $0d
  beq v0,v1,pgpuCR
  nop
pgpunxt:
  addiu a0,a0,0x0001 ; not a valid command?
  lbu v0,0x0000(a0)  ;
  j pgpuchk          ; skip..
  nop


pgpuLF:
  addiu t6,t6,0x0010 ; handle linefeed.
  j pgpunxt ; do next.
  addiu t5,t5,0x0010

pgpuCR:
  lw t4,cX  ; handle carriage return.
  j pgpunxt ; do next.
  nop

cXY:
cX:
  dh 0
cY:
  dh 0
;-------------------------------------------------------------

;-------------------------------------------------------------
; PrintGPU - plots text to the screen without using DMA.
; in:  a0 : pointer to string (0 terminated)
;      a1 : hi word=Y, lo word=X
;      a2 : color
;-------------------------------------------------------------
PrintGPU:
  or t0,zero,ra    ; save RA

  li t1,0xE100060F ; draw mode setting.
  li t2,0x04000000 ; Dma transfer off command

  jal WaitDone      ; also sets fp to $1f800000
  nop               ;
  sw t2,GP1(fp)     ; DMA mode = 0
  nop               ;
  sw t1,GP0(fp)     ; Set texture page


  sw a1,c2XY        ; save XY
  lui v0,0x7400     ; type $70 = 8*8 sprite

  or t1,a2,v0       ; t1 = type + color
  lui t2,511<<6     ; t2 = clut (0,511)

  lh t4,c2Y         ; t5 = Y1
  lh t3,c2X         ; t4 = X
  addiu t5,t4,0x08  ; t6 = Y2


  lbu v0,0x0000(a0) ; get character
  nop               ;
  slti v1,v0,0x0020 ; first char command -> handle.
  bne v1,zero,p2gpucommand 
  nop

p2gpuloop:
                    ; find texture coords

  andi v1,v0,0x001F ; get loworder 5 bits
  sll v1,v1,3       ; * 8 = u

  srl v0,v0,5       ; get highorder 3 bits
  sll v0,v0,4       ; * 16 = v

  sll t9,v0,0x8     ; V     Top part
  or t9,t9,v1       ; U
  or t9,t9,t2       ; Clut

  addiu v0,v0,0x08  ;

  sll t6,v0,0x8     ; V     Bottom part
  or t6,t6,v1       ; U
  or t6,t6,t2       ; Clut


                    ; Plot top half
  sll t8,t4,0x10    ; Y
  or t8,t8,t3       ; X
  jal WaitGPU       ;
  nop               ;
  sw t1,GP0(fp)     ; set type RGB
  sw t8,GP0(fp)     ;     Y    X
  sw t9,GP0(fp)     ;     clut U V

                    ; Bottom half
  sll t8,t5,0x10    ; Y
  or t8,t8,t3       ; X
  jal WaitGPU       ;
  nop               ;
  sw t1,GP0(fp)     ; send type|BGR
  sw t8,GP0(fp)     ; send Y   |X
  sw t6,GP0(fp)     ; send clut|U|V
  addiu t3,t3,0x08  ; x = x + 8   <- for proportional
                    ;                feed char width here.

p2gpunxt:
  addiu a0,a0,0x01     ; next character
  lbu v0,0x0000(a0)    ; fetch
  nop                  ;

  slti v1,v0,0x0020    ; command char?->handle
  bgtz v1,p2gpucommand ;
  nop                  ;

  j p2gpuloop          ; otherwise plot next char.
  nop                  ;

p2gpudone:
  or ra,zero,t0 ; return if done.
  jr ra         ;
  nop           ;

p2gpucommand:
  beq v0,zero,p2gpudone ; done yet?
  ori v1,zero,0x000A    ;
  beq v0,v1,p2gpuLF     ; handle linefeed.
  ori v1,zero,0x000D    ;
  beq v0,v1,p2gpuCR     ; handle carriage return.
  nop                   ; discard unknown codes.
  j p2gpunxt            ;


p2gpuLF:
  addiu t4,t4,0x0010 ; linefeed.
  j p2gpunxt         ; Y=Y+$10
  addiu t5,t5,0x0010 ;

p2gpuCR:
  lw t3,c2X  ; carriage return.
  j p2gpunxt ; X = start X.
  nop        ;

c2XY:  ; Temp XY storage.
c2X:
  dh 0 ;
c2Y:
  dh 0 ;

;-------------------------------------------------------------
; Upload the font into vram.
Loadfont:
  or s0,zero,ra

  la a0,img        ; source address font
  li a1,0x000003C0 ; target topleft Y|X
  li a2,0x00400040 ; target H|W
  li a3,0x00000800 ; number of words to send
  jal MemtoVRAM
  nop
  la a0,clut       ; source address clut
  li a1,0x01FF0000 ; target topleft Y|X
  li a2,0x00010010 ; target H|W
  li a3,0x00000008 ; number of words to send
  jal MemtoVRAM
  nop

  or ra,zero,s0
  jr ra
  nop

;-------------------------------------------------------------
;The list area..
back:
  dw back2,back2>>8,back2>>16,0x03
  dw 0x02000000
  dw 0x000003C0
  dw 0x010000FE


back2:
  dw 0x08FFFFFF ; Primitive of gouroud 4 point polygon
  dw 0x38300040 ; Type + bgr    top left
  dw 0x00000000 ; XY
  dw 0x00250010 ; bgr           top right
  dw 0x00000150 ; XY
  dw 0x00250010 ; bgr           bottom left
  dw 0x01100000 ; XY
  dw 0x00800000 ; bgr           bottom right
  dw 0x01100150 ; XY


list:
  db list2,list2>>8,list2>>16,0x01 ;
  dw 0xE100060F                    ; dtd, dfe = 1
                                   ; tpage xy = 960,0
                                   ; 4 bit clut

list2:
  dw 0                             ; Here go the primitives
                                   ; generated by the routine.
;-------------------------------------------------------------
;Temporary data. Clut + Font image data.
;-------------------------------------------------------------
clut:
  dh 0x0000,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF
  dh 0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF

img:
  .incbin "FONT.BIN" ; this is the font.
;-------------------------------------------------------------

.close