;================
; PSX GTE (COP2)
;================
; GTE Registers - 16-Bit Vectors
VXY0 equ  0 ; GTE R0 - 16-Bit Vectors: Vector XY 0 (V0) - Bit 0..15 = VX0 (S.15.0), Bit 16..31 VY0 (S.15.0)
VZ0  equ  1 ; GTE R1 - 16-Bit Vectors: Vector  Z 0 (V0) - Bit 0..15 = VZ0 (S.15.0) (Returns Sign-Expanded 32-Bit Value)

VXY1 equ  2 ; GTE R2 - 16-Bit Vectors: Vector XY 1 (V1) - Bit 0..15 = VX1 (S.15.0), Bit 16..31 VY1 (S.15.0)
VZ1  equ  3 ; GTE R3 - 16-Bit Vectors: Vector  Z 1 (V1) - Bit 0..15 = VZ1 (S.15.0) (Returns Sign-Expanded 32-Bit Value)

VXY2 equ  4 ; GTE R4 - 16-Bit Vectors: Vector XY 2 (V2) - Bit 0..15 = VX2 (S.15.0), Bit 16..31 VY2 (S.15.0)
VZ2  equ  5 ; GTE R5 - 16-Bit Vectors: Vector  Z 2 (V2) - Bit 0..15 = VZ2 (S.15.0) (Returns Sign-Expanded 32-Bit Value)

IR1  equ  9 ; GTE R9  - 16-Bit Vectors: Vector 3 (IR) - Bit 0..15 = IR1 (S.15.0) (Returns Sign-Expanded 32-Bit Value)
IR2  equ 10 ; GTE R10 - 16-Bit Vectors: Vector 3 (IR) - Bit 0..15 = IR2 (S.15.0) (Returns Sign-Expanded 32-Bit Value)
IR3  equ 11 ; GTE R11 - 16-Bit Vectors: Vector 3 (IR) - Bit 0..15 = IR3 (S.15.0) (Returns Sign-Expanded 32-Bit Value)

; GTE Registers - Interpolation Factor
IR0  equ  8 ; GTE R8 - Interpolation Factor: Intermediate Value 0 (IR0) - Bit 0..15 = IR0 (S.3.12) (Returns Sign-Expanded 32-Bit Value)

; GTE Registers - Screen XYZ Coordinate FIFO
SXY0 equ 12 ; GTE R12 - Screen XYZ Coordinate FIFO: Screen XY 0 FIFO (Older)  - Bit 0..15 = SX0 (S.15.0), Bit 16..31 SY0 (S.15.0)
SXY1 equ 13 ; GTE R13 - Screen XYZ Coordinate FIFO: Screen XY 1 FIFO (Old)    - Bit 0..15 = SX1 (S.15.0), Bit 16..31 SY1 (S.15.0)
SXY2 equ 14 ; GTE R14 - Screen XYZ Coordinate FIFO: Screen XY 2 FIFO (New)    - Bit 0..15 = SX2 (S.15.0), Bit 16..31 SY2 (S.15.0)
SXYP equ 15 ; GTE R15 - Screen XYZ Coordinate FIFO: Screen XY 2 FIFO (Mirror) - Bit 0..15 = SXP (S.15.0), Bit 16..31 SYP (S.15.0) (Move-On-Write)

SZ0  equ 16 ; GTE R16 - Screen XYZ Coordinate FIFO: Screen Z 0 FIFO (Oldest) - Bit 0..15 = SZ0 (16.0)
SZ1  equ 17 ; GTE R17 - Screen XYZ Coordinate FIFO: Screen Z 1 FIFO (Older)  - Bit 0..15 = SZ1 (16.0)
SZ2  equ 18 ; GTE R18 - Screen XYZ Coordinate FIFO: Screen Z 2 FIFO (Old)    - Bit 0..15 = SZ2 (16.0)
SZ3  equ 19 ; GTE R19 - Screen XYZ Coordinate FIFO: Screen Z 3 FIFO (New)    - Bit 0..15 = SZ3 (16.0)

