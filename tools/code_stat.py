#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import os.path

# ---------------------------------------------------------------
FILE_POSTFIXS = (".cc", ".h", ".cpp", ".c", ".java")

# ---------------------------------------------------------------
def should_stat(file_name):
    for postfix in FILE_POSTFIXS:
        if file_name.endswith(postfix):
            return True
    return False

def code_line_stat_cpp(file_name):
    line_num = 0
    # if line only contains ' ' or '\t' or comment, then this line is invalid
    valid_line = False
    NOT_IN_COMMENT = 0
    IN_COMMENT_INIT = 1         # /
    IN_COMMENT_PRE_C = 2        # /*
    IN_COMMENT_POST_C = 3       # /*xxx*
    IN_COMMENT_IN_CPP = 5       # //
    in_comment = NOT_IN_COMMENT
    fp = open(file_name, "rb")
    for buf in iter(lambda: fp.read(10 * 1024), ''):
        for c in buf:
            if c == '\r':
                # ignore '\r'
                pass
            elif in_comment == NOT_IN_COMMENT:
                if c == '\n':
                    if valid_line: line_num = line_num + 1
                    valid_line = False
                elif c == '/':
                    in_comment = IN_COMMENT_INIT
                elif c != ' ' and c != '\t':
                    valid_line = True
            elif in_comment == IN_COMMENT_INIT:
                if c == '/':
                    in_comment = IN_COMMENT_IN_CPP
                elif c == '*':
                    in_comment = IN_COMMENT_PRE_C
                else:
                    in_comment = NOT_IN_COMMENT
                    valid_line = True
                    if c == '\n':
                        # no need to check valid_line, cause previous character
                        # must be '/'
                        line_num = line_num + 1
                        valid_line = False
            elif in_comment == IN_COMMENT_PRE_C:
                if c == '\n':
                    if valid_line: line_num = line_num + 1
                    valid_line = False
                elif c == '*':
                    in_comment = IN_COMMENT_POST_C
            elif in_comment == IN_COMMENT_POST_C:
                if c == '/':
                    in_comment = NOT_IN_COMMENT
                elif c == '\n':
                    if valid_line: line_num = line_num + 1
                    valid_line = False
            elif in_comment == IN_COMMENT_IN_CPP:
                if c == '\n':
                    if valid_line: line_num = line_num + 1
                    valid_line = False
                    in_comment = NOT_IN_COMMENT
            else:
                print("unknown in_comment state")
                raise
    # endof for
    fp.close()
    return line_num;

def code_line_stat_file(file_name):
    print("file: %s" % (file_name))
    return code_line_stat_cpp(file_name)

def code_line_stat(file_name):
    if os.path.isfile(file_name):
        return code_line_stat_file(file_name)
    else:
        total_line = 0
        for path, dirs, files in os.walk(file_name):
            total_line = total_line + \
                reduce(lambda accum, x: accum + code_line_stat(x),
                       [os.path.join(path, x)
                        for x in filter(should_stat, files)],
                       0)
        return total_line

def main(argv):
    if len(argv) < 2:
        print("no enough argument")
        return 1
    
    print("code lines: %d" %
          (reduce(lambda total, x: total + x,
                  [code_line_stat(x) for x in argv[1:]])))
    return 0

if __name__ == "__main__":
    main(sys.argv)

