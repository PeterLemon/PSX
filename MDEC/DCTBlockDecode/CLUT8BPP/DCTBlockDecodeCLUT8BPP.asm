; PSX 'Bare Metal' MDEC DCT Block Decode CLUT 8BPP Demo by krom (Peter Lemon):
.psx
.create "DCTBlockDecodeCLUT8BPP.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_GPU.INC" ; Include PSX GPU Definitions & Macros
.include "LIB/PSX_MDEC.INC" ; Include PSX MDEC Definitions & Macros

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup Screen Mode
WRGP1 GPURESET,0  ; Write GP1 Command Word (Reset GPU)
WRGP1 GPUDISPEN,0 ; Write GP1 Command Word (Enable Display)
WRGP1 GPUDISPM,HRES320+VRES240+BPP15+VNTSC ; Write GP1 Command Word (Set Display Mode: 320x240, 15BPP, NTSC)
WRGP1 GPUDISPH,0xC60260 ; Write GP1 Command Word (Horizontal Display Range 608..3168)
WRGP1 GPUDISPV,0x042018 ; Write GP1 Command Word (Vertical Display Range 24..264)

; Setup Drawing Area
WRGP0 GPUDRAWM,0x000488   ; Write GP0 Command Word (Drawing To Display Area Allowed Bit 10, Texture Page Colors = 8BPP Bit 7..8, Texture Page Y Base = 0 Bit 4, Texture Page X Base = 512 Bit 0..3)
WRGP0 GPUDRAWATL,0x000000 ; Write GP0 Command Word (Set Drawing Area Top Left X1=0, Y1=0)
WRGP0 GPUDRAWABR,0x03BD3F ; Write GP0 Command Word (Set Drawing Area Bottom Right X2=319, Y2=239)
WRGP0 GPUDRAWOFS,0x000000 ; Write GP0 Command Word (Set Drawing Offset X=0, Y=0)

; Set Quantization Table
SetQuantTableY Q ; MDEC($02) - Set Quantization Table: Quantization Table Y (Luminance)

; Set Scale Table
SetScaleTable S ; MDEC($03) - Set Scale Table: Scale Table

; Copy Palette Color Look Up Table (CLUT) To VRAM
CopyRectCPU 512,256, 256,1 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,127 ; T0 = Data Copy Word Count
la a1,CLUT ; A1 = Texture RAM Offset
CopyCLUT:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyCLUT ; IF (T0 != 0) Copy Texture64x64
  subiu t0,1 ; T0-- (Delay Slot)


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,2,DCTA ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureA8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 512,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureA8x8 ; A1 = Texture RAM Offset
CopyTextureA8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureA8x8 ; IF (T0 != 0) Copy Texture A 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 32,8, 0,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,2,DCTB ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureB8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 516,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureB8x8 ; A1 = Texture RAM Offset
CopyTextureB8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureB8x8 ; IF (T0 != 0) Copy Texture B 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 44,8, 8,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,2,DCTC ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureC8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 520,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureC8x8 ; A1 = Texture RAM Offset
CopyTextureC8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureC8x8 ; IF (T0 != 0) Copy Texture C 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 56,8, 16,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,3,DCTD ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureD8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 524,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureD8x8 ; A1 = Texture RAM Offset
CopyTextureD8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureD8x8 ; IF (T0 != 0) Copy Texture D 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 68,8, 24,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,3,DCTE ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureE8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 528,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureE8x8 ; A1 = Texture RAM Offset
CopyTextureE8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureE8x8 ; IF (T0 != 0) Copy Texture E 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 80,8, 32,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,2,DCTF ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureF8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 532,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureF8x8 ; A1 = Texture RAM Offset
CopyTextureF8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureF8x8 ; IF (T0 != 0) Copy Texture F 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 92,8, 40,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


; Decode DCT Macroblock
DecodeMacroBlock 1,0,0,15,DCTG ; MDEC($01) - Decode Macroblock: Depth 8-Bit, Sign, Bit15, Word Size, RLE Quantized DCT Block

; Read Data/Response
ReadMDEC 16,TextureG8x8 ; MDEC - Read Data/Response: Data Read Word Size, RAM Output Address

; Copy Texture To VRAM
CopyRectCPU 536,0, 4,8 ; Copy Rectangle (CPU To VRAM): X,Y, Width,Height
li t0,15 ; T0 = Data Copy Word Count
la a1,TextureG8x8 ; A1 = Texture RAM Offset
CopyTextureG8x8:
  lw t1,0(a1) ; T1 = DATA Word
  addiu a1,4  ; A1 += 4 (Delay Slot)
  sw t1,GP0(a0) ; Write GP0 Packet Word
  bnez t0,CopyTextureG8x8 ; IF (T0 != 0) Copy Texture G 8x8
  subiu t0,1 ; T0-- (Delay Slot)

