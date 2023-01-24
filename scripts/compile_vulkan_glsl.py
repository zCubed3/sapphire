# Assistant script for compiling GLSL vert+frag files for Vulkan

# The first argument is the output
# The rest are input files (in order of inclusion)

import sys
import subprocess

out_path = sys.argv[1]
temp_out_path = out_path + ".merge_temp.glsl"

source = ""
for i in range(2, len(sys.argv)):
    with open(sys.argv[i], 'r') as src_file:
        source += src_file.read()

with open(temp_out_path, "w") as merge_file:
    merge_file.write(source)

subprocess.run(["glslc"])