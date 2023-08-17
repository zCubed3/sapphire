# MIT License
#
# Copyright (c) 2023 zCubed (Liam R.)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Merges a SPIR-V vert-frag shader into a single file

import sys
import os

in_vert_path = sys.argv[1]
in_frag_path = sys.argv[2]
out_path = sys.argv[3]

vert_source = ""
frag_source = ""

with open(in_vert_path, "rb") as vert_src_file:
    vert_source = vert_src_file.read()

with open(in_frag_path, "rb") as frag_src_file:
    frag_source = frag_src_file.read()

with open(out_path, "wb") as merge_file:
    vert_len = len(vert_source)
    frag_len = len(frag_source)

    merge_file.write("MSPV".encode('ascii'))

    # The MSPV format expects descriptor for the stage type before reading it
    merge_file.write("VERT".encode('ascii'))
    merge_file.write(vert_len.to_bytes(4, byteorder="little"))
    merge_file.write(vert_source)

    merge_file.write("FRAG".encode('ascii'))
    merge_file.write(frag_len.to_bytes(4, byteorder="little"))
    merge_file.write(frag_source)