; Render Texture Rectangle
TexRectRaw 104,8, 48,0, 0x4020, 8,8 ; Texture Rectangle Raw: X,Y, U,V, PAL, Width,Height


Loop:
  b Loop
  nop ; Delay Slot

CLUT:
  dh 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000 ; 256x16B = 512 Bytes
  dh 0x0421,0x0421,0x0421,0x0421,0x0421,0x0421,0x0421,0x0421
  dh 0x0842,0x0842,0x0842,0x0842,0x0842,0x0842,0x0842,0x0842
  dh 0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63,0x0C63
  dh 0x1084,0x1084,0x1084,0x1084,0x1084,0x1084,0x1084,0x1084
  dh 0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5,0x14A5
  dh 0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6,0x18C6
  dh 0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7,0x1CE7
  dh 0x2108,0x2108,0x2108,0x2108,0x2108,0x2108,0x2108,0x2108
  dh 0x2529,0x2529,0x2529,0x2529,0x2529,0x2529,0x2529,0x2529
  dh 0x294A,0x294A,0x294A,0x294A,0x294A,0x294A,0x294A,0x294A
  dh 0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B,0x2D6B
  dh 0x318C,0x318C,0x318C,0x318C,0x318C,0x318C,0x318C,0x318C
  dh 0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD,0x35AD
  dh 0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE,0x39CE
  dh 0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF,0x3DEF
  dh 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
  dh 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631
  dh 0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
  dh 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
  dh 0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x5294
  dh 0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5,0x56B5
  dh 0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6,0x5AD6
  dh 0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7,0x5EF7
  dh 0x6318,0x6318,0x6318,0x6318,0x6318,0x6318,0x6318,0x6318
  dh 0x6739,0x6739,0x6739,0x6739,0x6739,0x6739,0x6739,0x6739
  dh 0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A,0x6B5A
  dh 0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B,0x6F7B
  dh 0x739C,0x739C,0x739C,0x739C,0x739C,0x739C,0x739C,0x739C
  dh 0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD,0x77BD
  dh 0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE,0x7BDE
  dh 0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF,0x7FFF

; 700,0,0,0,0,0,0,0 ; We Apply The IDCT To A Matrix, Only Containing A DC Value Of 700.
; 0,0,0,0,0,0,0,0   ; It Will Produce A Grey Colored Square.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
DCTA: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(863 & 0x3FF)
  dh (63<<10)+(0 & 0x3FF)
  dh 0xFE00,0x0000 ; MDEC End Of Data Code

; 700,100,0,0,0,0,0,0 ; Now Let's Add An AC Value Of 100, At The 1st Position
; 0,0,0,0,0,0,0,0     ; It Will Produce A Bar Diagram With A Curve Like A Half Cosine Line.
; 0,0,0,0,0,0,0,0     ; It Is Said It Has A Frequency Of 1 In X-Direction.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
DCTB: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(864 & 0x3FF)
  dh (0<<10)+(50 & 0x3FF)
  dh (62<<10)+(0 & 0x3FF)
  dh 0xFE00 ; MDEC End Of Data Code

; 700,0,100,0,0,0,0,0 ; What Happens If We Place The AC Value Of 100 At The Next Position?
; 0,0,0,0,0,0,0,0     ; The Shape Of The Bar Diagram Shows A Cosine Line, Too.
; 0,0,0,0,0,0,0,0     ; But Now We See A Full Period.
; 0,0,0,0,0,0,0,0     ; The Frequency Is Twice As High As In The Previous Example.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
DCTC: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(864 & 0x3FF)
  dh (1<<10)+(50 & 0x3FF)
  dh (61<<10)+(0 & 0x3FF)
  dh 0xFE00 ; MDEC End Of Data Code

; 700,100,100,0,0,0,0,0 ; But What Happens If We Place Both AC Values?
; 0,0,0,0,0,0,0,0       ; The Shape Of The Bar Diagram Is A Mix Of Both The 1st & 2nd Cosines.
; 0,0,0,0,0,0,0,0       ; The Resulting AC Value Is Simply An Addition Of The Cosine Lines.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
DCTD: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(864 & 0x3FF)
  dh (0<<10)+(50 & 0x3FF)
  dh (0<<10)+(50 & 0x3FF)
  dh (61<<10)+(0 & 0x3FF)
  dh 0xFE00,0x0000 ; MDEC End Of Data Code

