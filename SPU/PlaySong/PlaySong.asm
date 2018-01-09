; PSX 'Bare Metal' SPU Play Song Demo by krom (Peter Lemon):
.psx
.create "PlaySong.bin", 0x80010000

.include "LIB/PSX.INC" ; Include PSX Definitions
.include "LIB/PSX_SPU.INC" ; Include PSX SPU Definitions & Macros

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup SPU Ready For DMA Transfer
WRIOH SPUDCNT,0x0004 ; Write Sound RAM Data Transfer Control Half (Sound RAM Data Transfer Type = Normal)
WRIOH SPUCNT,0x8000  ; Write SPU Control Half (SPU = ON, Sound RAM Transfer Mode = STOP)
SPUStop:
  RDIOH SPUSTAT,t0 ; Read SPU Status Register Half To T0
  andi t0,0x3F     ; T0 = SPU Mode (Bit 0..5)
  bnez t0,SPUStop  ; IF (SPU Mode != 0) SPU Stop
  nop ; Delay Slot

WRIOH SPUDBASE,0x1800 ; Write Sound RAM Data Transfer Address Half (Address = $C000)
WRIOH SPUCNT,0x8020   ; Write SPU Control Half (SPU = ON, Sound RAM Transfer Mode = DMA Write)

; DMA ADPCM Sample Data To SPU RAM
WRIOW D4_MADR,Sample     ; Write DMA SPU Base Address Word
WRIOW D4_BCR,0x00030004  ; Write DMA SPU Block Control Word (Blocksize = 4 Words (16 Bytes), Amount Of Blocks = Sample Byte Size / Blocksize)
WRIOW D4_CHCR,0x01000201 ; Write DMA SPU Channel Control Word (From Main RAM, Sync Blocks To DMA Requests, Start DMA)
WRIOW DPCR,0x00080000    ; Write DMA Control Register Word (DMA SPU Master Enable Bit 19 = 1)
DMABusy:
  RDIOH SPUSTAT,t0 ; Read SPU Status Register Half To T0
  andi t0,0x400    ; T0 = Data Transfer Busy Flag (Bit 10)
  bnez t0,DMABusy  ; IF (SPU DMA != 0) DMA Busy
  nop ; Delay Slot

; Initialize SPU
WRIOH SPUCNT,0xC080      ; Write SPU Control Half (SPU = ON, Unmute, Reverb Master Enabled, Sound RAM Transfer Mode = STOP)
CLIOW SPUKON             ; Clear Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word
WRIOW SPUKOFF,0x00FFFFFF ; Write Voice 0..23 Key OFF (Start Release) Word
CLIOW SPUPMON            ; Clear Voice 0..23 Pitch Modulation Enable Flags Word
CLIOW SPUNON             ; Clear Voice 0..23 Noise Mode Enable Flags Word
CLIOW SPUEON             ; Clear Voice 0..23 Channel Echo/Reverb Enable Flags Word
CLIOW SPUEVOL            ; Clear Echo/Reverb Output Volume Left/Right Word
CLIOW SPUKOFF            ; Clear Voice 0..23 Key OFF (Start Release) Word
WRIOW SPUMVOL,0x3FFF3FFF ; Write Main Volume Left/Right Word

; Setup Echo/Reverb
WRIOH SPUEBASE,0xF6F8      ; Write Sound RAM Echo/Reverb Work Area Start Address Half (Address = $7B7C0)
WRIOH SPUDAPF1,0x00B1      ; Write APF Offset 1 Half
WRIOH SPUDAPF2,0x007F      ; Write APF Offset 2 Half
WRIOH SPUVIIR,0x70F0       ; Write Reflection Volume 1 Half
WRIOH SPUVCOMB1,0x4FA8     ; Write Comb Volume 1 Half
WRIOH SPUVCOMB2,0xBCE0     ; Write Comb Volume 2 Half
WRIOH SPUVCOMB3,0x4510     ; Write Comb Volume 3 Half
WRIOH SPUVCOMB4,0xBEF0     ; Write Comb Volume 4 Half
WRIOH SPUVWALL,0xB4C0      ; Write Reflection Volume 2 Half
WRIOH SPUVAPF1,0x5280      ; Write APF Volume 1 Half
WRIOH SPUVAPF2,0x4EC0      ; Write APF Volume 2 Half
WRIOW SPUMSAME,0x076B0904  ; Write Same Side Reflection Address 1 Left/Right Word
WRIOW SPUMCOMB1,0x065F0824 ; Write Comb Address 1 Left/Right Word
WRIOW SPUMCOMB2,0x061607A2 ; Write Comb Address 2 Left/Right Word
WRIOW SPUDSAME,0x05ED076C  ; Write Same Side Reflection Address 2 Left/Right Word
WRIOW SPUMDIFF,0x042E05EC  ; Write Diff Side Reflection Address 1 Left/Right Word
WRIOW SPUMCOMB3,0x0305050F ; Write Comb Address 3 Left/Right Word
WRIOW SPUMCOMB4,0x02B70462 ; Write Comb Address 4 Left/Right Word
WRIOW SPUDDIFF,0x0265042F  ; Write Diff Side Reflection Address 2 Left/Right Word
WRIOW SPUMAPF1,0x01B20264  ; Write APF Address 1 Left/Right Word
WRIOW SPUMAPF2,0x00800100  ; Write APF Address 2 Left/Right Word
WRIOW SPUVIN,0x80008000    ; Write Input Volume Left/Right Word
WRIOW SPUEVOL,0x28002800   ; Write Echo/Reverb Output Volume Left/Right Word
WRIOW SPUEON,0x0000007F    ; Write Voice 0..23 Channel Echo/Reverb Enable Flags Word

