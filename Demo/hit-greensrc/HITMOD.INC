; MOD player by Silpheed of Hitmen (plays .HIT files)
; This is VERY basic - only 4 channels, no effects, no finetunes...
; Maybe a few bugs in it, but I haven't really noticed any

; HM_Init - Initialise SPU, read module info etc.
; a0 - address of module data (must be word-aligned)

HM_Init:
  sw a0,HM_ModStart
  move t9,ra
  li k1,1F800000h ; assumes this will stay the same later on

  move t0,zero
  li t1,0x1010
  addiu t2,a0,20

HM_GetSmpInfo:
  lh t3,22(t2)
  nop
  andi t4,t3,0xFF
  sll t4,8
  srl t3,8
  andi t3,t3,0xFF
  or t3,t4
            
  la t4,HM_SmpAddress
  sll t5,t0,2
  addu t4,t5
  sw t1, (t4)

  addu t1,t3

  lb t4,24(t2)
  lb t5,25(t2)
  la t6,HM_FineTunes
  la t7,HM_Volumes
  addu t6,t0
  addu t7,t0
  sb t4,(t6)
  sb t5,(t7)

  addiu t2,30            

  slti t3,t0,30
  bnez t3,HM_GetSmpInfo
  addiu t0,1

  sw t1,HM_SampleEnd        

  lw t0,0x10F0(k1)
  li at,0xB0000
  or t0,at
  sw t0,0x10F0(k1)

  li t1,0x3000
  sh t1,0x1D80(k1)
  sh t1,0x1D82(k1)
  sh zero,0x1DAA(k1)

  jal HM_WaitSPU

  li t2,4
  sh t2,0x1DAC(k1)

  jal HM_WaitSPU
  nop

  li t0,0x8000
  sh t0,0x1DAA(k1)
  nop                    

  li t0,0x1010>>3
  sh t0,0x1DA6(k1)

  lh t1,0x1DAA(k1)
  nop
  andi t1,t1,0xFFCF
  ori t1,t1,0x20
  sh t1,0x1DAA(k1)

  lb t0,950(a0)
  li t1,952
  sb t0,HM_SongLength
  addu t1,t1,a0
  lb t2,(t1)
  move t0,zero
  sb t2,HM_CurrPattern
  move t4,zero
  sll t2,10
  addu t2,a0
  addiu t2,1084
  sw t2,HM_RowPointer
  nop

HM_ReadOrders:
  lb t2,(t1)
  nop
  slt t3,t2,t4
  bnez t3,HM_skip1
  nop

  move t4,t2            
            
HM_skip1:
  addiu t1,1
  slti t2,t0,127
  bnez t2,HM_ReadOrders
  addiu t0,1

  sb t4,HM_NumPatterns
  nop

  addiu t4,1
  sll t4,10
  sw t4,HM_PattDataSize
                       
HM_wait2:
  lh t0,0x1DAA(k1)
  li t1,0x20
  andi t0,t0,0x30
  bne t0,t1,HM_wait2
  nop                    

  lw t0,0x1014(k1) ; transfer samples
  nop
  li t1,0xF0FFFFFF
  and t0,t1
  li t1,0x20000000
  or t0,t1
  sw t0,0x1014(k1)

  lw t1,HM_PattDataSize
  addiu t0,a0,1084
  addu t0,t1

  sw t0,0x10C0(k1)

  lw t1,HM_SampleEnd
  li t0,0x1010
  subu t0,t1,t0
  srl t0,6
  addiu t0,1
  sll t0,16
  ori t0,0x10
  sw t0,0x10C4(k1)
  nop

  li t0,0x01000201
  sw t0,0x10C8(k1)
  nop

HM_wait3: ; wait till transfer finished
  lw t0,0x10C8(k1)
  li t1,0x01000000
  and t0,t1
  bnez t0,HM_wait3
  nop

  li t0,0xC000
  sh t0,0x1DAA(k1)

  sh zero,0x1C08(k1)
  sh zero,0x1C0A(k1)
  sh zero,0x1C18(k1)
  sh zero,0x1C1A(k1)
  sh zero,0x1C28(k1)
  sh zero,0x1C2A(k1)
  sh zero,0x1C38(k1)
  sh zero,0x1C3A(k1)

  move ra,t9
  jr ra
  nop


; HM_Poll - actual player routine to be called from vblank or rootcounter

HM_Poll:
  move t9,ra

  lb t0,HM_Tick
  lb t1,HM_Speed
  addiu t0,1

  slt t1,t0,t1
  bnez t1,HM_doEffects
  nop

  move t8,zero

