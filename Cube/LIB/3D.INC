.macro LoadXYZ,X,Y,Z ; Load X,Y,Z: X,Y,Z
  lw t0,0(a2) ; T0 = X
  lw t1,4(a2) ; T1 = Y
  lw t2,8(a2) ; T2 = Z
  addiu a2,12 ; A2 += 12
  Calc3D t0,t1,t2,X,Y,Z
.endmacro

.macro Calc3D,X,Y,Z,XCALC,YCALC,ZCALC ; Calculate X,Y,Z 3D: X,Y,Z,XCalc,YCalc,ZCalc
  la a1,Matrix3D ; Load Matrix3D Address
  lw t3,0(a1) ; XCalc = (Matrix3D[0] * X) + (Matrix3D[1] * Y) + (Matrix3D[2] * Z) + Matrix3D[3]
  lw t4,4(a1)
  lw t5,8(a1)
  lw t6,12(a1)
  mult t3,x
  mflo XCALC
  mult t4,y
  mflo t3
  add XCALC,t3
  mult t5,z
  mflo t3
  add XCALC,t3
  sll t6,8
  add XCALC,t6
  lw t3,16(a1) ; YCalc = (Matrix3D[4] * X) + (Matrix3D[5] * Y) + (Matrix3D[6] * Z) + Matrix3D[7]
  lw t4,20(a1)
  lw t5,24(a1)
  lw t6,28(a1)
  mult t3,x
  mflo YCALC
  mult t4,y
  mflo t3
  add YCALC,t3
  mult t5,z
  mflo t3
  add YCALC,t3
  sll t6,8
  add YCALC,t6
  lw t3,32(a1) ; ZCalc = (Matrix3D[8] * X) + (Matrix3D[9] * Y) + (Matrix3D[10] * Z) + Matrix3D[11]
  lw t4,36(a1)
  lw t5,40(a1)
  lw t6,44(a1)
  mult t3,x
  mflo ZCALC
  mult t4,y
  mflo t3
  add ZCALC,t3
  mult t5,z
  mflo t3
  add ZCALC,t3
  sll t6,8
  add ZCALC,t6

  Calc2D XCALC,YCALC,ZCALC
.endmacro

.macro Calc2D,X,Y,Z ; Calculate X,Y 2D: X,Y,Z
  beqz Z,Skip ; IF (Z <= 0) Skip
  sra Z,8 ; Z /= 256 (Delay Slot)

  div X,Z ; X = X / Z + (ScreenX / 2)
  mflo X
  addi X,320/2

  div Y,Z ; Y = Y / Z + (ScreenY / 2)
  mflo Y
  addi Y,240/2

  Skip:
.endmacro

.macro CullFace,X1,Y1,X2,Y2,X3,Y3 ; Cull Face Calculation: X1,Y1,X2,Y2,X3,Y3
  sub t0,X3,X1 ; T0 = HDX (X3 - X1)
  sub t1,Y2,Y1 ; T1 = MDY (Y2 - Y1)
  mult t0,t1 ; T0 = HDX * MDY
  mflo t0

  sub t1,Y3,Y1 ; T1 = HDY (Y3 - Y1)
  sub t2,X2,X1 ; T2 = MDX (X2 - X1)
  mult t1,t2 ; T1 = HDY * MDX
  mflo t1

  sub t0,t1 ; T0 = R (HDX * MDY) - (HDY * MDX) Compare To Zero
.endmacro