; Setup Voices
WRIOH SPUBASE0,0x1800     ; Write Voice 0 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP0,0x1800+4   ; Write Voice 0 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL0,0x10B910B9  ; Write Voice 0 Volume Left/Right Word
WRIOW SPUADSR0,0x4CCC20FF ; Write Voice 0 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE1,0x1800     ; Write Voice 1 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP1,0x1800+4   ; Write Voice 1 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL1,0x10B910B9  ; Write Voice 1 Volume Left/Right Word
WRIOW SPUADSR1,0x4CCC20FF ; Write Voice 1 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE2,0x1800     ; Write Voice 2 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP2,0x1800+4   ; Write Voice 2 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL2,0x10B910B9  ; Write Voice 2 Volume Left/Right Word
WRIOW SPUADSR2,0x4CCC20FF ; Write Voice 2 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE3,0x1800     ; Write Voice 3 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP3,0x1800+4   ; Write Voice 3 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL3,0x10B910B9  ; Write Voice 3 Volume Left/Right Word
WRIOW SPUADSR3,0x4CCC20FF ; Write Voice 3 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE4,0x1800     ; Write Voice 4 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP4,0x1800+4   ; Write Voice 4 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL4,0x10B910B9  ; Write Voice 4 Volume Left/Right Word
WRIOW SPUADSR4,0x4CCC20FF ; Write Voice 4 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE5,0x1800     ; Write Voice 5 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP5,0x1800+4   ; Write Voice 5 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL5,0x10B910B9  ; Write Voice 5 Volume Left/Right Word
WRIOW SPUADSR5,0x4CCC20FF ; Write Voice 5 ADSR (Attack/Decay/Sustain/Release) Word

WRIOH SPUBASE6,0x1800     ; Write Voice 6 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP6,0x1800+4   ; Write Voice 6 ADPCM Repeat Address Half (Address = $C020)
WRIOW SPUVOL6,0x10B910B9  ; Write Voice 6 Volume Left/Right Word
WRIOW SPUADSR6,0x4CCC20FF ; Write Voice 6 ADSR (Attack/Decay/Sustain/Release) Word

SynthHarpC9Pitch equ 0xAA40

SongStart:
  SetPitch 0,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,APitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,APitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000


  SetPitch 0,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000


  SetPitch 0,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000


  SetPitch 0,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000


  SetPitch 0,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,APitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,APitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,GPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000


  SetPitch 0,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000001 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 0)
  SPUWait 0x400000

  SetPitch 1,FPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000002 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 1)
  SPUWait 0x400000

  SetPitch 2,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000004 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 2)
  SPUWait 0x400000

  SetPitch 3,EPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000008 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 3)
  SPUWait 0x400000

  SetPitch 4,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000010 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 4)
  SPUWait 0x400000

  SetPitch 5,DPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000020 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 5)
  SPUWait 0x400000

  SetPitch 6,CPitch,3,SynthHarpC9Pitch
  WRIOW SPUKON,0x00000040 ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word (Play Voice 6)
  SPUWait 0x800000

  b SongStart
  nop ; Delay Slot

Sample:
  .incbin "ADPCM/Synth Harp (Loop=4,ADSR=$4CCC20FF,Echo)(C9Freq=$AA40).adpcm" ; Include ADPCM Sample Data

.close