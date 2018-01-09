.psx
.create "ROOTEXMP.bin", 0x80010000

;--------------------------------------------------------------------------
; Here's a little example of how to set up and start a root
; counter and how to use it to play a mod using Silpheed's
; modplayer.
; More info and explanations in various docs on psx.rules.org.
; 1999 | doomed/padua | psx.rules.org | www.padua.org ........
;--------------------------------------------------------------------------
; Addresses
RCnt_Base   equ 0x1F801100 ; Base address for root counters
RCnt_Count  equ 0x0        ; offset for count register
RCnt_Mode   equ 0x4        ; offset for mode register
RCnt_Target equ 0x8        ; offset for target register

; Counters
DescRC   equ 0xF2000000 ; Rootcounter tag, for event routines.
RCntCNT0 equ 0xF2000000 ; display pixel
RCntCNT1 equ 0xF2000001 ; horizontal sync
RCntCNT2 equ 0xF2000002 ; one-eighth of system clock
RCntCNT3 equ 0xF2000003 ; vertical sync target value fixed to 1

; Modes
RCntIntr  equ 0x1000 ; Interrupt mode
RCntNotar equ 0x0100 ; Count to 65535
RCntStop  equ 0x0010 ; Timer stop mode
RCntSC    equ 0x0001 ; System Clock mode

; Mod player routines
MOD_Load equ 0x80010008 ; Set up the module
MOD_Poll equ 0x8001031C ; Execute one tick

; Mod player variables
Mod_Channels equ 0x80011F40 ; # Channels in mod
Mod_LOrder   equ 0x80012150 ; Last Order
Mod_COrder   equ 0x8001216C ; Current Order
Mod_CPattern equ 0x8001216D ; Current Pattern
Mod_CRow     equ 0x8001216E ; Current Row
Mod_CTick    equ 0x80011271 ; Current Tick
;--------------------------------------------------------------------------
.org 0x80010000

j main
nop

.incbin "MODPLAY.BIN" ; The mod player

main:
  la a0,str1        ; Print welcome line and name of
  la a1,Mod         ; mod to the console.
  jal Printf        ;
  nop               ;

  la a0,Mod         ; get address of MOD
  jal MOD_Load      ;
  nop               ;

  la a0,str11
  lw a1,Mod_Channels
  lw a2,Mod_LOrder
  jal Printf
  nop

  la a0,str2        ; Print that we're setting
  jal Printf        ; it up to the console.
  nop               ;

  jal ResetEntryInt ; Set up interrupt environment.
  nop               ;

; Set up root counter.
  li a0,RCntCNT1   ; Horizontal clock root counter.
  li a1,0x00000138 ; 312 lines
  li a2,RCntIntr   ; Interrupt mode.
  jal SetRCnt      ; Set the counter
  nop              ;

  li a0,RCntCNT1   ; Horizontal clock root counter.
  jal StartRCnt    ; Start the interrupt
  nop              ;

  jal EnterCrit    ; Open event should be called
  nop              ; within a critical section..

  li a0,RCntCNT1   ; Horizontal clock root counter.
  li a1,0x00000002 ; Interrupt event.
  li a2,0x00001000 ; Handle event on interrupt mode
  li a3,RHandler   ; Address of handling function.
  jal OpenEvent    ; Create the event.
  nop              ;
  sw v0,rootsav    ; Save event number.

  jal ExitCrit
  nop

  la a0,str3       ; Print that we're starting it
  jal Printf       ; to the console.
  nop              ;

; And let it roll...
  lw a0,rootsav    ; get identifier
  jal EnableEvent  ; turn it on.
  nop              ;

lpp:
  la a0,str4        ; And to show it works,
  lbu a1,Mod_CRow   ; print some info about the
  lbu a2,Mod_COrder ; playing mod to the console.
  lbu a3,Mod_CPattern
  jal Printf
  nop
  j lpp
  nop