; GTE Registers - Color Register & Color FIFO
RGBC equ  6 ; GTE R6 - Color Register & Color FIFO: Color/Code Value (RGBC) - Bit 0..7 = R, Bit 8..15 = G, Bit 16..23 = B, Bit 24..31 = CODE

RGB0 equ 20 ; GTE R20 - Color Register & Color FIFO: Characteristic Color FIFO 0 (RGB0) - Bit 0..7 = R0, Bit 8..15 = G0, Bit 16..23 = B0, Bit 24..31 = CD0
RGB1 equ 21 ; GTE R21 - Color Register & Color FIFO: Characteristic Color FIFO 1 (RGB1) - Bit 0..7 = R1, Bit 8..15 = G1, Bit 16..23 = B1, Bit 24..31 = CD1
RGB2 equ 22 ; GTE R22 - Color Register & Color FIFO: Characteristic Color FIFO 2 (RGB2) - Bit 0..7 = R2, Bit 8..15 = G2, Bit 16..23 = B2, Bit 24..31 = CD2

RES1 equ 23 ; GTE R23 - Color Register and Color FIFO: Reserved (R/W)

; GTE Registers - 32-Bit Maths Accumulator/Sum Of Product
MAC0 equ 24 ; GTE R24 - Maths Accumulator/Sum Of Product: Value  0 (MAC0) - Bit 0..31 = MAC0 (S.31.0)
MAC1 equ 25 ; GTE R25 - Maths Accumulator/Sum Of Product: Vector 1 (MAC1) - Bit 0..31 = MAC1 (S.31.0)
MAC2 equ 26 ; GTE R26 - Maths Accumulator/Sum Of Product: Vector 2 (MAC2) - Bit 0..31 = MAC2 (S.31.0)
MAC3 equ 27 ; GTE R27 - Maths Accumulator/Sum Of Product: Vector 3 (MAC3) - Bit 0..31 = MAC3 (S.31.0)

; GTE Registers - Color Conversion R,G,B (48-Bit/15-Bit)
IRGB equ 28 ; GTE R28 - Color Conversion:  Input (IRGB) - Bit 0..4 = Red, Bit 5..9 = Green, Bit 10..14 = Blue, Bit 15-31 = Zero (Not Used)
ORGB equ 29 ; GTE R29 - Color Conversion: Output (IRGB) - Bit 0..4 = Red, Bit 5..9 = Green, Bit 10..14 = Blue, Bit 15-31 = Zero (Not Used)

; GTE Registers - Count Leading-Zero/One (Sign Bits)
LZCS equ 30 ; GTE R30 - Count Leading-Zero/One: Source (LZCS) - Bit 0..31 = LZCS
LZCR equ 31 ; GTE R31 - Count Leading-Zero/One: Result (LZCR) - Bit 0..31 = LZCR

; GTE Registers - Matrix (3x3)
RT0  equ  0 ; GTE R32 - Matrix: Rotation (RT/CNT00) - Bit 0..15 = RT11 (S.3.12), Bit 16..31 RT12 (S.3.12)
RT1  equ  1 ; GTE R33 - Matrix: Rotation (RT/CNT01) - Bit 0..15 = RT13 (S.3.12), Bit 16..31 RT21 (S.3.12)
RT2  equ  2 ; GTE R34 - Matrix: Rotation (RT/CNT02) - Bit 0..15 = RT22 (S.3.12), Bit 16..31 RT23 (S.3.12)
RT3  equ  3 ; GTE R35 - Matrix: Rotation (RT/CNT03) - Bit 0..15 = RT31 (S.3.12), Bit 16..31 RT32 (S.3.12)
RT4  equ  4 ; GTE R36 - Matrix: Rotation (RT/CNT04) - Bit 0..15 = RT33 (S.3.12) (Returns Sign-Expanded 32-Bit Value)