.macro Point,START,END ; Point: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1

    lw s2,0(a2) ; S2 = Face Color
    addiu a2,4 ; A2 += 4

    FillRect1x1V s2,s0,s1 ; Fill Rectangle 1x1: Color,X,Y

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro Line,START,END ; Line: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2

    lw s4,0(a2) ; S4 = Face Color
    addiu a2,4 ; A2 += 4

    FillLineV s4,s0,s1,s2,s3 ; Fill Line: Color,X1,Y1,X2,Y2

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireTriCullNone,START,END ; Wire Triangle Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriLineV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle Line: Color,X1,Y1,X2,Y2,X3,Y3

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireTriCullBack,START,END ; Wire Triangle Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,4 ; A2 += 4 (Delay Slot)
    subiu a2,4 ; A2 -= 4

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriLineV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle Line: Color,X1,Y1,X2,Y2,X3,Y3

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireTriCullFront,START,END ; Wire Triangle Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,4 ; A2 += 4 (Delay Slot)
    subiu a2,4 ; A2 -= 4

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriLineV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle Line: Color,X1,Y1,X2,Y2,X3,Y3

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillTriCullNone,START,END ; Fill Triangle Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle: Color,X1,Y1,X2,Y2,X3,Y3

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillTriCullBack,START,END ; Fill Triangle Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,4 ; A2 += 4 (Delay Slot)
    subiu a2,4 ; A2 -= 4

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle: Color,X1,Y1,X2,Y2,X3,Y3

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillTriCullFront,START,END ; Fill Triangle Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,4 ; A2 += 4 (Delay Slot)
    subiu a2,4 ; A2 -= 4

    lw s6,0(a2) ; S6 = Face Color
    addiu a2,4 ; A2 += 4

    FillTriV s6,s0,s1,s2,s3,s4,s5 ; Fill Triangle: Color,X1,Y1,X2,Y2,X3,Y3

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriRawCullNone,START,END ; Texture Triangle Raw Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    TexTriRawV s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriRawCullBack,START,END ; Texture Triangle Raw Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,32  ; A2 += 32 (Delay Slot)
    subiu a2,32  ; A2 -= 32

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    TexTriRawV s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriRawCullFront,START,END ; Texture Triangle Raw Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,32  ; A2 += 32 (Delay Slot)
    subiu a2,32  ; A2 -= 32

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    TexTriRawV s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriBlendCullNone,START,END ; Texture Triangle Blend Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR
    addiu a2,4  ; A2 += 4

    TexTriBlendV t6,s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriBlendCullBack,START,END ; Texture Triangle Blend Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,36  ; A2 += 36 (Delay Slot)
    subiu a2,36  ; A2 -= 36

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR
    addiu a2,4  ; A2 += 4

    TexTriBlendV t6,s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriBlendCullFront,START,END ; Texture Triangle Blend Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,36  ; A2 += 36 (Delay Slot)
    subiu a2,36  ; A2 -= 36

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR
    addiu a2,4  ; A2 += 4

    TexTriBlendV t6,s0,s1,s6,s7,t4,s2,s3,s8,t1,t5,s4,s5,t2,t3 ; Texture Triangle Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexTriCullNone,START,END ; Shaded Texture Triangle Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR1
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = COLOR2
    addiu a2,4  ; A2 += 4
    lw t8,0(a2) ; T8 = COLOR3
    addiu a2,4  ; A2 += 4

    ShadeTexTriV t6,s0,s1,s6,s7,t4,t7,s2,s3,s8,t1,t5,t8,s4,s5,t2,t3 ; Shaded Texture Triangle: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexTriCullBack,START,END ; Shaded Texture Triangle Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR1
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = COLOR2
    addiu a2,4  ; A2 += 4
    lw t8,0(a2) ; T8 = COLOR3
    addiu a2,4  ; A2 += 4

    ShadeTexTriV t6,s0,s1,s6,s7,t4,t7,s2,s3,s8,t1,t5,t8,s4,s5,t2,t3 ; Shaded Texture Triangle: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexTriCullFront,START,END ; Shaded Texture Triangle Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    lw s6,0(a2) ; S6 = U1
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = V1
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = U2
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V2
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U3
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V3
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = PAL
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = TEX
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = COLOR1
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = COLOR2
    addiu a2,4  ; A2 += 4
    lw t8,0(a2) ; T8 = COLOR3
    addiu a2,4  ; A2 += 4

    ShadeTexTriV t6,s0,s1,s6,s7,t4,t7,s2,s3,s8,t1,t5,t8,s4,s5,t2,t3 ; Shaded Texture Triangle: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireQuadCullNone,START,END ; Wire Quad Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadLineV s8,s0,s1,s2,s3,s6,s7,s4,s5 ; Fill Quad Line: X1,Y1,X2,Y2,X3,Y3,X4,Y4,Color

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireQuadCullBack,START,END ; Wire Quad Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadLineV s8,s0,s1,s2,s3,s6,s7,s4,s5 ; Fill Quad Line: X1,Y1,X2,Y2,X3,Y3,X4,Y4,Color

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro WireQuadCullFront,START,END ; Wire Quad Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadLineV s8,s0,s1,s2,s3,s6,s7,s4,s5 ; Fill Quad Line: X1,Y1,X2,Y2,X3,Y3,X4,Y4,Color

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillQuadCullNone,START,END ; Fill Quad Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadV s8,s0,s1,s2,s3,s4,s5,s6,s7 ; Fill Quad: Color,X1,Y1,X2,Y2,X3,Y3,X4,Y4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillQuadCullBack,START,END ; Fill Quad Face Cull Back: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadV s8,s0,s1,s2,s3,s4,s5,s6,s7 ; Fill Quad: Color,X1,Y1,X2,Y2,X3,Y3,X4,Y4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro FillQuadCullFront,START,END ; Fill Quad Face Cull Front: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = Face Color
    addiu a2,4 ; A2 += 4

    FillQuadV s8,s0,s1,s2,s3,s4,s5,s6,s7 ; Fill Quad: Color,X1,Y1,X2,Y2,X3,Y3,X4,Y4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadRawCullNone,START,END ; Texture Quad Raw Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    TexQuadRawV s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadRawCullBack,START,END ; Texture Quad Raw Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,52  ; A2 += 52 (Delay Slot)
    subiu a2,52  ; A2 -= 52

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    TexQuadRawV s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadRawCullFront,START,END ; Texture Quad Raw Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,52  ; A2 += 52 (Delay Slot)
    subiu a2,52  ; A2 -= 52

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    TexQuadRawV s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad RAW: X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadBlendCullNone,START,END ; Texture Quad Blend Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR
    addiu a2,4  ; A2 += 4

    TexQuadBlendV k0,s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadBlendCullBack,START,END ; Texture Quad Blend Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,56  ; A2 += 56 (Delay Slot)
    subiu a2,56  ; A2 -= 56

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR
    addiu a2,4  ; A2 += 4

    TexQuadBlendV k0,s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadBlendCullFront,START,END ; Texture Quad Blend Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,56  ; A2 += 56 (Delay Slot)
    subiu a2,56  ; A2 -= 56

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR
    addiu a2,4  ; A2 += 4

    TexQuadBlendV k0,s0,s1,s8,t1,t8,s2,s3,t2,t3,t9,s4,s5,t4,t5,s6,s7,t6,t7 ; Texture Quad Blend: Color,X1,Y1,U1,V1,Palette,X2,Y2,U2,V2,Texpage,X3,Y3,U3,V3,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexQuadCullNone,START,END ; Shaded Texture Quad Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR1
    addiu a2,4  ; A2 += 4
    lw k1,0(a2) ; K1 = COLOR2
    addiu a2,4  ; A2 += 4
    lw at,0(a2) ; AT = COLOR3
    addiu a2,4  ; A2 += 4
    lw v0,0(a2) ; V0 = COLOR4
    addiu a2,4  ; A2 += 4

    ShadeTexQuadV k0,s0,s1,s8,t1,t8,k1,s2,s3,t2,t3,t9,at,s4,s5,t4,t5,v0,s6,s7,t6,t7 ; Shaded Texture Quad: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3,Color4,X4,Y4,U4,V4 

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexQuadCullBack,START,END ; Shaded Texture Quad Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,68  ; A2 += 68 (Delay Slot)
    subiu a2,68  ; A2 -= 68

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR1
    addiu a2,4  ; A2 += 4
    lw k1,0(a2) ; K1 = COLOR2
    addiu a2,4  ; A2 += 4
    lw at,0(a2) ; AT = COLOR3
    addiu a2,4  ; A2 += 4
    lw v0,0(a2) ; V0 = COLOR4
    addiu a2,4  ; A2 += 4

    ShadeTexQuadV k0,s0,s1,s8,t1,t8,k1,s2,s3,t2,t3,t9,at,s4,s5,t4,t5,v0,s6,s7,t6,t7 ; Shaded Texture Quad: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3,Color4,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexQuadCullFront,START,END ; Shaded Texture Quad Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,68  ; A2 += 68 (Delay Slot)
    subiu a2,68  ; A2 -= 68

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    lw s8,0(a2) ; S8 = U1
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = V1
    addiu a2,4  ; A2 += 4

    lw t2,0(a2) ; T2 = U2
    addiu a2,4  ; A2 += 4
    lw t3,0(a2) ; T3 = V2
    addiu a2,4  ; A2 += 4

    lw t4,0(a2) ; T4 = U3
    addiu a2,4  ; A2 += 4
    lw t5,0(a2) ; T5 = V3
    addiu a2,4  ; A2 += 4

    lw t6,0(a2) ; T6 = U4
    addiu a2,4  ; A2 += 4
    lw t7,0(a2) ; T7 = V4
    addiu a2,4  ; A2 += 4

    lw t8,0(a2) ; T8 = PAL
    addiu a2,4  ; A2 += 4
    lw t9,0(a2) ; T9 = TEX
    addiu a2,4  ; A2 += 4

    lw k0,0(a2) ; K0 = COLOR1
    addiu a2,4  ; A2 += 4
    lw k1,0(a2) ; K1 = COLOR2
    addiu a2,4  ; A2 += 4
    lw at,0(a2) ; AT = COLOR3
    addiu a2,4  ; A2 += 4
    lw v0,0(a2) ; V0 = COLOR4
    addiu a2,4  ; A2 += 4

    ShadeTexQuadV k0,s0,s1,s8,t1,t8,k1,s2,s3,t2,t3,t9,at,s4,s5,t4,t5,v0,s6,s7,t6,t7 ; Shaded Texture Quad: Color1,X1,Y1,U1,V1,Palette,Color2,X2,Y2,U2,V2,Texpage,Color3,X3,Y3,U3,V3,Color4,X4,Y4,U4,V4 

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro XPos,X ; Object X Translation: X
  la a1,X ; Load X Translation Value Address
  lw t0,0(a1) ; Load X Translation Value
  la a1,Matrix3D ; Load Matrix3D Address
  sw t0,12(a1) ; Matrix3D[3] = (Translation X)