;--------------------------------------------------------------------------
; This is the function that gets called when root counter reaches
; target.
RHandler:
  addiu sp,sp,-8 ; save ra
  sw ra,4(sp)

  jal MOD_Poll   ; play the mod
  nop            ;

  lw ra,4(sp)    ; restore ra
  nop
  jr ra          ; end return
  addiu sp,sp,8  ;

; Because this handler gets called from an event handler,
; there's no need for interrupt handling code.

;--------------------------------------------------------------------------
; System Functions.
;--------------------------------------------------------------------------
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
;--------------------------------------------------------------------------
SetRCnt:
  ori t2,zero,0xB0
  jr t2
  ori t1,zero,0x02
;--------------------------------------------------------------------------
; GetRcnt - Gets current value of a root counter.
; in:   a0: Counter number
; out:  v0: Current count value.
;           Always zero if counter 3(Vsync) is specified.
;--------------------------------------------------------------------------
GetRCnt:
  ori t2,zero,0xB0
  jr t2
  ori t1,zero,0x03
;--------------------------------------------------------------------------
;StartRCnt - Stets a root counter interrupt mask.
;in     a0: Root counter
;out    v0: 1 for Success
;--------------------------------------------------------------------------
StartRCnt:
  ori t2,zero,0xB0
  jr t2
  ori t1,zero,0x04
;--------------------------------------------------------------------------
; StopRCnt - Clears a root counter interrupt mask.
; In    a0: Root counter number.
;--------------------------------------------------------------------------
StopRCnt:
  ori t2,zero,0xB0
  jr t2
  ori t1,zero,0x05

ResetRCnt:
  ori t2,zero,0xB0
  jr t2
  ori t1,zero,0x06
;--------------------------------------------------------------------------
Printf:
  ori t2,zero,0x00A0
  jr t2
  ori t1,zero,0x003F
;--------------------------------------------------------------------------
OpenEvent:             ; a0 class
  addiu t2,zero,0x00B0 ; a1 spec
  jr t2                ; a2 mode
  addiu t1,zero,0x0008 ; a3 address of function
  nop                  ; ->v0 #event
;--------------------------------------------------------------------------
EnableEvent: ; a0 #event
  addiu t2,zero,0x00B0
  jr t2
  addiu t1,zero,0x000C
  nop
;--------------------------------------------------------------------------
DisableEvent: ; a0 #event
  addiu t2,zero,0x00B0
  jr t2
  addiu t1,zero,0x000D
  nop
;--------------------------------------------------------------------------
CloseEvent: ; a0 #event
  addiu t2,zero,0x00B0
  jr t2
  addiu t1,zero,0x0009
  nop
;--------------------------------------------------------------------------
EnterCrit:
  addiu a0,zero,0x0001
  syscall 0x00000
  jr ra
  nop
;--------------------------------------------------------------------------
ExitCrit:
  addiu a0,zero,0x0002
  syscall 0x00000
  jr ra
  nop
;--------------------------------------------------------------------------
ResetEntryInt:
  addiu t2,zero,0x00B0
  jr t2
  addiu t1,zero,0x0018
;--------------------------------------------------------------------------

; Data
rootsav:
  dw 0 ; Here goes the event identifier.

str1:
  db 0x0D,0x0A,"Root counter example by Doomed/Padua. Modplayer by Silpheed/Hitmen.",0x0D,0x0A,0x0A
  db "Loading MOD: ",0x27,"%s",0x27,0x0D,0x0A,0
str11:
  db "%02d Channels",0x2C," %02d Orders",0x0D,0x0A,0
str2:
  db "Setting up root counter",0x0D,0x0A,0
str3:
  db "Starting root counter",0x0D,0x0A,0x0A,0
str4:
  db "Row: %02d",0x2C," Order: %02d",0x2C," Pattern: %02d",0x0D,0

.align 4 ; always important..
Mod:
  .incbin "LUNATIC.HIT" ; the module

.close