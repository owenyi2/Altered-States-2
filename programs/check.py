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
    full = {} 

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
        try: 
            full[state.letter_idx].append(state)
        except KeyError:
            full[state.letter_idx] = [state] 
        letter_idx = state.letter_idx + 1

        i, j = state.coord
        if letter_idx >= len(us_state):
            letter_idx -= 1
            extra_life = True
            print(state.coord, state.letter_idx, solution[state.coord[0]][state.coord[1]])
            while letter_idx > 0:
                steps = full[letter_idx - 1]
                steps.reverse() 
                for step in steps:
                    if step.coord == state.parent:
                        state = step
                        letter_idx -= 1
                        print(state.coord, state.letter_idx, solution[state.coord[0]][state.coord[1]])
                        break
            # for key, value in full.items():
            #     print(key, [s.__dict__ for s in value])

            return True
        for di in range(-1, 2, 1):
            for dj in range(-1, 2, 1):
                if i + di < 0 or i + di >= 5 or j + dj < 0 or j + dj >= 5 or (di == 0 and dj == 0):
                    continue

                if solution[i + di][j + dj] == us_state[letter_idx]:
                    stack.append(search_state((i + di, j + dj), letter_idx, state.extra_life, (i, j)))
                else:
                    if state.extra_life:
                        stack.append(search_state((i + di, j + dj), letter_idx, False, (i, j)))
                    else:
                        continue
    return False
                

solution = [
    ['N', 'T', 'F', 'I', 'O'],
    ['G', 'E', 'X', 'L', 'C'],
    ['S', 'A', 'O', 'R', 'A'],
    ['H', 'I', 'N', 'B', 'I'],
    ['G', 'O', 'G', 'U', 'I']
]

for us_state, pop in census:
    if search(us_state, solution):
        print(us_state)
        print()
