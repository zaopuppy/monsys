#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import concurrent.futures
from concurrent.futures import ThreadPoolExecutor

def long_opera(v):
    print("enter long_opera()")
    time.sleep(5)
    print("left long_opera()")
    return v

def main():
    with ThreadPoolExecutor(max_workers=3) as executor:
        # fs = [ executor.submit(long_opera, i) for i in range(9) ]
        executor.map(long_opera, range(9))
        print("wait")
        for future in concurrent.futures.as_completed(fs):
        # for future in fs:
            print("loop")
            data = future.result()
            print("data: {}".format(data))

if __name__ == "__main__":
    main()
