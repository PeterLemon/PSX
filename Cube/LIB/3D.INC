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
    LoadXYZ s0,s1,s2 ; S0 = 2D X, S1 = 2D Y

    sll s1,16      ; Y <<= 16
    andi s0,0xFFFF ; X &= 0xFFFF
    or s0,s1       ; X |= Y

    lw s1,0(a2) ; S1 = Color+Command
    addiu a2,4  ; A2 += 4
 
    sw s1,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex)

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro Line,START,END ; Line: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    lw s2,0(a2) ; S2 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s2,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriCullNone,START,END ; Texture Triangle Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s4,0(a2) ; S4 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriCullBack,START,END ; Texture Triangle Back Face Cull: Object Start Address, Object End Address
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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s4,0(a2) ; S4 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexTriCullFront,START,END ; Texture Triangle Front Face Cull: Object Start Address, Object End Address
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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s4,0(a2) ; S4 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s4,0(a2) ; S4 = Color2
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

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
    addiu a2,24  ; A2 += 24 (Delay Slot)
    subiu a2,24  ; A2 -= 24

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s4,0(a2) ; S4 = Color2
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

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
    addiu a2,24  ; A2 += 24 (Delay Slot)
    subiu a2,24  ; A2 -= 24

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    lw s3,0(a2) ; S3 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s4,0(a2) ; S4 = Color2
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord3)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex5)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex5)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex5)
    WRIOW GP0,0x55555555 ; Write GP0  Packet Word (Termination Code)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadCullNone,START,END ; Texture Quad Face Cull None: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3
    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s5,0(a2) ; S5 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadCullBack,START,END ; Texture Quad Back Face Cull: Object Start Address, Object End Address
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

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s5,0(a2) ; S5 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadCullFront,START,END ; Texture Quad Front Face Cull: Object Start Address, Object End Address
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

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s5,0(a2) ; S5 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

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

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color2
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color3
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw t0,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw t1,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw t2,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

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
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color2
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color3
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw t0,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw t1,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw t2,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

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
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    LoadXYZ s6,s7,s8 ; S6 = 2D X4, S7 = 2D Y4

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s3,16      ; Y2 <<= 16
    andi s2,0xFFFF ; X2 &= 0xFFFF
    or s1,s2,s3    ; X2 |= Y2

    sll s5,16      ; Y3 <<= 16
    andi s4,0xFFFF ; X3 &= 0xFFFF
    or s2,s4,s5    ; X3 |= Y3

    sll s7,16      ; Y4 <<= 16
    andi s6,0xFFFF ; X4 &= 0xFFFF
    or s3,s6,s7    ; X4 |= Y4

    lw s4,0(a2) ; S4 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s5,0(a2) ; S5 = Color2
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color3
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s8,GP0(a0) ; Write GP0  Packet Word (Texcoord1+Palette)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw t0,GP0(a0) ; Write GP0  Packet Word (Texcoord2+Texpage)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw t1,GP0(a0) ; Write GP0  Packet Word (Texcoord3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)
    sw t2,GP0(a0) ; Write GP0  Packet Word (Texcoord4)

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