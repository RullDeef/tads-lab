import sys
from random import shuffle

n = int(sys.argv[1])

nums = list(range(- (n // 2), n // 2))
shuffle(nums)

with open("data.txt", "wt") as file:
    print(*nums, file=file)
