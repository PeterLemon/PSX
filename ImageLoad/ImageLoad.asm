; PSX 'Bare Metal' Image Load Demo by krom (Peter Lemon):
.psx
.create "ImageLoad.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup Screen Mode
WRGP1 GPURESET,0  ; Write GP1 Command Word (Reset GPU)
WRGP1 GPUDISPEN,0 ; Write GP1 Command Word (Enable Display)
WRGP1 GPUDISPM,HRES640+VRES480+BPP24+VNTSC ; Write GP1 Command Word (Set Display Mode: 640x480, 24BPP, NTSC, Vertical Interlace)
WRGP1 GPUDISPH,0xC60260 ; Write GP1 Command Word (Horizontal Display Range 608..3168)
WRGP1 GPUDISPV,0x07E018 ; Write GP1 Command Word (Vertical Display Range 24..504)

; Setup Drawing Area
WRGP0 GPUDRAWM,0x000400   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BD3F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=319, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

; Memory Transfer
CopyRectCPU 0,0, 960,480 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,230399 ; T0 = Data Copy Word Count
la a1,Image ; A1 = Image RAM Offset
CopyImage:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyImage ; IF (T0 != 0) Copy Image
  subiu t0,1 ; T0-- (Delay Slot)

Loop:
  b Loop
  nop ; Delay Slot

Image:
  .incbin "Image.bin" ; Include 640x480 24BPP Image Data (921600 Bytes)

.close