LLM0 equ  8 ; GTE R40 - Matrix: Light Source (LLM/CNT08) - Bit 0..15 = L11 (S.3.12), Bit 16..31 L12 (S.3.12)
LLM1 equ  9 ; GTE R41 - Matrix: Light Source (LLM/CNT09) - Bit 0..15 = L13 (S.3.12), Bit 16..31 L21 (S.3.12)
LLM2 equ 10 ; GTE R42 - Matrix: Light Source (LLM/CNT10) - Bit 0..15 = L22 (S.3.12), Bit 16..31 L23 (S.3.12)
LLM3 equ 11 ; GTE R43 - Matrix: Light Source (LLM/CNT11) - Bit 0..15 = L31 (S.3.12), Bit 16..31 L32 (S.3.12)
LLM4 equ 12 ; GTE R44 - Matrix: Light Source (LLM/CNT12) - Bit 0..15 = L33 (S.3.12) (Returns Sign-Expanded 32-Bit Value)

LCM0 equ 16 ; GTE R48 - Matrix: Light Color Source (LCM/CNT16) - Bit 0..15 = LR1 (S.3.12), Bit 16..31 LR2 (S.3.12)
LCM1 equ 17 ; GTE R49 - Matrix: Light Color Source (LCM/CNT17) - Bit 0..15 = LR3 (S.3.12), Bit 16..31 LG1 (S.3.12)
LCM2 equ 18 ; GTE R50 - Matrix: Light Color Source (LCM/CNT18) - Bit 0..15 = LG2 (S.3.12), Bit 16..31 LG3 (S.3.12)
LCM3 equ 19 ; GTE R51 - Matrix: Light Color Source (LCM/CNT19) - Bit 0..15 = LB1 (S.3.12), Bit 16..31 LB2 (S.3.12)
LCM4 equ 20 ; GTE R52 - Matrix: Light Color Source (LCM/CNT20) - Bit 0..15 = LB3 (S.3.12) (Returns Sign-Expanded 32-Bit Value)

; GTE Registers - Translation Vector X,Y,Z (TR)
TRX  equ  5 ; GTE R37 - Translation Vector: X (TRX/CNT05) - Bit 0..31 = TRX (S.31.0)
TRY  equ  6 ; GTE R38 - Translation Vector: Y (TRX/CNT06) - Bit 0..31 = TRY (S.31.0)
TRZ  equ  7 ; GTE R39 - Translation Vector: Z (TRX/CNT07) - Bit 0..31 = TRZ (S.31.0)

; GTE Registers - Background Color R,G,B (BK)
RBK  equ 13 ; GTE R45 - Background Color: Red   Component (RBK/CNT13) - Bit 0..31 = RBK (S.19.12)
GBK  equ 14 ; GTE R46 - Background Color: Green Component (GBK/CNT14) - Bit 0..31 = GBK (S.19.12)
BBK  equ 15 ; GTE R47 - Background Color: Blue  Component (BBK/CNT15) - Bit 0..31 = BBK (S.19.12)

; GTE Registers - Far Color R,G,B (FC)
RFC  equ 21 ; GTE R53 - Far Color: Red   Component (RFC/CNT21) - Bit 0..31 = RFC (S.27.4)
GFC  equ 22 ; GTE R54 - Far Color: Green Component (GFC/CNT22) - Bit 0..31 = GFC (S.27.4)
BFC  equ 23 ; GTE R55 - Far Color: Blue  Component (BFC/CNT23) - Bit 0..31 = BFC (S.27.4)

; GTE Registers - Screen Offset & Distance
OFX  equ 24 ; GTE R56 - Screen Offset & Distance: Screen Offset X (OFX/CNT24) - Bit 0..31 = OFX (S.15.16)
OFY  equ 25 ; GTE R57 - Screen Offset & Distance: Screen Offset Y (OFY/CNT25) - Bit 0..31 = OFY (S.15.16)

H    equ 26 ; GTE R58 - Screen Offset & Distance: Projection Plane Distance (H/CNT26) - Bit 0..15 = H (16.0) (Returns Sign-Expanded 32-Bit Value)

