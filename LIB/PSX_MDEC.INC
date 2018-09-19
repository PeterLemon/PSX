;==========
; PSX MDEC
;==========
;===============
; MDEC Commands
;===============
MDECNOP                   equ 0x00 ; MDEC Commands: $00 - NOP
MDECBLOCK                 equ 0x01 ; MDEC Commands: $01 - Decode Macroblock
MDECSETQUANT              equ 0x02 ; MDEC Commands: $02 - Set Quantization Table
MDECSETSCALE              equ 0x03 ; MDEC Commands: $03 - Set Scale Table
MDEC4                     equ 0x04 ; MDEC Commands: $04 - NOP
MDEC5                     equ 0x05 ; MDEC Commands: $05 - NOP
MDEC6                     equ 0x06 ; MDEC Commands: $06 - NOP
MDEC7                     equ 0x07 ; MDEC Commands: $07 - NOP

;=============
; MDEC Macros
;=============
; MDEC Decode Macroblock
.macro DecodeMacroBlock,DEPTH,SIGN,BIT15,SIZE,DCT ; MDEC($01) - Decode Macroblock
  WRIOW MDECPAR,(0x01<<29)+(DEPTH<<27)+(SIGN<<26)+(BIT15<<25)+SIZE ; Write MDEC Command Word (Color+Command)
  li t0,SIZE-1                     ; T0 = Data Copy Word Count
  la a1,DCT                        ; A1 = Quantization Table Y Address
  CopyDCT:                         ; Copy DCT Words
    lw t1,0(a1)                    ; Load MDEC Parameter Word
    addiu a1,4                     ; A1 += 4 (Delay Slot)
    sw t1,MDECPAR(a0)              ; Write MDEC Parameter Word (DCT Data: Parameter Words)
    bnez t0,CopyDCT                ; IF (T0 != 0) Copy DCT
    subiu t0,1                     ; T0-- (Delay Slot)
.endmacro

; MDEC Set Quantization Table
.macro SetQuantTableY,YQ           ; MDEC($02) - Set Quantization Table Y (Luminance)
  WRIOW MDECPAR,(0x02<<29)+0       ; Write MDEC Command Word (Color+Command)
  li t0,15                         ; T0 = Data Copy Word Count
  la a1,YQ                         ; A1 = Quantization Table Y Address
  CopyQuantTableY:                 ; Copy 64 Unsigned Bytes Of Quantization Table Y
    lw t1,0(a1)                    ; Load MDEC Parameter Word
    addiu a1,4                     ; A1 += 4 (Delay Slot)
    sw t1,MDECPAR(a0)              ; Write MDEC Parameter Word (Quantization Table Data: Unsigned Parameter Bytes)
    bnez t0,CopyQuantTableY        ; IF (T0 != 0) Copy Quant Table Y
    subiu t0,1                     ; T0-- (Delay Slot)
.endmacro

.macro SetQuantTableYUV,YQ,UVQ     ; MDEC($02) - Set Quantization Table Y (Luminance) & UV (Color)
  WRIOW MDECPAR,(0x02<<29)+1       ; Write MDEC Command Word (Color+Command)
  li t0,15                         ; T0 = Data Copy Word Count
  la a1,YQ                         ; A1 = Quantization Table Y Address
  CopyQuantTableY:                 ; Copy 64 Unsigned Bytes Of Quantization Table Y
    lw t1,0(a1)                    ; Load MDEC Parameter Word
    addiu a1,4                     ; A1 += 4 (Delay Slot)
    sw t1,MDECPAR(a0)              ; Write MDEC Parameter Word (Quantization Table Data: Unsigned Parameter Bytes)
    bnez t0,CopyQuantTableY        ; IF (T0 != 0) Copy Quant Table Y
    subiu t0,1                     ; T0-- (Delay Slot)

  li t0,15                         ; T0 = Data Copy Word Count
  la a1,UVQ                        ; A1 = Quantization Table UV Address
  CopyQuantTableUV:                ; Copy 64 Unsigned Bytes Of Quantization Table UV
    lw t1,0(a1)                    ; Load MDEC Parameter Word
    addiu a1,4                     ; A1 +=4 (Delay Slot)
    sw t1,MDECPAR(a0)              ; Write MDEC Parameter Word (Quantization Table Data: Unsigned Parameter Bytes)
    bnez t0,CopyQuantTableUV       ; IF (T0 != 0) Copy Quant Table UV
    subiu t0,1                     ; T0-- (Delay Slot)
.endmacro

; MDEC Set Scale Table
.macro SetScaleTable,ST            ; MDEC($03) - Set Scale Table (Signed 16-Bit Fraction: S.1.14)
  WRIOW MDECPAR,(0x03<<29)         ; Write MDEC Command Word (Command)
  li t0,31                         ; T0 = Data Copy Word Count
  la a1,ST                         ; A1 = Scale Table Address
  CopyScaleTable:                  ; Copy 64 Signed Shorts Of Scale Table
    lw t1,0(a1)                    ; Load MDEC Parameter Word
    addiu a1,4                     ; A1 += 4 (Delay Slot)
    sw t1,MDECPAR(a0)              ; Write MDEC Parameter Word (Scale Table Data: Signed Parameter Shorts)
    bnez t0,CopyScaleTable         ; IF (T0 != 0) Copy Scale Table
    subiu t0,1                     ; T0-- (Delay Slot)
.endmacro

; MDEC Read Data/Response
.macro ReadMDEC,SIZE,RAM           ; MDEC - Read Data/Response (32-Bit Words)
  li t0,SIZE-1                     ; T0 = Data Read Word Count
  la a1,RAM                        ; A1 = RAM Output Address
  ReadMDECWord:                    ; Read MDEC Word
    RDIOW MDECPAR,t1               ; Read I/O Port Register Word
    sw t1,0(a1)                    ; Write MDEC Word
    addiu a1,4                     ; A1 += 4 (Delay Slot)
    bnez t0,ReadMDECWord           ; IF (T0 != 0) Read MDEC Word
    subiu t0,1                     ; T0-- (Delay Slot)
.endmacro