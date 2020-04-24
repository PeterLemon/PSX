; PSX 'Bare Metal' HUFFMAN Decode Demo by krom (Peter Lemon):
.psx
.create "HUFFMANDecode.bin", 0x80010000

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

la a1,Huff    ; A1 = Source Address
lui a2,0x8010 ; A2 = Destination Address (RAM Start Offset)

lbu t0,3(a1) ; T0 = HI Data Length Byte
lbu t1,2(a1) ; T1 = MID Data Length Byte
sll t0,8
or t0,t1
lbu t1,1(a1) ; T1 = LO Data Length Byte
sll t0,8
or t0,t1     ; T0 = Data Length
add t0,a2    ; T0 = Destination End Offset (RAM End Offset)
addi a1,4    ; Add 4 To Huffman Offset

lbu t1,0(a1) ; T1 = (Tree Table Size / 2) - 1
addi a1,1    ; A1 = Tree Table
sll t1,1
addi t1,1    ; T1 = Tree Table Size
add t1,a1    ; T1 = Compressed Bitstream Offset

subi a1,5   ; A1 = Source Address
ori t6,r0,0 ; T6 = Branch/Leaf Flag (0 = Branch 1 = Leaf)
ori t7,r0,5 ; T7 = Tree Table Offset (Reset)
HuffChunkLoop:
  lbu t2,3(t1) ; T2 = Data Length Byte 0
  lbu t8,2(t1) ; T8 = Data Length Byte 1
  sll t2,8
  or t2,t8
  lbu t8,1(t1) ; T8 = Data Length Byte 2
  sll t2,8
  or t2,t8
  lbu t8,0(t1)  ; T8 = Data Length Byte 3
  sll t2,8
  or t2,t8      ; T2 = Node Bits (Bit31 = First Bit)
  addi t1,4     ; Add 4 To Compressed Bitstream Offset
  lui t3,0x8000 ; T3 = Node Bit Shifter

  HuffByteLoop: 
    beq a2,t0,HuffEnd ; IF (Destination Address == Destination End Offset) HuffEnd
    nop ; Delay Slot
    beqz t3,HuffChunkLoop ; IF (Node Bit Shifter == 0) HuffChunkLoop
    nop ; Delay Slot

    add t8,a1,t7
    lbu t4,0(t8) ; T4 = Next Node
    andi t8,t6,1 ; Test T6 == Leaf
    beqz t8,HuffBranch
    nop ; Delay Slot
    sb t4,0(a2) ; Store Data Byte To Destination IF Leaf
    addi a2,1   ; Add 1 To RAM Offset
    ori t6,r0,0 ; T6 = Branch
    ori t7,r0,5 ; T7 = Tree Table Offset (Reset)
    j HuffByteLoop
    nop ; Delay Slot

    HuffBranch:
      andi t5,t4,0x3F ; T5 = Offset To Next Child Node
      sll t5,1
      addi t5,2        ; T5 = Node0 Child Offset * 2 + 2
      li t8,0xFFFFFFFE ; T7 = Tree Offset NOT 1
      and t7,t8
      add t7,t5 ; T7 = Node0 Child Offset

      and t8,t2,t3 ; Test Node Bit (0 = Node0, 1 = Node1)
      srl t3,1     ; Shift T3 To Next Node Bit
      beqz t8,HuffNode0
      nop ; Delay Slot
      addi t7,1      ; T7 = Node1 Child Offset
      ori t8,r0,0x40 ; T8 = Test Node1 End Flag
      j HuffNodeEnd
      nop ; Delay Slot
      HuffNode0:
        ori t8,r0,0x80 ; T8 = Test Node0 End Flag
      HuffNodeEnd:

      and t9,t4,t8 ; Test Node End Flag (1 = Next Child Node Is Data)
      beqz t9,HuffByteLoop
      nop ; Delay Slot
      ori t6,r0,1 ; T6 = Leaf
      j HuffByteLoop
      nop ; Delay Slot
  HuffEnd:

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

Huff:
  .incbin "Image.huff" ; Include 640x480 24BPP Compressed Image Data (200500 Bytes)

.close