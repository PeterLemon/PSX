;===========
; PSX INPUT
;===========
;=================
; Digital Buttons
;=================
JOY_L2     equ 0x0001 ; Joypad Input: L2       (Bit 0)
JOY_R2     equ 0x0002 ; Joypad Input: R2       (Bit 1)
JOY_L1     equ 0x0004 ; Joypad Input: L1       (Bit 2)
JOY_R1     equ 0x0008 ; Joypad Input: R1       (Bit 3)
JOY_T      equ 0x0010 ; Joypad Input: Triangle (Bit 4)
JOY_C      equ 0x0020 ; Joypad Input: Circle   (Bit 5)
JOY_X      equ 0x0040 ; Joypad Input: X        (Bit 6)
JOY_S      equ 0x0080 ; Joypad Input: Square   (Bit 7)
JOY_SELECT equ 0x0100 ; Joypad Input: Select   (Bit 8)
JOY_L3     equ 0x0200 ; Joypad Input: L3       (Bit 9)  (Analog Mode Only)
JOY_R3     equ 0x0400 ; Joypad Input: R3       (Bit 10) (Analog Mode Only)
JOY_START  equ 0x0800 ; Joypad Input: Start    (Bit 11)
JOY_UP     equ 0x1000 ; Joypad Input: Up       (Bit 12)
JOY_RIGHT  equ 0x2000 ; Joypad Input: Right    (Bit 13)
JOY_DOWN   equ 0x4000 ; Joypad Input: Down     (Bit 14)
JOY_LEFT   equ 0x8000 ; Joypad Input: Left     (Bit 15)

;==============
; Input Macros
;==============
.macro InitJoy,BUFFER ; Initialise Joypads & Setup VSync Wait Routine Using BIOS: Buffer Address
  li t1,0x15
  li a0,0x20000001
  li t2,0xB0
  la a1,BUFFER ; Set Pad Buffer Address To Automatically Update Each Frame
  jalr t2 ; Jump To BIOS Routine
  nop ; Delay Slot
.endmacro

.macro WaitVSync,BUFFER,DATA ; Wait For Vertical Retrace Period & Store XOR Pad Data: Buffer Address, Data Address
  la a1,BUFFER   ; Load Pad Buffer Address
  Wait:          ; Wait For Vertical Retrace Period & Store XOR Pad Data
    lw t0,0(a1)  ; Load Pad Buffer
    nop          ; Delay Slot
    beqz t0,Wait ; IF (Pad Buffer == 0) Wait
    nor t0,r0    ; NOR Compliment Pad Data Bits (Delay Slot)
    sw r0,0(a1)  ; Store Zero To Pad Buffer
    la a1,DATA   ; Load Pad Data Address
    sw t0,0(a1)  ; Store Pad Data
.endmacro

.macro IsJoyDown,INPUT,DATA ; Is Joypad Digital Button Pressed Down: Input, Input Data Address
  la a1,DATA  ; Load Input Data Address
  lw t0,0(a1) ; Load Input Data Word
  nop ; Delay Slot
  andi t0,INPUT ; T0 = Input Status
.endmacro