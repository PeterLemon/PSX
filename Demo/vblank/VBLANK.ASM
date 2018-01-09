.psx
.create "VBLANK.bin", 0x80010000

;--------------------------------------------------------------------------
; Vertical Blank example.                                       1999 doomed
; Here's a quick example of a vertical sync handler. It does a nice counter
; so you can count how many frames your stuff takes.
; This is by no means the most beautiful way to do it, but it works and
; ensures your vblank thing gets called as one of the first things in the
; irq handler. This does assume the normal exception handlers to be installed.
;
; When an irq occurs the psx saves the registers then calls all routines
; in interrupt que 0, then from que 1, 2 and 3. After that the routine
; installed by HookEntryInt gets called. Which is normally the Return from
; exception routine, which restores all registers, does an rfe and then
; returns from to the normal program.
;--------------------------------------------------------------------------
Intreg_pending  equ 0x1F801070 ; Interrupt occur
Intreg_mask     equ 0x1F801074 ; Interrupt enable
I_Vblank        equ 0x1        ; Vblank is bit 0
;-------------------------------------------------------------
.org 0x80010000

la a0,str1
jal printf
nop

jal InstallVblank
nop

endless:
  lw a1,Vblankcount
  la a0,str2
  jal printf
  nop
  j endless
  nop

;-------------------------------------------------------------
printf:
  addiu t2,zero,0x00A0
  jr t2
  addiu t1,zero,0x003F
;-------------------------------------------------------------

WaitVblank:
  sw zero,Vblankcount
  nop
wvbl:
  lw v0,Vblankcount
  nop
  beq v0,zero,wvbl
  nop
  jr ra
  nop
;-------------------------------------------------------------
InstallVblank:        ; Installs the Vsync handler.
  addiu sp,sp,-0x10
  sw ra,8(sp)

  sw zero,Vblankcount ; counter to zero

  jal EnterCrit       ; Enter critical section
  nop                 ; = disable irq's

  ori a0,zero,0       ; Que 0 -> is handled first
  la a1,HandleVsync   ; location of struct.

  jal SysEnqIntrp     ; Insert into interrupt que
  nop                 ;

  lw a0,Intreg_mask   ; get mask register
  nop
  ori a0,a0,I_Vblank  ; turn on vsync irq
  sw a0,Intreg_mask   ; write back

  jal ExitCrit        ; And enable irq's again.
  nop

  lw ra,8(sp)
  nop
  jr ra
  nop
;--------------------------------------------------------------------------
Vblank_handler:
  lw v0,Intreg_mask    ; get mask
  nop
  andi v0,v0,I_Vblank  ; vblank enable?
  beqz v0,Vblank_end   ; Quit if not enabled
  nop
  lw v0,Intreg_pending ; Get pending
  nop
  andi v1,v0,I_Vblank
  beqz v1,Vblank_end   ; Quit if not occurred
  nop

; If you're using other Vblank stuff as well (cards/pads etc..) you should
; not acknowledge the vblank, as this handler gets called first.. (que 0,
; the vsync handler used by the pads etc is in que 2) So delete next 2
; lines in that case. (Also make sure you init those before this handler.)

  xori v0,v0,I_Vblank  ; invert vblankpending
  sw v0,Intreg_pending ; and write back

  lw v0,Vblankcount    ; increase counter
  nop
  addiu v0,v0,1
  sw v0,Vblankcount

Vblank_end:
  jr ra
  ori v0,zero,0 ; return 0.
;--------------------------------------------------------------------------
EnterCrit:
  addiu a0,zero,0x0001
  syscall 0x00000
  jr ra
  nop
;--------------------------------------------------------------------------
SysEnqIntrp: ; Enques and IRQ.
  addiu t2,zero,0x00C0
  jr t2
  addiu t1,zero,0x0002
;--------------------------------------------------------------------------
ExitCrit:
  addiu a0,zero,0x0002
  syscall 0x00000
  jr ra
  nop
;--------------------------------------------------------------------------
Vblankcount:
  dw 0

; This structure will be added to the handlerlist
HandleVsync:
  dw 0              ; will contain next irqhandler in queue
  dw 0              ; func1
  dw Vblank_handler ; func2
  dw 0              ; pad

; func2 is called first. If there's a func1, and func2 returns a value <> 0,
; func1 gets called with the return value of func2 in a0.
;--------------------------------------------------------------------------
str1:
  db 0x0D,0x0A,"Vertical blank example.                        1999",0x2C," doomed/padua",0x0D,0x0A
str2:
  db "Vblanks since start: %8x",0x0D,0

.close