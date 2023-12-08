from collections import Counter

with open('day7.txt') as f:
    data = [line.split() for line in f.readlines()]

data.sort(key=lambda p: [sorted((v for k, v in Counter(p[0]).items()), reverse=True), ['23456789TJQKA'.index(c) for c in p[0]]])
print(sum((i + 1) * int(hand[1]) for i, hand in enumerate(data)))

def handKey(hand):
    p = sorted((v for k, v in Counter(hand).items() if k != 'J'), reverse=True) or [0]
    p[0] += hand.count('J')
    return p, ['J23456789TQKA'.index(c) for c in hand]

data.sort(key=lambda p:  handKey(p[0]))
print(sum((i + 1) * int(hand[1]) for i, hand in enumerate(data)))