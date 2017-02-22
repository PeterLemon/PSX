;-------------------------------------------------------------
; These are the routines used in the previous example. While
; not exactly the same, they produce the same result and take
; the same arguments as the system calls. They might give you
; some insight in what happens when you call those.
;
; doomed/padua  1999..
;-------------------------------------------------------------
; SetRcnt - Sets a root counter. (Root counter 3 can't be set.)
; in:   a0: Root counter number.
;       a1: Target.
;       a2: Mode.
;You can or these modes together:
;       RCntStop        stops the counter
;       RCntIntr        sets the counter to interrupt mode.
;       RCntSC          sets timer speed to system clock.
;       RCntNotar       sets the timer to count to 65535 instead
;                       of target.
;-------------------------------------------------------------
SetRCnt:
  andi a0,a0,0xFFFF  ; strip descriptor half.
                     ; for kernal compatability.
  slti v0,a0,3       ; only values of 0,1 and 2 allowed.
                     ; Vblanc counter can't be set.
  bne v0,zero,SRC_Do ; process if true
  sll v0,a0,4        ; counter * 16

  jr ra              ; Else quit and return zero
  addu v0,zero,zero  ;

SRC_Do:
  li v1,RCnt_Base       ; Find base offset.
  addu v0,v1,v0         ;
  sw zero,RCnt_Mode(v0) ; Reset Root counter.

  andi v1,a2,RCntIntr   ; Check for interrupt
  sw a1,RCnt_Target(v0) ; Set target.

  beq v1,zero,SRC_Noint ;
  ori t0,zero,0x0040    ; Base mode, IQ2=1
  ori t0,t0,0x0010      ; Set Irq On.
SRC_Noint:
  andi  v1,a2,RCntNotar ; Check for no target mode
  bne v1,zero,SRC_Notar ; flag set?
  slti v1,a0,2          ; Check if counters 0 or 1 are targetted.
  ori t0,t0,0x0008      ; Set count to target on.

SRC_Notar:
  bne v1,zero,SRC_01  ; handle root counters 0 and 1.
  andi v1,a2,RCntSC
                      ; This is only for root counter 2.
  bne v1,zero,SRC_SC  ; branch if system clock is targetted.
  andi v1,a2,RCntStop ;
  ori t0,t0,0x0200    ; set normal speed
SRC_SC:
  beq v1,zero,SRC_Nostop ;
  nop                    ;
  ori t0,t0,0x0001       ; set timer to stop

SRC_Nostop:
  j SRC_Setmode
  nop

SRC_01:                   ; This is for counters 0 and 1.
  bne v1,zero,SRC_Setmode ;
  nop                     ;
  ori t0,t0,0x0100        ; Set normal speed.

SRC_Setmode:
  sw t0,RCnt_Mode(v0) ; Set the mode and return
  jr ra               ;
  ori v0,zero,1       ;

;-------------------------------------------------------------
; GetRcnt - Gets current value of a root counter.
; in:   a0: Counter number
; out:  v0: Current count value.
;           Always zero if counter 3(Vsync) is specified.
;-------------------------------------------------------------
GetRCnt:
  andi v1,a0,0xFFFF  ; Strip Root Count Descriptor
  slti v0,v1,0x0003  ; counter 3(vsync) or improper value?
  bne v0,zero,GRC_Do ; no = go.
  sll v1,v1,0x04     ; Get Counter offset

  jr ra              ; return 0 and quit.
  or v0,zero,zero    ;
GRC_Do:
  li v0,RCnt_Base       ;
  addu v1,v1,v0         ; Make address
  lhu v0,RCnt_Count(v1) ; Get value.
  jr ra                 ; Done.
  nop                   ;
;-------------------------------------------------------------
;StartRCnt - Stets a root counter interrupt mask.
;in     a0: Root counter
;out    v0: 1 for Success
;-------------------------------------------------------------
StartRCnt:
  andi v0,a0,0xFFFF   ; Strip Root Count Descriptor
  slti v1,v0,0x0004   ; root counter 0-3 ?
  bne v1,zero,STRC_Do ; yes = handle
  sll a0,v0,0x02      ; Get table offset (Counter * 4)

  jr ra               ; else quit & zero
  or v0,zero,zero     ;
STRC_Do:
  li v0,0x1F801074 ; v0 = address of interrupt mask register.
  la v1,RCntIMask  ; Get address of Mask table
  addu a0,v1,a0    ; add offset
  lw a0,0(a0)      ; Get mask value.
  lw v1,0(v0)      ; Read mask register
  or v1,v1,a0      ; Apply mask
  jr ra
  sw v1,0(v0)      ; Write to mask register.

;-------------------------------------------------------------
; StopRCnt - Clears a root counter interrupt mask.
; In    a0: Root counter number.
;-------------------------------------------------------------
StopRCnt:
  andi v0,a0,0xFFFF   ; Strip Root Count Descriptor
  slti v1,v0,0x0004   ; root counter 0-3 ?
  bne v1,zero,SPRC_Do ; yes = handle
  sll a0,v0,0x02      ; Get table offset (Counter * 4)

  jr ra               ; else quit & zero
  or v0,zero,zero     ;
SPRC_Do:
  li v0,0x1F801074 ; v0 = address of interrupt mask register.
  la v1,RCntIMask  ; Get address of Mask table
  addu a0,v1,a0    ; add offset
  lw a0,0(a0)      ; Get mask value.
  lw v1,0(v0)      ; Read mask register
  nor a0,zero,a0   ; invert mask value
  and v1,v1,a0     ; Apply mask
  jr ra
  sw v1,0(v0)      ; Write to mask register.
;-------------------------------------------------------------
RCntIMask:     ; Mask table for interrupt reg.
 dw 0x00000010 ; Root counter 0
 dw 0x00000020 ; Root counter 1
 dw 0x00000040 ; Root counter 2
 dw 0x00000001 ; Root counter 3