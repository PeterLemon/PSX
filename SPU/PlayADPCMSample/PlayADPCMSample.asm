; PSX 'Bare Metal' SPU Play ADPCM Sample Demo by krom (Peter Lemon):
.psx
.create "PlayADPCMSample.bin", 0

.include "LIB/PSX.INC" ; Include PSX Definitions

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
WRIOW D4_BCR,0x02CD0004  ; Write DMA SPU Block Control Word (Blocksize = 4 Words (16 Bytes), Amount Of Blocks = Sample Byte Size / Blocksize)
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
WRIOW SPUEON,0x00000001    ; Write Voice 0..23 Channel Echo/Reverb Enable Flags Word

; Play ADPCM Sample
WRIOH SPUBASE0,0x1800     ; Write Voice 0 ADPCM  Start Address Half (Address = $C000)
WRIOH SPULOOP0,0x1800+718 ; Write Voice 0 ADPCM Repeat Address Half (Address = $D670)
WRIOW SPUVOL0,0x10B910B9  ; Write Voice 0 Volume Left/Right Word
WRIOH SPUFRQ0,0x0800      ; Write Voice 0 ADPCM Sample Rate Half
WRIOW SPUADSR0,0x5FC528FF ; Write Voice 0 ADSR (Attack/Decay/Sustain/Release) Word
WRIOW SPUKON,0x00000001   ; Write Voice 0..23 Key ON (Start Attack/Decay/Sustain) Word

Loop:
  b Loop
  nop ; Delay Slot

Sample:
  .incbin "sample.adpcm" ; Include ADPCM Sample Data

.close