.endmacro

.macro YPos,Y ; Object Y Translation: Y
  la a1,Y ; Load Y Translation Value Address
  lw t0,0(a1) ; Load Y Translation Value
  la a1,Matrix3D ; Load Matrix3D Address
  sw t1,28(a1) ; Matrix3D[7] = (Translation Y)
.endmacro

.macro ZPos,Z ; Object Z Translation: Z
  la a1,Z ; Load Z Translation Value Address
  lw t0,0(a1) ; Load Z Translation Value
  la a1,Matrix3D ; Load Matrix3D Address
  sw t2,44(a1) ; Matrix3D[11] = (Translation Z)
.endmacro

.macro XYZPos,X,Y,Z ; Object X,Y,Z Translation: X,Y,Z
  la a1,X ; Load X Translation Value Address
  lw t0,0(a1) ; Load X Translation Value
  la a1,Y ; Load Y Translation Value Address
  lw t1,0(a1) ; Load Y Translation Value
  la a1,Z ; Load Z Translation Value Address
  lw t2,0(a1) ; Load Z Translation Value
  la a1,Matrix3D ; Load Matrix3D Address
  sw t0,12(a1) ; Matrix3D[3]  = (Translation X)
  sw t1,28(a1) ; Matrix3D[7]  = (Translation Y)
  sw t2,44(a1) ; Matrix3D[11] = (Translation Z)
