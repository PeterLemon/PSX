; PSX 'Bare Metal' LZ77 Decode Demo by krom (Peter Lemon):
.psx
.create "LZ77Decode.bin", 0x80010000

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

la a1,LZ+4    ; A1 = Source Address
lui a2,0xA010 ; A2 = Destination Address (RAM Start Offset)

lbu t0,-1(a1) ; T0 = HI Data Length Byte
lbu t1,-2(a1) ; T1 = MID Data Length Byte
sll t0,8
or t0,t1
lbu t1,-3(a1) ; T1 = LO Data Length Byte
sll t0,8
or t0,t1      ; T0 = Data Length
addu t0,a2    ; T0 = Destination End Offset (RAM End Offset)

LZLoop:
  lbu t1,0(a1)        ; T1 = Flag Data For Next 8 Blocks (0 = Uncompressed Byte, 1 = Compressed Bytes)
  addiu a1,1          ; Add 1 To LZ Offset
  ori t2,r0,10000000b ; T2 = Flag Data Block Type Shifter
  LZBlockLoop:
    beq a2,t0,LZEnd  ; IF (Destination Address == Destination End Offset) LZEnd
    and t4,t1,t2     ; Test Block Type (Delay Slot)
    beqz t2,LZLoop   ; IF (Flag Data Block Type Shifter == 0) LZLoop
    srl t2,1         ; Shift T2 To Next Flag Data Block Type (Delay Slot)
    lbu t3,0(a1)     ; T3 = Copy Uncompressed Byte / Number Of Bytes To Copy & Disp MSB's
    bnez t4,LZDecode ; IF (BlockType != 0) LZDecode Bytes
    addiu a1,1       ; Add 1 To LZ Offset (Delay Slot)
    sb t3,0(a2)      ; Store Uncompressed Byte To Destination
    j LZBlockLoop
    addiu a2,1       ; Add 1 To RAM Offset (Delay Slot)

    LZDecode:
      lbu t4,0(a1)  ; T4 = Disp LSB's
      addiu a1,1    ; Add 1 To LZ Offset
      sll t5,t3,8   ; T5 = Disp MSB's
      or t4,t5      ; T4 = Disp 16-Bit
      andi t4,0xFFF ; T4 &= $FFF (Disp 12-Bit)
      nor t4,r0     ; T4 = -Disp - 1
      addu t4,a2    ; T4 = Destination - Disp - 1
      srl t3,4      ; T3 = Number Of Bytes To Copy (Minus 3)
      addiu t3,3    ; T3 = Number Of Bytes To Copy
      LZCopy:
        lbu t5,0(t4)   ; T5 = Byte To Copy
        addiu t4,1     ; Add 1 To T4 Offset
        sb t5,0(a2)    ; Store Byte To RAM
        subiu t3,1     ; Number Of Bytes To Copy -= 1
        bnez t3,LZCopy ; IF (Number Of Bytes To Copy != 0) LZCopy Bytes
        addiu a2,1     ; Add 1 To RAM Offset (Delay Slot)
        j LZBlockLoop
        nop ; Delay Slot
  LZEnd:

; Memory Transfer
CopyRectCPU 0,0, 960,480 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,230399  ; T0 = Data Copy Word Count
lui a1,0xA010 ; A1 = Image RAM Offset
CopyImage:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyImage ; IF (T0 != 0) Copy Image
  subiu t0,1 ; T0-- (Delay Slot)

Loop:
  b Loop
  nop ; Delay Slot

LZ:
  .incbin "Image.lz" ; Include 640x480 24BPP Compressed Image Data (137129 Bytes)

.close