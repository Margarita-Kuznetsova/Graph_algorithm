# import required module
from pathlib import Path
import os
import subprocess
import threading as th
 
# assign directory
directory = 'benchmark_set'
 
# iterate over files in
# that directory
files = sorted(Path(directory).glob('*.hgr'))

def doProcessing(bin, output):
    for file in files:
        process_one = subprocess.check_output([f'./build/{bin}', file])
        output.append(process_one)

modout = []
orgout = []
mod = th.Thread(target=doProcessing, args=('default',modout,))
org = th.Thread(target=doProcessing, args=('modified',orgout,))

mod.start()
org.start()

mod.join()
org.join()

def ptinting(out, file):
    for x in out:
        file.write(x.decode('utf-8'))

modf = open("ResultsMod.out", 'w')
orgf = open("ResultsOrg.out", 'w')

ptinting(modout, modf)
ptinting(orgout, orgf)