.endmacro

.macro XRotCalc,XROT,PRECALC ; X Rotation Calculation: X Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,XROT ; Load X Rotation Address
  lw t0,0(a1) ; T0 = X Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add X Rotation * 16 To X Rotate Matrix Offset
  lw t0,0(a1) ; Load  XC
  lw t1,4(a1) ; Load -XS
  lw t2,8(a1) ; Load  XS

  la a1,Matrix3D ; Load Matrix3D Address
  sw t0,20(a1) ; Store T0  (XC) To Matrix3D  6th Word
  sw t1,24(a1) ; Store T1 (-XS) To Matrix3D  7th Word
  sw t2,36(a1) ; Store T2  (XS) To Matrix3D 10th Word
  sw t0,40(a1) ; Store T0  (XC) To Matrix3D 11th Word
.endmacro

.macro YRotCalc,YROT,PRECALC ; Y Rotation Calculation: Y Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,YROT ; Load Y Rotation Address
  lw t0,0(a1) ; T0 = Y Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add Y Rotation * 16 To Y Rotate Matrix Offset
  lw t0,0(a1) ; Load  YC
  lw t1,4(a1) ; Load -YS
  lw t2,8(a1) ; Load  YS

  la a1,Matrix3D ; Load Matrix3D Address
  sw t0,0(a1)  ; Store T0  (YC) To Matrix3D  1st Word
  sw t2,8(a1)  ; Store T1  (YS) To Matrix3D  3rd Word
  sw t1,32(a1) ; Store T2 (-YS) To Matrix3D  9th Word
  sw t0,40(a1) ; Store T0  (YC) To Matrix3D 11th Word
