;-------------------------------------------------------------
; GPU routines.
;-------------------------------------------------------------
GP0     equ 0x1810 ; some equ's for easy ref.
GP1     equ 0x1814
DPCR    equ 0x10F0
DICR    equ 0x10F4
D2_MADR equ 0x10A0
D2_BCR  equ 0x10A4
D2_CHCR equ 0x10A8
;-------------------------------------------------------------
; InitGPU - basic GPU init routine
; in:  a0 - display mode
;-------------------------------------------------------------
InitGPU:
  lui fp,0x1F80
  or t0,zero,ra

  sw zero,GP1(fp)  ; reset (command 0)


  li t1,0x05000000 ; display offset
  jal WaitGPU      ;
  nop              ;
  sw t1,GP1(fp)    ;

  li t1,0x06CA0220 ; horizontal start/end command 6
  sw t1,GP1(fp)    ; $|06|ca0|220 = 336 pixels
                   ;      x2  x1

  li t1,0x0704B81E ; vertical start/end command $07
  sw t1,GP1(fp)    ; Y start =$1e, Y end =$12e=272 pixels

  sw a0,GP1(fp)    ; set display mode
  nop              ;

  li t1,0xE10006CF ; draw mode
  sw t1,GP0(fp)    ; command $e1  set draw mode
                   ; data: bit $17-$0b = 0
                   ; bit $0a-$00: %1|1|01|10|0|0101
                   ;               a|b|c |d |e|f
                   ; A: DFE=$1 (Draw to display area allowed)
                   ; B: DTD=$1 (Dither on)
                   ; C: TP =$1 (8bit clut mode)
                   ; D: ABR=$0 (half transparancy 0.5F+0.5B)
                   ; E: TY =$0 (Texture page at Y=0*256=  0)
                   ; F: TX =$f (Texture page at X=15*64=960)


  li t1,0xE3000000 ; clip start command $e3
  sw t1,GP0(fp)    ; bit $0a-$00= X, bit $14-$0b = Y
                   ; sets top left corner of drawing area


  li t1,0xE407429F ; clip end command $e4
  sw t1,GP0(fp)    ; set bottom right of drawing area
                   ; X = bit $09-$00 =$014f  $29f
                   ; Y = bit $13-$0a =$0110  $110
        
  li t1,0xE5000000 ; draw offset
  sw t1,GP0(fp)    ;

  li t1,0x03000000 ; enable display
  sw t1,GP1(fp)    ;

  or ra,zero,t0
  jr ra
  nop

;-------------------------------------------------------------
; SendList - sends a list of primitives to GPU
; in:   a0 - address of list
;-------------------------------------------------------------
SendList:
  lui fp,0x1F80
  or t0,zero,ra
  li t2,0x04000002
  jal WaitGPU
  nop

  lw t3,DPCR(fp)
  sw zero,DICR(fp)
  ori t3,t3,0x0800
  sw t3,DPCR(fp)

  sw t2,GP1(fp)
  sw a0,D2_MADR(fp)
  sw zero,D2_BCR(fp)
  li t1,0x01000401
  sw t1,D2_CHCR(fp)

  or ra,zero,t0
  jr ra
  nop

;-------------------------------------------------------------
; MemtoVRAM  - transfer graphic data to VRAM.
; in:    a0 - source address
;        a1 - x,y    (y << 16 | x)
;        a2 - w,h    (h << 16 | w)
;        a3 - amount to copy (in words)
;-------------------------------------------------------------
MemtoVRAM:
  or t4,zero,ra         ; save RA

  jal WaitDone          ; wait for idle + DMA finish.
  nop                   ;

  or t5,zero,a0         ; save start addy.
  lui at,mvl1>>16       ;
  sw a1,mvl1&0xFFFF(at) ; set XY
  sw a2,mvl2&0xFFFF(at) ; set HW
  lui a0,mvl>>16        ; send transfer setup.
  jal SendList          ;
  ori a0,a0,mvl&0xFFFF  ;

  sll t1,a3,0x10        ; set number of dma blocks.
  ori t1,t1,0x0001      ; set block size to 1 word.
  li t2,0x01000201      ; dma control: continuous, mem->vram

  jal WaitDMA           ; Wait for setup to complete
  nop                   ;

  sw t5,D2_MADR(fp)     ; set base address
  sw t1,D2_BCR(fp)      ; set block control
  sw t2,D2_CHCR(fp)     ; start dma

  or ra,zero,t4         ; return.
  jr ra                 ;
  nop                   ;

mvl:
  dw 0x04FFFFFF
  dw 0x01000000
  dw 0xA0000000
mvl1:
  dw 0x00000000
mvl2:
  dw 0x00000000

;-------------------------------------------------------------
; WaitGPU - waits until GPU ready to recieve commands
;-------------------------------------------------------------
WaitGPU:
  lui fp,0x1F80

  lw v1,GP1(fp)   ; load status word from GPU
  lui v0,0x1000   ; load bit $1c
  and v1,v1,v0    ; and out all but bit $1c
  beqz v1,WaitGPU ; bit $1c = 0 -> GPU is busy
  nop             ;
  jr ra
  nop
;-------------------------------------------------------------
; WaitIdle - waits until GPU is idle
;-------------------------------------------------------------
WaitIdle:
  lui fp,0x1F80

  lui v1,0x0400
widl:
  lw v0,GP1(fp)
  nop
  and v0, v0, v1
  beqz v0, widl
  nop
  jr ra
  nop
;-------------------------------------------------------------
; WaitDone - waits for DMA & GPU to be idle.
;-------------------------------------------------------------
WaitDone:
  lui fp,0x1F80

wd1:
  lw v0,D2_CHCR(fp)
  lui v1,0x0100
  and v0,v0,v1
  bne v0,zero,wd1
  nop

wd2:
  lw v0,GP1(fp)
  lui v1,0x0400
  and v0,v0,v1
  beq v0,zero,WaitDone
  nop

  jr ra
  nop
;-------------------------------------------------------------
; WaitDMA - waits for DMA idle.
;-------------------------------------------------------------
WaitDMA:
  lui fp,0x1F80

wdmalp:
  lw v0,D2_CHCR(fp)
  lui v1,0x0100
  and v0,v0,v1
  bne v0,zero,wdmalp
  nop

  jr ra
  nop