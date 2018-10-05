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

.macro AverageZTri,Z1,Z2,Z3 ; Average Z Triangle: Z1,Z2,Z3
  add t0,Z1,Z2 ; T0 = Z1+Z2
  add t0,Z3 ; T0 = Z1+Z2+Z3
.endmacro

.macro AverageZQuad,Z1,Z2,Z3,Z4 ; Average Z Quad: Z1,Z2,Z3,Z4
  add t0,Z1,Z2 ; T0 = Z1+Z2
  add t0,Z3 ; T0 = Z1+Z2+Z3
  add t0,Z4 ; T0 = Z1+Z2+Z3+Z4
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

.macro PointZSort,START,END,SORT ; Point Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X, S1 = 2D Y, S2 = 3D Z

    sll s1,16      ; Y <<= 16
    andi s0,0xFFFF ; X &= 0xFFFF
    or s0,s1       ; X |= Y

    sw s2,0(v0) ; Store Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s1,0(a2) ; S1 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s1,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s0,0(v0) ; Store Vertex To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,12 ; A3 += 12

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,12 ; A2 += 12 (Delay Slot)
    subiu a2,12 ; A2 -= 12

    li t0,(12/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(8/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
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

.macro LineZSort,START,END,SORT ; Line Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1, S2 = 3D Z1
    LoadXYZ s3,s4,s5 ; S3 = 2D X2, S4 = 2D Y2, S5 = 3D Z2

    sll s1,16      ; Y1 <<= 16
    andi s0,0xFFFF ; X1 &= 0xFFFF
    or s0,s1       ; X1 |= Y1

    sll s4,16      ; Y2 <<= 16
    andi s3,0xFFFF ; X2 &= 0xFFFF
    or s1,s3,s4    ; X2 |= Y2

    add s2,s5   ; Z1 += Z2
    sw s2,0(v0) ; Store Z1+Z2 To Sort Address
    addiu v0,4  ; V0 += 4

    lw s2,0(a2) ; S2 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s2,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s0,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,16 ; A3 += 16

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,16 ; A2 += 16 (Delay Slot)
    subiu a2,16 ; A2 -= 16

    li t0,(16/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(12/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
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

.macro WireTriCullNoneZSort,START,END,SORT ; Wire Triangle Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro WireTriCullBackZSort,START,END,SORT ; Wire Triangle Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,4   ; A2 += 4 (Delay Slot)
    subiu a2,4   ; A2 -= 4

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro WireTriCullFrontZSort,START,END,SORT ; Wire Triangle Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,4   ; A2 += 4 (Delay Slot)
    subiu a2,4   ; A2 -= 4

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro FillTriCullNoneZSort,START,END,SORT ; Fill Triangle Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,20 ; A3 += 20

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,20 ; A2 += 20 (Delay Slot)
    subiu a2,20 ; A2 -= 20

    li t0,(20/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(16/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro FillTriCullBackZSort,START,END,SORT ; Fill Triangle Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,4   ; A2 += 4 (Delay Slot)
    subiu a2,4   ; A2 -= 4

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,20 ; A3 += 20

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,20 ; A2 += 20 (Delay Slot)
    subiu a2,20 ; A2 -= 20

    li t0,(20/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(16/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro FillTriCullFrontZSort,START,END,SORT ; Fill Triangle Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,4   ; A2 += 4 (Delay Slot)
    subiu a2,4   ; A2 -= 4

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,20 ; A3 += 20

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,20 ; A2 += 20 (Delay Slot)
    subiu a2,20 ; A2 -= 20

    li t0,(20/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(16/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeTriCullNone,START,END ; Shaded Triangle Face Cull None: Object Start Address, Object End Address
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

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTriCullBack,START,END ; Shaded Triangle Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,12  ; A2 += 12 (Delay Slot)
    subiu a2,12  ; A2 -= 12

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

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTriCullFront,START,END ; Shaded Triangle Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,12  ; A2 += 12 (Delay Slot)
    subiu a2,12  ; A2 -= 12

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

    sw s3,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s4,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTriCullNoneZSort,START,END,SORT ; Shaded Triangle Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTriCullBackZSort,START,END,SORT ; Shaded Triangle Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,12  ; A2 += 12 (Delay Slot)
    subiu a2,12  ; A2 -= 12

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeTriCullFrontZSort,START,END,SORT ; Shaded Triangle Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,12  ; A2 += 12 (Delay Slot)
    subiu a2,12  ; A2 -= 12

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,28 ; A3 += 28

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,28 ; A2 += 28 (Delay Slot)
    subiu a2,28 ; A2 -= 28

    li t0,(28/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(24/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro TexTriCullNoneZSort,START,END,SORT ; Texture Triangle Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro TexTriCullBackZSort,START,END,SORT ; Texture Triangle Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro TexTriCullFrontZSort,START,END,SORT ; Texture Triangle Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro ShadeTexTriCullNoneZSort,START,END,SORT ; Shaded Texture Triangle Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexTriCullBackZSort,START,END,SORT ; Shaded Texture Triangle Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,24  ; A2 += 24 (Delay Slot)
    subiu a2,24  ; A2 -= 24

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeTexTriCullFrontZSort,START,END,SORT ; Shaded Texture Triangle Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,24  ; A2 += 24 (Delay Slot)
    subiu a2,24  ; A2 -= 24

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    AverageZTri s0,s1,s2 ; Average Z Triangle: Z1,Z2,Z3
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4

    lw s6,0(a2) ; S6 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s7,0(a2) ; S7 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord3
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro WireQuadCullNoneZSort,START,END,SORT ; Wire Quad Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex5 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro WireQuadCullBackZSort,START,END,SORT ; Wire Quad Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex5 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro WireQuadCullFrontZSort,START,END,SORT ; Wire Quad Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s3,0(v0) ; Store Vertex5 To Sort Address
    addiu v0,4  ; V0 += 4

    li t0,0x55555555 ; T0 = Termination Code
    sw t0,0(v0) ; Store Termination Code To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,32 ; A3 += 32

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,32 ; A2 += 32 (Delay Slot)
    subiu a2,32 ; A2 -= 32

    li t0,(32/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(28/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro FillQuadCullBack,START,END ; Fill Quad Back Face Cull: Object Start Address, Object End Address
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

.macro FillQuadCullFront,START,END ; Fill Quad Front Face Cull: Object Start Address, Object End Address
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

.macro FillQuadCullNoneZSort,START,END,SORT ; Fill Quad Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,24 ; A3 += 24

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,24 ; A2 += 24 (Delay Slot)
    subiu a2,24 ; A2 -= 24

    li t0,(24/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(20/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro FillQuadCullBackZSort,START,END,SORT ; Fill Quad Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,24 ; A3 += 24

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,24 ; A2 += 24 (Delay Slot)
    subiu a2,24 ; A2 -= 24

    li t0,(24/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(20/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro FillQuadCullFrontZSort,START,END,SORT ; Fill Quad Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,16  ; A2 += 16 (Delay Slot)
    subiu a2,16  ; A2 -= 16

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,24 ; A3 += 24

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,24 ; A2 += 24 (Delay Slot)
    subiu a2,24 ; A2 -= 24

    li t0,(24/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(20/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeQuadCullNone,START,END ; Shaded Quad Face Cull None: Object Start Address, Object End Address
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

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeQuadCullBack,START,END ; Shaded Quad Back Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,28  ; A2 += 28 (Delay Slot)
    subiu a2,28  ; A2 -= 28

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

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeQuadCullFront,START,END ; Shaded Quad Front Face Cull: Object Start Address, Object End Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  Loop:
    LoadXYZ s0,s1,s2 ; S0 = 2D X1, S1 = 2D Y1
    LoadXYZ s2,s3,s4 ; S2 = 2D X2, S3 = 2D Y2
    LoadXYZ s4,s5,s6 ; S4 = 2D X3, S5 = 2D Y3

    CullFace s0,s1,s2,s3,s4,s5 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,28  ; A2 += 28 (Delay Slot)
    subiu a2,28  ; A2 -= 28

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

    sw s4,GP0(a0) ; Write GP0 Command Word (Color1+Command)
    sw s0,GP0(a0) ; Write GP0  Packet Word (Vertex1)
    sw s5,GP0(a0) ; Write GP0  Packet Word (Color2)
    sw s1,GP0(a0) ; Write GP0  Packet Word (Vertex2)
    sw s6,GP0(a0) ; Write GP0  Packet Word (Color3)
    sw s2,GP0(a0) ; Write GP0  Packet Word (Vertex3)
    sw s7,GP0(a0) ; Write GP0  Packet Word (Color4)
    sw s3,GP0(a0) ; Write GP0  Packet Word (Vertex4)

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot
.endmacro

.macro ShadeQuadCullNoneZSort,START,END,SORT ; Shaded Quad Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,36 ; A3 += 36

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,36 ; A2 += 36 (Delay Slot)
    subiu a2,36 ; A2 -= 36

    li t0,(36/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(32/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro ShadeQuadCullBackZSort,START,END,SORT ; Shaded Quad Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,28  ; A2 += 28 (Delay Slot)
    subiu a2,28  ; A2 -= 28

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,36 ; A3 += 36

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,36 ; A2 += 36 (Delay Slot)
    subiu a2,36 ; A2 -= 36

    li t0,(36/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(32/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeQuadCullFrontZSort,START,END,SORT ; Shaded Quad Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,28  ; A2 += 28 (Delay Slot)
    subiu a2,28  ; A2 -= 28

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,36 ; A3 += 36

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,36 ; A2 += 36 (Delay Slot)
    subiu a2,36 ; A2 -= 36

    li t0,(36/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(32/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro TexQuadCullNoneZSort,START,END,SORT ; Texture Quad Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro TexQuadCullBackZSort,START,END,SORT ; Texture Quad Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,32  ; A2 += 32 (Delay Slot)
    subiu a2,32  ; A2 -= 32

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro TexQuadCullFrontZSort,START,END,SORT ; Texture Quad Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,32  ; A2 += 32 (Delay Slot)
    subiu a2,32  ; A2 -= 32

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color+Command
    addiu a2,4  ; A2 += 4

    lw s1,0(a2) ; S1 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw s8,0(a2) ; S8 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s1,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s2,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s6,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,40 ; A3 += 40

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,40 ; A2 += 40 (Delay Slot)
    subiu a2,40 ; A2 -= 40

    li t0,(40/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(36/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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

.macro ShadeTexQuadCullNoneZSort,START,END,SORT ; Shaded Texture Quad Face Cull None Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t0,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t1,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t2,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,52 ; A3 += 52

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,52 ; A2 += 52 (Delay Slot)
    subiu a2,52 ; A2 -= 52

    li t0,(52/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(48/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot
.endmacro

.macro ShadeTexQuadCullBackZSort,START,END,SORT ; Shaded Texture Quad Back Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Back Face Cull: X1,Y1,X2,Y2,X3,Y3
    bgez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) >= 0 Skip
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t0,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t1,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t2,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,52 ; A3 += 52

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,52 ; A2 += 52 (Delay Slot)
    subiu a2,52 ; A2 -= 52

    li t0,(52/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(48/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
.endmacro

.macro ShadeTexQuadCullFrontZSort,START,END,SORT ; Shaded Texture Quad Front Face Cull Z Sort: Object Start Address, Object End Address, Sort Address
  la a2,START ; A2 = Object Start Address
  la a3,END   ; A3 = Object End Address
  la v0,SORT  ; V0 = Sort Address
  Loop:
    LoadXYZ s3,s4,s0 ; S3 = 2D X1, S4 = 2D Y1, S0 = 3D Z1
    LoadXYZ s5,s6,s1 ; S5 = 2D X2, S6 = 2D Y2, S1 = 3D Z2
    LoadXYZ s7,s8,s2 ; S6 = 2D X3, S8 = 2D Y3, S2 = 3D Z3

    CullFace s3,s4,s5,s6,s7,s8 ; Front Face Cull: X1,Y1,X2,Y2,X3,Y3
    blez t0,Skip ; IF ((X3-X1 * Y2-Y1) - (Y3-Y1 * X2-X1)) <= 0 Skip
    addiu a2,44  ; A2 += 44 (Delay Slot)
    subiu a2,44  ; A2 -= 44

    sll s4,16      ; Y1 <<= 16
    andi s3,0xFFFF ; X1 &= 0xFFFF
    or s3,s4       ; X1 |= Y1

    sll s6,16      ; Y2 <<= 16
    andi s5,0xFFFF ; X2 &= 0xFFFF
    or s4,s5,s6    ; X2 |= Y2

    sll s8,16      ; Y3 <<= 16
    andi s7,0xFFFF ; X3 &= 0xFFFF
    or s5,s7,s8    ; X3 |= Y3

    LoadXYZ s7,s8,s6 ; S7 = 2D X4, S8 = 2D Y4, S6 = 3D Z4

    AverageZQuad s0,s1,s2,s6 ; Average Z Quad: Z1,Z2,Z3,Z4
    sw t0,0(v0) ; Store Average Z To Sort Address
    addiu v0,4  ; V0 += 4

    sll s8,16      ; Y4 <<= 16
    andi s7,0xFFFF ; X4 &= 0xFFFF
    or s7,s8       ; X4 |= Y4

    lw s0,0(a2) ; S0 = Color1+Command
    addiu a2,4  ; A2 += 4
    lw s1,0(a2) ; S1 = Color2
    addiu a2,4  ; A2 += 4
    lw s2,0(a2) ; S2 = Color3
    addiu a2,4  ; A2 += 4
    lw s6,0(a2) ; S6 = Color4
    addiu a2,4  ; A2 += 4

    lw s8,0(a2) ; S8 = Texcoord1+Palette
    addiu a2,4  ; A2 += 4
    lw t0,0(a2) ; T0 = Texcoord2+Texpage
    addiu a2,4  ; A2 += 4
    lw t1,0(a2) ; T1 = Texcoord3
    addiu a2,4  ; A2 += 4
    lw t2,0(a2) ; T2 = Texcoord4
    addiu a2,4  ; A2 += 4

    sw s0,0(v0) ; Store Color1+Command To Sort Address
    addiu v0,4  ; V0 += 4
    sw s3,0(v0) ; Store Vertex1 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s8,0(v0) ; Store Texcoord1+Palette To Sort Address
    addiu v0,4  ; V0 += 4

    sw s1,0(v0) ; Store Color2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s4,0(v0) ; Store Vertex2 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t0,0(v0) ; Store Texcoord2+TexPage To Sort Address
    addiu v0,4  ; V0 += 4

    sw s2,0(v0) ; Store Color3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s5,0(v0) ; Store Vertex3 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t1,0(v0) ; Store Texcoord3 To Sort Address
    addiu v0,4  ; V0 += 4

    sw s6,0(v0) ; Store Color4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw s7,0(v0) ; Store Vertex4 To Sort Address
    addiu v0,4  ; V0 += 4
    sw t2,0(v0) ; Store Texcoord4 To Sort Address
    addiu v0,4  ; V0 += 4

    Skip:
    bne a2,a3,Loop ; IF (Object Start Offset != Object End Offset) Loop
    nop ; Delay Slot

  ; Sort List (Selection Sort)
  la a2,SORT ; A2 = Sort Start Address
  beq a2,v0,SkipObject ; IF (Sort Start Offset == Sort End Offset) Skip Object
  nop ; Delay Slot
  SelectionSort:
    or a3,a2,r0 ; A3 = Sort Current Address
    li t0,0 ; T0 = MaxZ
    SortLoop:
      lw t1,0(a3) ; T1 = Average Z Value
      nop ; Delay Slot
      blt t1,t0,ZLT ; IF (Average Z Value < MaxZ) Z Less Than
      nop ; Delay Slot
      or t0,t1,r0 ; ELSE (MaxZ = Average Z Value)
      or v1,a3,r0 ; V1 = MaxZ Address
      ZLT:
      addiu a3,52 ; A3 += 52

      bne a3,v0,SortLoop ; IF (Sort Current Offset != Sort End Offset) Sort Loop
      nop ; Delay Slot

    beq v1,a2,NoSwap ; IF (MaxZ Address == Sort Start Address) No Swap
    addiu a2,52 ; A2 += 52 (Delay Slot)
    subiu a2,52 ; A2 -= 52

    li t0,(52/4)-1 ; T0 = Swap Count
    Swap:
      lw t1,0(a2)
      lw t2,0(v1)
      sw t1,0(v1)
      sw t2,0(a2)
      addiu a2,4 ; A2 += 4
      addiu v1,4 ; V1 += 4
      bnez t0,Swap ; IF (Swap Count != 0) Swap
      subiu t0,1 ; T0-- (Delay Slot)

    NoSwap:
    bne a2,v0,SelectionSort ; IF (Sort Start Offset != Sort End Offset) Selection Sort
    nop ; Delay Slot

  ; Render Sort List
  la a1,SORT ; A1 = Sort Start Address
  RenderLoop:
    addiu a1,4 ; A1 += 4 (Skip Average Z)
    li t0,(48/4)-1 ; T0 = Render Count
    Render:
      lw t1,0(a1) ; T1 = Command/Packet Word
      addiu a1,4  ; A1 += 4
      sw t1,GP0(a0) ; Write GP0 Command/Packet Word
      bnez t0,Render ; IF (Render Count != 0) Render
      subiu t0,1 ; T0-- (Delay Slot)
    bne a1,v0,RenderLoop ; IF (Sort Start Offset != Sort End Offset) Render Loop
    nop ; Delay Slot

  SkipObject:
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