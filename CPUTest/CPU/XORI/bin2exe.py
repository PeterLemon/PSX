#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import struct
import math

usage = '''
python bin2exe.py infile outfile
'''

def main(argv):
    if len(argv) != 2:
        print(usage, file=sys.stderr)
        sys.exit(1)

    max_size = 0x200000
    infile_size = os.path.getsize(argv[0])
    if infile_size > max_size:
        print("Error: Input file %s longer than %d bytes" % (argv[0], max_size), file=sys.stderr)
        sys.exit(1)

    ofile = open(argv[1], 'wb')
    
    with open(argv[0], 'rb') as ifile:
        # Write header
        if sys.version_info >= (3, 0):
            ofile.write(bytes('PS-X EXE', 'ascii'))
        else:
            ofile.write('PS-X EXE')
        # Entry point
        ofile.seek(0x10)
        ofile.write(struct.pack('<I',0x80010000))
        # Initial GP/R28 (crt0.S currently sets this)
        ofile.write(struct.pack('<I',0xFFFFFFFF))
        # Destination address in RAM
        ofile.write(struct.pack('<I',0x80010000))
        # Initial SP/R29 & FP/R30
        ofile.seek(0x30)
        ofile.write(struct.pack('<I',0x801FFF00))
        # SP & FP offset added to    ^^^^^^^^^^ just use 0
        #ofile.write(struct.pack('<I',0x00000000))
        # Zero fill rest of the header
        ofile.seek(0x800)

        # Copy input to output
        buffer_size = 0x2000
        for i in range(0,int(math.ceil(float(infile_size)/buffer_size))):
            buffer = ifile.read(buffer_size)
            ofile.write(buffer)
        # ofile.write(ifile.read())

        # Pad binary to 0x800 boundary
        exe_size = ofile.tell()
        if exe_size % 0x800 != 0:
            exe_size += (0x800 - (exe_size % 0x800))
            ofile.seek(exe_size-1)
            ofile.write(struct.pack('B',0))

        # Filesize excluding 0x800 byte header
        ofile.seek(0x1C)
        ofile.write(struct.pack('<I', exe_size - 0x800))

    ofile.close()

if __name__ == '__main__':
    main(sys.argv[1:])
    sys.exit(0)

