#!/usr/bin/python

import sys
import time
TRACE = False
N = 4
T0 = 0
RESULT = None
def x_r(s, left, right):
    if left == right:
        if left == 0:
            if TRACE:
                sys.stderr.write('\n')
            else:
                if RESULT is not None:
                    RESULT.append(s)
                else:
                    print s
            return
        if TRACE:
            sys.stderr.write('(')
            time.sleep(T0)
        x_r(s + '(', left - 1, right)
    elif left < right:
        if left > 0:
            if TRACE:
                sys.stderr.write('(')
                time.sleep(T0)
            x_r(s + '(', left - 1, right)
        if left > 0:
            if TRACE:
                sys.stderr.write(s)
        if TRACE:
            sys.stderr.write(')')
            time.sleep(T0)
        x_r(s + ')', left, right - 1)
    else:
        # unreachable
        assert True == False, 'left > right: unbalanced'

if __name__ == '__main__':
    RESULT = []
    x_r('', N, N)
    for s in RESULT:
        print s
