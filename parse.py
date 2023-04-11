#!/bin/python3

from pathlib import Path
import subprocess


def start(bin : str, dump : str):
    out = []
    processes = []
    files = sorted(Path('benchmark_set').glob('*.hgr'))
    length = len(files)
    for file in files:
        process = subprocess.Popen([f'./bin/{bin}', file])
        processes.append(process)
    print(f"Starting {dump}", flush=True)
    for i in range(length):
        percent_complete(i, length, title=dump)
        processes[i].wait()
        with open(str(files[i]) + '.part.2') as f:
            out.append(f.read())
    percent_complete(length, length, title=dump)
    return out

def printFile(data, file):
    for result in data:
        file.write(result)

def main():
    org = open("Result.out", 'w')
    mod = open("ResultMod.out", 'w')
    data_org = start("default", 'default')
    printFile(data_org, org)
    data_mod = start("modified", 'modified')
    printFile(data_mod, mod)

if __name__ == "__main__":
    main()