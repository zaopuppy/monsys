#!/usr/bin/env python

import os
import os.path
import sys


def run_java(class_path, jar_dir_list, main_class):
    for d in jar_dir_list:
        for root, dirs, files in os.walk(d):
            class_path.extend(
                [ os.path.join(root, f)
                  for f in filter(lambda x: x.endswith(".jar"), files) ])
    print(class_path)
    args = (
        "java",
        "-classpath", ":".join(class_path),
        main_class,
        )
    os.execvp(args[0], args)


def main():
    if len(sys.argv) < 2:
        print("bad arguments")
        return

    if sys.argv[1] == "server":
        run_java(["bin"],
                 ["libs"],
                 "com.letsmidi.monsys.push.PushServer")
    # elif sys.argv[2] == "client":
    #     run_java(["bin"],
    #              ["libs"],
    #              "com.letsmidi.monsys.push.PushServer")
    else:
        print("bad arguments")


if __name__ == "__main__":
    main()