; 700,100,100,0,0,0,0,0 ; Now Let's Add An AC Value At The Other Direction.
; 200,0,0,0,0,0,0,0     ; Now The Values Vary In Y Direction, Too. The Principle Is:
; 0,0,0,0,0,0,0,0       ; The Higher The Index Of The AC Value The Greater The Frequency Is.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
DCTE: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(864 & 0x3FF)
  dh (0<<10)+(50 & 0x3FF)
  dh (0<<10)+(50 & 0x3FF)
  dh (5<<10)+(100 & 0x3FF)
  dh (55<<10)+(0 & 0x3FF)
  dh 0xFE00 ; MDEC End Of Data Code

; 950,0,0,0,0,0,0,0 ; Placing An AC Value At The Opposite Side Of The DC Value.
; 0,0,0,0,0,0,0,0   ; The Highest Possible Frequency Of 8 Is Applied In Both X- & Y- Direction.
; 0,0,0,0,0,0,0,0   ; Because Of The High Frequency The Neighbouring Values Differ Numerously.
; 0,0,0,0,0,0,0,0   ; The Picture Shows A Checker-Like Appearance.
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,0
; 0,0,0,0,0,0,0,500
DCTF: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(989 & 0x3FF)
  dh (62<<10)+(250 & 0x3FF)
  dh 0xFE00,0x0000 ; MDEC End Of Data Code

; 600,0,-261,0,-200,0,-108,0 ; Letter "A" Character
; -106,0,-196,0,256,0,196,0
; -185,0,100,0,185,0,-241,0
; 217,0,-166,0,90,0,-166,0
; 0,0,0,0,0,0,0,0
; -145,0,111,0,-60,0,111,0
; 77,0,-41,0,-77,0,100,0
; 21,0,39,0,-51,0,-39,0
DCTG: ; DCT RLE 8x8 Matrix
  dh (0<<10)+(810 & 0x3FF)
  dh (1<<10)+(-130 & 0x3FF)
  dh (1<<10)+(-100 & 0x3FF)
  dh (1<<10)+(-54 & 0x3FF)
  dh (1<<10)+(-53 & 0x3FF)
  dh (1<<10)+(-98 & 0x3FF)
  dh (1<<10)+(128 & 0x3FF)
  dh (1<<10)+(98 & 0x3FF)
  dh (1<<10)+(-92 & 0x3FF)
  dh (1<<10)+(50 & 0x3FF)
  dh (1<<10)+(92 & 0x3FF)
  dh (1<<10)+(-120 & 0x3FF)
  dh (1<<10)+(108 & 0x3FF)
  dh (1<<10)+(-83 & 0x3FF)
  dh (1<<10)+(45 & 0x3FF)
  dh (1<<10)+(-83 & 0x3FF)
  dh (9<<10)+(-72 & 0x3FF)
  dh (1<<10)+(55 & 0x3FF)
  dh (1<<10)+(-30 & 0x3FF)
  dh (1<<10)+(55 & 0x3FF)
  dh (1<<10)+(38 & 0x3FF)
  dh (1<<10)+(-20 & 0x3FF)
  dh (1<<10)+(-38 & 0x3FF)
  dh (1<<10)+(50 & 0x3FF)
  dh (1<<10)+(10 & 0x3FF)
  dh (1<<10)+(19 & 0x3FF)
  dh (1<<10)+(-25 & 0x3FF)
  dh (1<<10)+(-19 & 0x3FF)
  dh (0<<10)+(0 & 0x3FF)
  dh 0xFE00 ; MDEC End Of Data Code

Q: ; PSX Standard Quantization 8x8 Matrix
  db  2,16,19,22,26,27,29,34
  db 16,16,22,24,27,29,34,37
  db 19,22,26,27,29,34,34,38
  db 22,22,26,27,29,34,37,40
  db 22,26,27,29,32,35,40,48
  db 26,27,29,32,35,40,48,58
  db 26,27,29,34,38,46,56,69
  db 27,29,35,38,46,56,69,83

S: ; JPEG Standard Scale 8x8 Matrix (Signed 16-Bit Fraction: S.1.14)
  dh 23170,  23170,  23170,  23170,  23170,  23170,  23170,  23170
  dh 32138,  27245,  18204,   6392,  -6393, -18205, -27246, -32139
  dh 30273,  12539, -12540, -30274, -30274, -12540,  12539,  30273
  dh 27245,  -6393, -32139, -18205,  18204,  32138,   6392, -27246
  dh 23170, -23171, -23171,  23170,  23170, -23171, -23171,  23170
  dh 18204, -32139,   6392,  27245, -27246,  -6393,  32138, -18205
  dh 12539, -30274,  30273, -12540, -12540,  30273, -30274,  12539
  dh  6392, -18205,  27245, -32139,  32138, -27246,  18204,  -6393

TextureA8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureB8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureC8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureD8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureE8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureF8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

TextureG8x8:
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0
  db 0,0,0,0,0,0,0,0

.close