DQA  equ 27 ; GTE R59 - Screen Offset & Distance: Depth Queing Parameter A. (Coefficient) (DQA/CNT27) - Bit 0..15 = DQA (S.7.8)
DQB  equ 28 ; GTE R60 - Screen Offset & Distance: Depth Queing Parameter A. (Offset)      (DQB/CNT28) - Bit 0..31 = DQB (S.7.24)

; GTE Registers - Average Z Factors
ZSF3 equ 29 ; GTE R61 - Average Z: Z3 Average Scale Factor (Normally 1/3) (ZSF3/CNT29) - Bit 0..15 = ZSF3 (S.3.12)
ZSF4 equ 30 ; GTE R62 - Average Z: Z4 Average Scale Factor (Normally 1/4) (ZSF4/CNT30) - Bit 0..15 = ZSF4 (S.3.12)

OTZ  equ  7 ; GTE R7 - Average Z Registers: Average Z Value (Ordering Table) (OTZ) - Bit 0..15 = OTZ (16.0)

; GTE Registers - Error Flag
FLAG equ 31 ; GTE R63 - Error Flag: Returns Calculation Errors (FLAG/CNT31) - Bit 0..31 = FLAG


; GTE Coordinate Calculation Commands
RTPS  equ 0x0180001 ; GTE Coordinate Calculation Commands: Perspective Transformation (Single) - 15 Cycles
RTPT  equ 0x0280030 ; GTE Coordinate Calculation Commands: Perspective Transformation (Triple) - 23 Cycles

NCLIP equ 0x1400006 ; GTE Coordinate Calculation Commands: Normal Clipping - 8 Cycles

AVSZ3 equ 0x158002D ; GTE Coordinate Calculation Commands: Average Three Z Values (Triangle) - 5 Cycles
AVSZ4 equ 0x168002E ; GTE Coordinate Calculation Commands: Average Four  Z Values (Quad) - 6 Cycles

; GTE General Purpose Calculation Commands
MVMVA equ 0x0400012 ; GTE General Purpose Calculation Commands: Multiply Vector By Matrix With Vector Addition - 8 Cycles
SQR   equ 0x0A00428 ; GTE General Purpose Calculation Commands: Calculate Square Of Vector (Result Always Positive) - 5 Cycles
OP    equ 0x170000C ; GTE General Purpose Calculation Commands: Calculate Outer Product Of Two Signed 16-Bit Vectors - 6 Cycles

; GTE Color Calculation Commands
NCS   equ 0x0C8041E ; GTE Color Calculation Commands: Normal Color (Single) - 14 Cycles
NCT   equ 0x0D80420 ; GTE Color Calculation Commands: Normal Color (Triple) - 30 Cycles
NCCS  equ 0x108041B ; GTE Color Calculation Commands: Normal Color Color (Single Vector) - 17 Cycles
NCCT  equ 0x118043F ; GTE Color Calculation Commands: Normal Color Color (Triple Vector) - 39 Cycles
NCDS  equ 0x0E80413 ; GTE Color Calculation Commands: Normal Color Depth Cue (Single Vector) - 19 Cycles
NCDT  equ 0x0F80416 ; GTE Color Calculation Commands: Normal Color Depth Cue (Triple Vector) - 44 Cycles

CC    equ 0x138041C ; GTE Color Calculation Commands: Color Color - 11 Cycles
CDP   equ 0x1280414 ; GTE Color Calculation Commands: Color Depth Cue - 13 Cycles

DCPL  equ 0x0680029 ; GTE Color Calculation Commands: Depth Cue Color Light - 8 Cycles
DPCS  equ 0x0780010 ; GTE Color Calculation Commands: Depth Cueing (Single) - 8 Cycles
DPCT  equ 0x088002A ; GTE Color Calculation Commands: Depth Cueing (Triple) - 17 Cycles
INTPL equ 0x0980011 ; GTE Color Calculation Commands: Interpolation Of Vector & Far Color - 8 Cycles

GPF   equ 0x190003D ; GTE Color Calculation Commands: General Purpose Interpolation - 5 Cycles
GPL   equ 0x1A0003E ; GTE Color Calculation Commands: General Purpose Interpolation With Base - 5 Cycles

