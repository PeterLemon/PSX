; PSX 'Bare Metal' SPU Play ADPCM Sample Demo by krom (Peter Lemon):
.psx
.create "PlayADPCMSample.bin", 0

.include "LIB/PSX.INC" ; Include PSX Definitions

.org 0x80010000 ; Entry Point Of Code

la a0,IO_BASE ; A0 = I/O Port Base Address ($1F80XXXX)

; Setup SPU Ready For DMA Transfer
li t0,0x0004      ; T0 = Sound RAM Data Transfer Control Short (Sound RAM Data Transfer Type = Normal)
sh t0,SPUDCNT(a0) ; Sound RAM Data Transfer Control = T0

li t0,0x8000     ; T0 = SPU Control Short (SPU = ON, Sound RAM Transfer Mode = STOP)
sh t0,SPUCNT(a0) ; SPU Control Register

SPUStop:
  lhu t0,SPUSTAT(a0) ; T0 = SPU Status Register
  nop ; Delay Slot
  andi t0,0x3F       ; T0 = SPU Mode (Bit 0..5)
  bnez t0,SPUStop    ; IF (SPU Mode != 0) SPU Stop
  nop ; Delay Slot

li t0,0x1800       ; T0 = Sound RAM Data Transfer Address Short (Address = $C000)
sh t0,SPUDBASE(a0) ; Sound RAM Data Transfer Address = T0

li t0,0x8020     ; T0 = SPU Control Short (SPU = ON, Sound RAM Transfer Mode = DMA Write)
sh t0,SPUCNT(a0) ; SPU Control Register


; DMA ADPCM Sample Data To SPU RAM
la t0,Sample      ; T0 = Sample Address
sw t0,D4_MADR(a0) ; DMA SPU Base Address = T0

li t0,0x02CD0004 ; DMA Block Control Word (Blocksize = 4 Words (16 Bytes), Amount Of Blocks = Sample Byte Size / Blocksize)
sw t0,D4_BCR(a0) ; DMA SPU Block Control = T0

li t0,0x01000201  ; T0 = DMA Channel Control Word (From Main RAM, Sync Blocks To DMA Requests, Start DMA)
sw t0,D4_CHCR(a0) ; DMA SPU Channel Control = T0

li t0,0x00080000 ; DMA Control Register Word (DMA SPU Master Enable Bit 19 = 1)
sw t0,DPCR(a0)   ; DMA Control Register = T0

DMABusy:
  lhu t0,SPUSTAT(a0) ; T0 = SPU Status Register
  nop ; Delay Slot
  andi t0,0x400      ; T0 = Data Transfer Busy Flag (Bit 10)
  bnez t0,DMABusy    ; IF (SPU DMA != 0) DMA Busy
  nop ; Delay Slot


; Play ADPCM Sample
li t0,0x8000     ; T0 = SPU Control Short (SPU = ON, Reverb Disabled, Sound RAM Transfer Mode = STOP)
sh t0,SPUCNT(a0) ; SPU Control Register

sw r0,SPUKON(a0)  ; Voice 0..23 Key ON (Start Attack/Decay/Sustain) = 0

li t0,0x00FFFFFF  ; Set Key Off Flags
sw t0,SPUKOFF(a0) ; Voice 0..23 Key OFF (Start Release) = T0

sw r0,SPUPMON(a0) ; Voice 0..23 Pitch Modulation Enable Flags = 0
sw r0,SPUNON(a0)  ; Voice 0..23 Noise Mode Enable Flags = 0
sw r0,SPUEON(a0)  ; Voice 0..23 Channel Echo/Reverb Enable Flags = 0
sw r0,SPUEVOL(a0) ; Echo/Reverb Output Volume Left/Right = 0

li t0,0x1800       ; T0 = SPU RAM ADPCM Sample Start Address
sh t0,SPUBASE0(a0) ; Voice 0 ADPCM Start Address = T0

li t0,(0x1800+718) ; T0 = SPU RAM ADPCM Sample Repeat Address
sh t0,SPULOOP0(a0) ; Voice 0 ADPCM Repeat Address = T0

sw r0,SPUKOFF(a0) ; Voice 0..23 Key OFF (Start Release) = 0

li t0,0x10B910B9  ; T0 = Main Volume Left/Right
sw t0,SPUMVOL(a0) ; Main Volume Left/Right = T0

li t0,0x10B910B9  ; T0 = Voice 0 Volume Left/Right
sw t0,SPUVOL0(a0) ; Voice 0 Volume Left/Right = T0

li t0,0x0800      ; T0 = Voice 0 ADPCM Sample Rate (22050Hz)
sh t0,SPUFRQ0(a0) ; Voice 0 ADPCM Sample Rate = T0

li t0,0x5FC528FF   ; T0 = Voice 0  ADSR (Attack/Decay/Sustain/Release)
sw t0,SPUADSR0(a0) ; Voice 0  ADSR (Attack/Decay/Sustain/Release) = T0

li t0,0x00000001  ; T0 = Voice 0  ADSR (Attack/Decay/Sustain/Release)
sw t0,SPUKON(a0)  ; Voice 0..23 Key ON (Start Attack/Decay/Sustain) = 1

Loop:
  b Loop
  nop ; Delay Slot

Sample:
  .incbin "sample.adpcm" ; Include ADPCM Sample Data

.close