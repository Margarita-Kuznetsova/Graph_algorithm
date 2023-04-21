#!/bin/python3

from pathlib import Path
import subprocess


def start(mod : bool, dump : str):
    out = []
    processes = []
    files = sorted(Path('benchmark_set').glob('*.hgr'))
    length = len(files)
    command = ['./bin/FM'] if not mod else ['./bin/FM', '-m']
    for file in files:
        process = subprocess.Popen(command + [file])
        processes.append(process)
    print(f"Starting {dump}", flush=True)
    for i in range(length):
        processes[i].wait()
        with open(str(files[i]) + '.part.2') as f:
            out.append(f.read())
    return out

def printFile(data, file):
    for result in data:
        file.write(result)

def main():
    org = open("Result.out", 'w')
    mod = open("ResultMod.out", 'w')
    data_org = start(False, 'default')
    printFile(data_org, org)
    data_mod = start(True, 'modified')
    printFile(data_mod, mod)

if __name__ == "__main__":
    main()