import os
import sys
import glob
def getFileName(idx):
    cnt = 0
    cpy = idx
    res = ".jpg.txt"
    while cpy > 0:
        cnt = cnt + 1
        res = str(cpy % 10) + res
        cpy = int(cpy / 10)
    
    for i in range(6 - cnt):
        res = str(0) + res
    return res
files = glob.glob(sys.argv[2] + '/*')
for f in files:
    os.remove(f)
f = open(sys.argv[1], "r")
f = f.readlines()
created = {}
for line in f:
    tokens = line.split(',')
    FILE = sys.argv[2] + '/' + getFileName(int(tokens[0]))
    if FILE in created.keys():
        append_write = 'a' # append if already exists
    else:
        created[FILE] = 1
        append_write = 'w' # make a new file if not
    file = open(FILE, append_write)
    file.write(tokens[2] + " " + tokens[3] + " " + str(float(tokens[2]) + float(tokens[4])) + " " + str(float(tokens[3]) + float(tokens[5])) + " 0 1\n")
    file.close()