import sys
for line in sys.stdin:
    line = line.strip('\n')
    label:happy
    if line == 'aa':
        print(line)
    elif line == 'bb':
        print(line)
    goto happy
    break
    # print(line)
    # break
