;=========
; PSX SPU
;=========
; SPU Macros
.macro SPUWait,AMOUNT ; Wait For Amount
  li t0,AMOUNT
  @@Wait:
    bnez t0,@@Wait ; IF (T0 != 0) Loop Wait
    subiu t0,1 ; T0-- (Delay Slot)
.endmacro

CSharpPitch equ 11
DPitch equ 10
DSharpPitch equ 9
EPitch equ 8
FPitch equ 7
FSharpPitch equ 6
GPitch equ 5
GSharpPitch equ 4
APitch equ 3
ASharpPitch equ 2
BPitch equ 1
CPitch equ 0

.macro SetPitch,voice,note,octave,C9Pitch
  .if note == CPitch
    WRIOH SPUFRQ0+(0x10*voice),C9Pitch>>(9-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == BPitch
    WRIOH SPUFRQ0+(0x10*voice),((C9Pitch*1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == ASharpPitch
    WRIOH SPUFRQ0+(0x10*voice),((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == APitch
    WRIOH SPUFRQ0+(0x10*voice),(((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == GSharpPitch
    WRIOH SPUFRQ0+(0x10*voice),((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == GPitch
    WRIOH SPUFRQ0+(0x10*voice),(((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == FSharpPitch
    WRIOH SPUFRQ0+(0x10*voice),((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == FPitch
    WRIOH SPUFRQ0+(0x10*voice),(((((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == EPitch
    WRIOH SPUFRQ0+(0x10*voice),((((((((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == DSharpPitch
    WRIOH SPUFRQ0+(0x10*voice),(((((((((((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == DPitch
    WRIOH SPUFRQ0+(0x10*voice),((((((((((((((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .elseif note == CSharpPitch
    WRIOH SPUFRQ0+(0x10*voice),(((((((((((((((((((((((((((((((C9Pitch*1000000000)/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000))/1059463094)\
                                        *1000000000)/1059463094)>>(8-octave) ; Write Voice 0..23 ADPCM Sample Rate Half
  .endif
.endmacro