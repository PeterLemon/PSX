.psx
.create "hit-green.bin", 0

; Another simple asm intro source by Silpheed of HITMEN

.org 0x80010000

li sp,0x801FFF00
li k1,0x1F800000 ; set to hardware base

li a0,0x08000009
jal InitGPU ; initialise the GPU
nop

la a0,image ; transfer image data to VRAM
li a1,320
li a2,0x820090
li a3,9360
jal MEM2VRAM_IO
nop

la a0,clut ; transfer clut data to VRAM
li a1,0x1000140
li a2,0x10100
li a3,128
jal MEM2VRAM_IO
nop

jal InitPads ; init pads, also the wait vsync routine
nop

la a0, module
jal HM_Init ; init the mod player
nop


; Main parts of the intro:

; Part 1 - draw the lines across the top and bottom of the screen

li s0,0
li s1,320
part1:
  sh s0,line1 ; update line positions
  sh s1,line2

  jal WaitIdle ; wait for GPU to finish processing
  nop

  jal WaitVSync ; wait for vertical retrace period
  nop

  la a0,list
  jal SendList ; send display list to GPU
  nop

  slti s2,s0,320
  subiu s1,4
  bnez s2,part1 ; loop until lines finished
  addiu s0,4


; Part 2 - move logo down from top of screen

li s1,0xFFFFFFAE ; (-82)
part2:
  sh s1,lpos1+2 ; update sprite positions
  sh s1,lpos2+2 ; 2 sprites needed (logo is more than 256 pixels wide)

  jal WaitIdle
  nop

  jal WaitVSync
  nop

  la a0,list
  jal SendList
  nop

  slti s2,s1,32
  bnez s2,part2
  addiu s1,2


; Part 3 - the main part... do the text writer and play the music

li s0,0 ; page number
li s4,0 ; delay counter

resetpage:
  sll s1,s0,2
  la s2,pages
  addu s2,s1
  lw s1,(s2) ; read page address

  li s2,0 ; char number
  la s3,prim3 ; first char in display list

part3:
  jal HM_Poll ; call mod player
  nop

  slti t0,s2,288
  bnez t0,skipreset ; dont reset until all chars updated
  nop

  slti t0,s4,300 ; wait for 300 frames before continuing
  bnez t0,skipupdate
  addiu s4,1

  li s4,0 ; reset counter
  addiu s0,1 ; goto next page
  andi s0,3

  li t0,0 ; clear all chars
  li t1,82
  li t4,0
  la t3,prim3

clearpage:
  sb t0,12(t3)
  sb t1,13(t3)

  addiu t3,16
  addiu t4,1
  slti t5,t4,288
  bnez t5,clearpage
  nop

  jal WaitVSync
  nop

  b resetpage
  nop

skipreset:
  addu t0,s1,s2 ; address of current char
  lb t1,(t0) ; read it
  nop
  subiu t1,32

  srl t0,t1,4
  sll t0,3 ; t0 = y offset of char in tpage
  andi t1,15
  sll t1,3
  addiu t0,82 ; t1 = x offset of char in tpage

  sb t0,13(s3) ; update sprite in display list
  sb t1,12(s3)

  addiu s3,16 ; move to next sprite
  addiu s2,1 ; next char

skipupdate:
  jal WaitIdle
  nop

  jal WaitVSync
  nop

  la a0,list
  jal SendList
  nop

  j part3 ; loop forever
  nop


.include "silph.inc" ; some useful routines

image:
  .incbin "gfx.raw" ; the raw image data for the logo and chars
clut:
  .incbin "gfx.clt" ; clut info for above

.align 4
list:
  .include "list.inc" ; the display list

pages:
  dw page1,page2,page3,page4 ; the pages to display

page1:
    db "------------------------"
    db "   HITMEN Presents...   "
    db "                        "
    db "     The GREENtro!      "
    db "                        "
    db "   Hmm... I think this  "
    db "  business of calling   "
    db "  every release XXXXtro "
    db "   is getting silly...  "
    db "      dont you? :)      "
    db "                        "
    db "------------------------"

page2:
    db "                        "
    db "     INTRO CREDITS:     "
    db "                        "
    db "                        "
    db "CODING..........Silpheed"
    db "                        "
    db "LOGO..............Gunhed"
    db "                        "
    db "MUSIC............Unknown"
    db "                        "
    db "                        "
    db "                        "

page3:
    db "                        "
    db "Greets fly out to...    "
    db "(in no particular order)"
    db "                        "
    db "BlackBag Napalm Creature"
    db "Roncler Gang K-Comm Acon"
    db "   Birdhouse Projects   "
    db "   Vision Thing Fab-4   "
    db "                        "
    db "Special greets to Nagra "
    db "     and Dodger...      "
    db "                        "

page4:
    db "                        "
    db "                        "
    db " Now go take a look at  "
    db " the source for this    "
    db " little thing and get   "
    db "  coding!!              "
    db "                        "
    db "                        "
    db "     - Silpheed         "
    db "                        "
    db "                        "
    db "HITMEN - Ruling in 1998!"

module:
  .incbin "thesong.hit"

.align 4
.include "hitmod.inc"

.close