HM_doRow:
  lw t0,HM_RowPointer
  nop

  lw t1,(t0)
  nop

  srl t2,t1,20
  andi t2,0xF
  andi t3,t1,0xF0
  or t2,t3

  beqz t2,HM_skip4
  nop

  subi t2,1
  la t3,HM_Volumes
  addu t3,t2
  lb t3,(t3)
  la t4,HM_ChanVol
  addu t4,t8
  sb t3,(t4)
  la t4,HM_LastSmp
  addu t4, t8
  sb t2, (t4)
  nop

HM_skip4:
  andi t2,t1,0xF            
  sll t2,t2,8
  srl t3,t1,8
  andi t3,0xFF
  or t2,t3

  beqz t2,HM_skip5
  nop

  sll t1,t8,4
  addiu t1,0x1C00
  addu t1,k1

  la t3,HM_ChanVol
  addu t3,t8
  lb t3,(t3)
  nop
  sll t3,8
  sh t3,(t1)
  sh t3,2(t1)
  nop

  li t3,7159091
  sll t2,1
  divu t3,t2
  mflo t2
  sll t2,12
  li t3,44100
  divu t2,t3
  mflo t2
  sh t2,4(t1)
                        
  la t3,HM_LastSmp
  addu t3,t8
  lb t6, (t3)
  nop
  sll t6,2
  la t3, HM_SmpAddress
  addu t3,t6
  lw t3, (t3)
  nop
  srl t3,3

  sh t3,6(t1)
  nop
  jal HM_WaitSPU
  nop
  li t3,1

  lh t4,HM_keys
  sllv t3,t8
  or t4,t3
  sh t4,HM_keys

HM_skip5:
  addiu t0,4                    
  sw t0,HM_RowPointer        
  nop

  slti t0,t8,3
  bnez t0,HM_doRow
  addiu t8,1                    

  lh t3,HM_keys
  nop
  sh t3,0x1D88(k1)
  sh zero,0x1D8A(k1)
  sh zero,HM_keys

  lb t0,HM_CurrRow
  sb zero,HM_Tick
  addiu t0,1
  sb t0,HM_CurrRow

  subi t1,t0,64
  bnez t1,HM_skip2
  nop

  lb t1,HM_CurrOrder
  move t0,zero
  lb t2,HM_SongLength
  sb t0,HM_CurrRow
  addiu t1,1
  sub t2,t2,t1
  bgtz t2,HM_skip3
  nop

  move t1,zero

HM_skip3:
  lw t2,HM_ModStart
  sb t1,HM_CurrOrder
  addu t2,t2,t1
  lb t2,952(t2)
  sb t0,HM_CurrRow
  sb t2,HM_CurrPattern

  move a1,t2


  lw t3,HM_ModStart
  sll t2,10
  addu t2,t3
  addiu t2,1084
  sw t2,HM_RowPointer

HM_skip2:
  move ra,t9
  jr ra
  nop


HM_doEffects: ; not implemented yet
  sb t0,HM_Tick

  move ra,t9
  jr ra
  nop


HM_WaitSPU:
  li t7,0x7FF

HM_wait1:
  subiu t7,t7,1
  bgtz t7,HM_wait1
  nop
  lh t7,0x1DAE(k1)
  nop
  andi t7,t7,0x7FF
  bnez t7,HM_WaitSPU
  nop
  jr ra
  nop


HM_ModStart:
  dw 0 ; start address of module
HM_PattDataSize:
  dw 0 ; size of pattern data
HM_SampleEnd:
  dw 0 ; address in SPU ram of end of samples (used to calculate amount of data to send)
HM_RowPointer:
  dw 0 ; pointer to current row to be processed
HM_SmpAddress:
  dcb 124,0 ; address in SPU ram of each sample
HM_keys:
  dh 0
HM_Speed:
  db 6 ; speed
HM_Tick:
  db 6 ; current 'tick' of player
HM_CurrOrder:
  db 0
HM_CurrRow:
  db 0
HM_CurrPattern:
  db 0
HM_SongLength:
  db 0 ; number of orders
HM_NumPatterns:
  db 0 ; number of patterns
HM_FineTunes:
  dcb 31,0 ; area for FT info for each sample
HM_Volumes:
  dcb 31,0 ; same but for volume instead
HM_ChanVol:
  db 0,0,0,0 ; current volumes on channels
HM_LastSmp:
  db 0,0,0,0 ; last sample played on channel