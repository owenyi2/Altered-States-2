with open("data/census.txt", "r") as f:
    census = [line.rstrip().split("\t") for line in  f.readlines()]

class search_state:
    def __init__(self, coord, letter_idx, extra_life, parent):
        self.coord = coord 
        self.letter_idx = letter_idx
        self.extra_life = extra_life
        self.parent = parent 

def search(us_state, solution):
    stack = []
    path = {} 

    letter_idx = 0
    first_letter = us_state[letter_idx]
    for i in range(5):
        for j in range(5):
            if solution[i][j] == first_letter:
                stack.append(
                    search_state((i, j), letter_idx, True, None) 
                )
            else:
                stack.append(
                    search_state((i, j), letter_idx, False, None) 
                )

    while len(stack) > 0:
        state = stack.pop()
        letter_idx = state.letter_idx + 1

        i, j = state.coord
        if letter_idx >= len(us_state):  
            path[letter_idx-1] = state
            for k, v in path.items():
                print(k, v.__dict__, solution[v.coord[0]][v.coord[1]])
            return True
        for di in range(-1, 2, 1):
            for dj in range(-1, 2, 1):
                if i + di < 0 or i + di >= 5 or j + dj < 0 or j + dj >= 5 or (di == 0 and dj == 0):
                    continue

                if solution[i + di][j + dj] == us_state[letter_idx]:
                    path[letter_idx-1] = state
                    stack.append(search_state((i + di, j + dj), letter_idx, state.extra_life, (i, j)))
                else:
                    if state.extra_life:
                        path[letter_idx-1] = state
                        stack.append(search_state((i + di, j + dj), letter_idx, False, (i, j)))
                    else:
                        continue
    return False
                
solution = [r.split(" ") for r in """\
A D H A I
O G T N S
C H E O I
I A R V L
M N L I F
""".split("\n")]


score = 0
for us_state, pop in census:
    if search(us_state, solution):
        score += int(pop)
        print(us_state)
        print()

print(score)
