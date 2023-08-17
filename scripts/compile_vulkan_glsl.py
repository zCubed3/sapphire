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

# Assistant script for compiling GLSL vert+frag files for Vulkan

# The first argument is the output
# The rest are input files (in order of inclusion)

import os
import sys
import subprocess

out_path = sys.argv[1]
stage = sys.argv[2]

temp_merge_path = out_path + ".merge_temp.glsl"

source = ""
for i in range(3, len(sys.argv)):
    with open(sys.argv[i], 'r') as src_file:
        source += src_file.read()

with open(temp_merge_path, "w") as merge_file:
    merge_file.write(source)
    merge_file.write("\n\n")

subprocess.run(["glslc", f"-fshader-stage={stage}", "-o", out_path, temp_merge_path])
os.remove(temp_merge_path)