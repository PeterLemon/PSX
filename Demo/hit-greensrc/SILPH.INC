; A bunch of handy PSX routines
; Silpheed/HITMEN September 1998

; NOTE: register k1 is expected to contain $1f800000 (hardware base)

; Define some hardware registers:

GP0 equ 0x1810
GP1 equ 0x1814
IREG equ 0x1070
IMASK equ 0x1074
DPCR equ 0x10F0
DICR equ 0x10F4
D2_MADR equ 0x10A0
D2_BCR equ 0x10A4
D2_CHCR equ 0x10A8

; SendList - sends a list of primitives to GPU
; in:   a0 - address of list

SendList:
  move t9,ra
  li t2,0x04000002
  jal WaitGPU
  nop

  lw t3,DPCR(k1)
  sw zero,DICR(k1)
  ori t3,0x800
  sw t3,DPCR(k1)

  sw t2,GP1(k1)
  sw a0,D2_MADR(k1)
  sw zero,D2_BCR(k1)
  li t1,0x01000401
  sw t1,D2_CHCR(k1)

  move ra,t9
  jr ra
  nop

; WaitGPU - waits until GPU ready to receive commands

WaitGPU:
  lw t1,GP1(k1)
  li t0,0x10000000
  and t1,t1,t0
  beqz t1,WaitGPU
  nop
  jr ra
  nop


; MEM2VRAM_IO - transfer graphic data to VRAM (IO mode)
; in: a0 - source address
;     a1 - x,y (y << 16 | x)     
;     a2 - w,h (h << 16 | w)
;     a3 - amount to copy (in words)
            
MEM2VRAM_IO:
  move t3,ra
  jal WaitIdle
  li t2,0x04000000
  sw t2,GP1(k1)
  li t0,0x01000000
  li t1,0xA0000000
  sw t0,GP1(k1)
  sw t1,GP0(k1)
  sw a1,GP0(k1)
  sw a2,GP0(k1)

  move t0,a3                    
  move t2,a0                    

M2VIO_loop:
  lw t1,(t2)
  subiu t0,t0,1
  sw t1,GP0(k1)
  addiu t2,t2,4
  bne zero,t0,M2VIO_loop
  nop

  move ra, t3
  jr ra
  nop


; WaitIdle - waits until GPU is idle            

WaitIdle:
  li t1,0x04000000
  lw t0,GP1(k1)
  nop
  and t0,t0,t1
  beqz t0,WaitIdle
  nop
  jr ra
  nop


; InitGPU - basic GPU init routine
; in:  a0 - display mode

InitGPU:
  move t3,ra
  sw zero,GP1(k1) ; reset

  li t2,0x03000001
  sw t2,GP1(k1) ; disable display

  li t2,0x06C40240 ; horizontal start/end
  sw t2,GP1(k1)
  nop

  li t2,0x07049025 ; vertical start/end
  sw t2,GP1(k1)
  nop

  li t2,0xE1000685 ; draw mode, texture page = (8bit,320,0)
  jal WaitGPU
  nop
  sw t2,GP0(k1)
  nop

  jal WaitGPU
  li t2,0xE3000000 ; clip start
  sw t2, GP0(k1)
  nop

  li t2,0xE407FD3F ; clip end 
  jal WaitGPU
  nop
  sw t2, GP0(k1)

  li t2,0x05000000 ; display offset
  jal WaitGPU
  nop
  sw t2,GP0(k1)

  li t2,0xE5000000 ; draw offset
  jal WaitGPU
  nop
  sw t2,GP0(k1)

  sw a0,GP1(k1) ; set display mode
  nop

  li t2,0x03000000
  sw t2,GP1(k1) ; enable display

  move ra,t3
  jr ra
  nop


; InitPads - initialise joypads, also needed for vsync wait routine
; trashes: fp

InitPads:
  move fp,ra
  li t1,0x15
  li a0,0x20000001
  li t2,0xB0
  la a1,pad_buf
  jalr t2
  nop
  move ra,fp
  jr ra
  nop


; WaitVSync - waits for vertical retrace period, relies on InitPads above
;           - also copies xor'ed pad data to pad_data variable

WaitVSync:
  lw t0,pad_buf
  lui t1,0xFFFF
  beqz t0,WaitVSync
  ori t1,0xFFFF
  sw zero,pad_buf
  xor t0,t1 ; reverse bits
  sw t0,pad_data
  jr ra
  nop

pad_buf:
  dw 0 ; pad data is automatically stored here every frame
pad_data:
  dw 0 ; pad data copied here (read it from here if using the WaitVSync routine)