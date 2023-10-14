import os

c_files = []
for dirpath, dirnames, filenames in os.walk('.'):
    for filename in filenames:
        if filename.endswith('.c'):
            c_files.append(os.path.join(dirpath, filename))
FLAGS = "-Wall "
gcc_cmd = 'gcc '

for c_file in c_files:
    gcc_cmd += c_file + ' '
gcc_cmd += FLAGS
gcc_cmd += '-o webserver'
os.system(gcc_cmd)
