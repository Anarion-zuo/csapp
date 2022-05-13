import random

def allocateStr(id, size):
    return 'a ' + str(id) + ' ' + str(size)

def freeStr(id):
    return 'f ' + str(id)

f = open('longexm.rep', mode='w')

f.write("20000000\n")


# number of allocation
# not too big
# or the testing drive does not allocate sufficient space
# resulting in a segment fault
opcount = 100

# largest possible request
# not too big
# or the testing drive does not allocate sufficient space
# resulting in a segment fault
maxReqSize = 4097


f.write(str(opcount) + '\n')
f.write(str(opcount * 2) + '\n')
f.write('1\n')

sizeList = []
for i in range(opcount):
    # let's allocate
    randomSize = random.randint(1, maxReqSize)
    sizeList.append(randomSize)

allocatedCount = 0
canBeFreed = []
while allocatedCount < opcount:
    aOrF = random.randint(0, 2)
    if aOrF == 0:
        print(allocateStr(allocatedCount, sizeList[allocatedCount]))
        f.write(allocateStr(allocatedCount, sizeList[allocatedCount]) + '\n')
        canBeFreed.append(allocatedCount)
        allocatedCount += 1
    else:
        if len(canBeFreed) > 0:
            toBeFreedIndex = random.randint(0, len(canBeFreed) - 1)
            toBeFreedIndex = canBeFreed.pop(toBeFreedIndex)
            print(freeStr(toBeFreedIndex))
            f.write(freeStr(toBeFreedIndex) + '\n')
while len(canBeFreed) > 0:
    toBeFreedIndex = random.randint(0, len(canBeFreed) - 1)
    toBeFreedIndex = canBeFreed.pop(toBeFreedIndex)
    print(freeStr(toBeFreedIndex))
    f.write(freeStr(toBeFreedIndex) + '\n')

f.close()