.endmacro

.macro ZRotCalc,ZROT,PRECALC ; Z Rotation Calculation: Z Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,ZROT ; Load Z Rotation Address
  lw t0,0(a1) ; T0 = Z Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add Z Rotation * 16 To Z Rotate Matrix Offset
  lw t0,0(a1) ; Load  ZC
  lw t1,4(a1) ; Load -ZS
  lw t2,8(a1) ; Load  ZS

  la a1,Matrix3D ; Load Matrix3D Address
  sw t0,0(a1)  ; Store T0  (ZC) To Matrix3D 1st Word
  sw t1,4(a1)  ; Store T1 (-ZS) To Matrix3D 2nd Word
  sw t2,16(a1) ; Store T2  (ZS) To Matrix3D 5th Word
  sw t0,20(a1) ; Store T0  (ZC) To Matrix3D 6th Word
.endmacro

.macro XYRotCalc,XROT,YROT,PRECALC ; XY Rotation Calculation: X Rotation, Y Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,XROT ; Load X Rotation Address
  lw t0,0(a1) ; T0 = X Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add X Rotation * 16 To X Rotate Matrix Offset
  lw t0,0(a1)  ; Load  XC
  lw t1,4(a1)  ; Load -XS
  lw t2,8(a1)  ; Load  XS
  lw t3,12(a1) ; Load -XC

  la a1,YROT ; Load Y Rotation Address
  lw t4,0(a1) ; T4 = Y Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t4,4 ; T4 *= 16
  addu a1,t4 ; Add Y Rotation * 16 To Y Rotate Matrix Offset
  lw t4,0(a1) ; Load  YC
  lw t5,4(a1) ; Load -YS
  lw t6,8(a1) ; Load  YS

  la a1,Matrix3D ; Load Matrix3D Address
  sw t4,0(a1) ; Store T4 (YC) To Matrix3D 1st Word
  mult t2,t6 ; Load (XS * YS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,4(a1) ; Store T7 (XS * YS) To Matrix3D 2nd Word
  mult t3,t6 ; Load (-XC * YS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,8(a1) ; Store T7 (-XC * YS) To Matrix3D 3rd Word
  sw t0,20(a1) ; Store T0 (XC) To Matrix3D 6th Word
  sw t2,24(a1) ; Store T2 (XS) To Matrix3D 7th Word
  sw t6,32(a1) ; Store T6 (YS) To Matrix3D 9th Word
  mult t1,t4 ; Load (-XS * YC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,36(a1) ; Store T7 (-XS * YC) To Matrix3D 10th Word
  mult t0,t4 ; Load (XC * YC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,40(a1) ; Store T7 (XC * YC) To Matrix3D 11th Word
.endmacro

.macro XZRotCalc,XROT,ZROT,PRECALC ; XZ Rotation Calculation: X Rotation, Z Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,XROT ; Load X Rotation Address
  lw t0,0(a1) ; T0 = X Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add X Rotation * 16 To X Rotate Matrix Offset
  lw t0,0(a1) ; Load  XC
  lw t1,4(a1) ; Load -XS
  lw t2,8(a1) ; Load  XS

  la a1,ZROT ; Load Z Rotation Address
  lw t3,0(a1) ; T3 = Z Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t3,4 ; T3 *= 16
  addu a1,t3 ; Add Z Rotation * 16 To Z Rotate Matrix Offset
  lw t3,0(a1)  ; Load  ZC
  lw t4,4(a1)  ; Load -ZS
  lw t5,8(a1)  ; Load  ZS
  lw t6,12(a1) ; Load -ZC

  la a1,Matrix3D ; Load Matrix3D Address
  sw t3,0(a1) ; Store T3 (ZC) To Matrix3D 1st Word
  sw t5,4(a1) ; Store T5 (ZS) To Matrix3D 2nd Word
  mult t0,t4 ; Load (XC * -ZS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,16(a1) ; Store T7 (XC * -ZS) To Matrix3D 5th Word
  mult t0,t3 ; Load (XC * ZC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,20(a1) ; Store T7 (XC * ZC) To Matrix3D 6th Word
  sw t2,24(a1) ; store T2 (XS) To Matrix3D 7th Word
  mult t2,t5 ; Load (XS * ZS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,32(a1) ; Store T7 (XS * ZS) To Matrix3D 9th Word
  mult t2,t6 ; Load (XS * -ZC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,36(a1) ; Store T7 (XS * -ZC) To Matrix3D 10th Word
  sw t0,40(a1) ; Store T0 (XC) To Matrix3D 11th Word
.endmacro

.macro YZRotCalc,YROT,ZROT,PRECALC ; YZ Rotation Calculation: Y Rotation, Z Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,YROT ; Load Y Rotation Address
  lw t0,0(a1) ; T0 = Y Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add Y Rotation * 16 To Y Rotate Matrix Offset
  lw t0,0(a1) ; Load  YC
  lw t1,4(a1) ; Load -YS
  lw t2,8(a1) ; Load  YS

  la a1,ZROT ; Load Z Rotation Address
  lw t3,0(a1) ; T3 = Z Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t3,4 ; T3 *= 16
  addu a1,t3 ; Add Z Rotation * 16 To Z Rotate Matrix Offset
  lw t3,0(a1)  ; Load  ZC
  lw t4,4(a1)  ; Load -ZS
  lw t5,8(a1)  ; Load  ZS
  lw t6,12(a1) ; Load -ZC

  la a1,Matrix3D ; Load Matrix3D Address
  mult t0,t3 ; Load (YC * ZC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,0(a1) ; Store T7 (YC * ZC) To Matrix3D 1st Word
  sw t5,4(a1) ; Store T5 (ZS) To Matrix3D 2nd Word
  mult t2,t6 ; Load (YS * -ZC) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,8(a1) ; Store T7 (YS * -ZC) To Matrix3D 3rd Word
  mult t0,t4 ; Load (YC * -ZS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,16(a1) ; Store T7 (YC * -ZS) To Matrix3D 5th Word
  sw t3,20(a1) ; Store T3 (ZC) To Matrix3D 6th Word
  mult t2,t5 ; Load (YS * ZS) To T7
  mflo t7
  sra t7,8 ; Divide T7 By 256
  sw t7,24(a1) ; Store T7 (YS * ZS) To Matrix3D 7th Word
  sw t2,32(a1) ; Store T2 (YS) To Matrix3D 9th Word
  sw t0,40(a1) ; Store T0 (YC) To Matrix3D 11th Word
.endmacro

.macro XYZRotCalc,XROT,YROT,ZROT,PRECALC ; XYZ Rotation Calculation: X Rotation, Y Rotation, Z Rotation, Matrix Sin & Cos Pre-Calculated Table
  la a1,XROT ; Load X Rotation Address
  lw t0,0(a1) ; T0 = X Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t0,4 ; T0 *= 16
  addu a1,t0 ; Add X Rotation * 16 To X Rotate Matrix Offset
  lw t0,0(a1)  ; Load  XC
  lw t1,4(a1)  ; Load -XS
  lw t2,8(a1)  ; Load  XS
  lw t3,12(a1) ; Load -XC

  la a1,YROT ; Load Y Rotation Address
  lw t4,0(a1) ; T4 = Y Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t4,4 ; T4 *= 16
  addu a1,t4 ; Add Y Rotation * 16 To Y Rotate Matrix Offset
  lw t4,0(a1) ; Load  YC
  lw t5,4(a1) ; Load -YS
  lw t6,8(a1) ; Load  YS

  la a1,ZROT ; Load Z Rotation Address
  lw t7,0(a1) ; T7 = Z Rotation
  la a1,PRECALC ; Load Matrix Sin & Cos Pre-Calculated Table Address
  sll t7,4 ; T7 *= 16
  addu a1,t7 ; Add Z Rotation * 16 To Z Rotate Matrix Offset
  lw t7,0(a1) ; Load  ZC
  lw t8,4(a1) ; Load -ZS
  lw t9,8(a1) ; Load  ZS

  la a1,Matrix3D ; Load Matrix3D Address
  mult t0,t4 ; Load (XC * YC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult s0,t7 ; Load (XC * YC * ZC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult t2,t9 ; Load (XS * ZS) To S1
  mflo s1
  sra s1,8 ; Divide S1 By 256
  sub s0,s1
  sw s0,0(a1) ; Store S0 (XC * YC * ZC) - (XS * ZS) To Matrix3D 1st Word

  mult t0,t4 ; Load (XC * YC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult s0,t9 ; Load (XC * YC * ZS) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult t2,t7 ; Load (XS * ZC) To S1
  mflo s1
  sra s1,8 ; Divide S1 By 256
  add s0,s1
  sw s0,4(a1) ; Store S0 (XC * YC * ZS) + (XS * ZC) To Matrix3D 2nd Word

  mult t3,t6 ; Load (-XC * YS) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  sw s0,8(a1) ; Store S0 (-XC * YS) To Matrix3D 3rd Word

  mult t1,t4 ; Load (-XS * YC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult s0,t7 ; Load (-XS * YC * ZC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult t0,t9 ; Load (XC * ZS) To S1
  mflo s1
  sra s1,8 ; Divide S1 By 256
  sub s0,s1
  sw s0,16(a1) ; Store S0 (-XS * YC * ZC) - (XC * ZS) To Matrix3D 5th Word

  mult t1,t4 ; Load (-XS * YC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult s0,t9 ; Load (-XS * YC * ZS) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  mult t0,t7 ; Load (XC * ZC) To S1
  mflo s1
  sra s1,8 ; Divide S1 By 256
  add s0,s1
  sw s0,20(a1) ; Stores S0 (-XS * YC * ZS) + (XC * ZC) To Matrix3D 6th Word

  mult t2,t6 ; Load (XS * YS) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  sw s0,24(a1) ; Store S0 (XS * YS) To Matrix3D 7th Word
  mult t6,t7 ; Load (YS * ZC) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  sw s0,32(a1) ; Store S0 (YS * ZC) To Matrix3D 9th Word
  mult t6,t9 ; Load (YS * ZS) To S0
  mflo s0
  sra s0,8 ; Divide S0 By 256
  sw s0,36(a1) ; Store S0 (YS * ZS) To Matrix3D 10th Word
  sw t4,40(a1) ; Store T4 (YC) To Matrix3D 11th Word
.endmacro