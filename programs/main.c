#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h> 
#include<assert.h>

typedef struct {
    char us_states[50][20];
    int pops[50];
} Census;

void read_census(Census *census) { 
    FILE *fptr;
    fptr = fopen("data/census.txt", "r");
    char line[100];
    int i = 0;

    while(fgets(line, 100, fptr)) {
        char *us_state;
        char *pop;

        us_state = strtok(line, "\t");
        strcpy(census->us_states[i], us_state);

        pop = strtok(NULL, "\t");
        census->pops[i] = atoi(pop);

        i++;
    }
}

typedef struct {
    int i;
    int j;
} Coord;

typedef struct {
    Coord coord;
    bool extra_life;
    int letter_index;
} SearchState;

typedef struct {
    SearchState stack[256];
    int top;
    int capacity;
} SearchStateStack;

void push(SearchStateStack *stack, SearchState item) {
    // assert(stack->top < stack->capacity);
    stack->stack[stack->top] = item;
    stack->top += 1;
}
SearchState pop(SearchStateStack *stack) {
    // assert(stack->top > 0);
    stack->top -= 1;
    SearchState item = stack->stack[stack->top];
    return item;
}

bool search(char* us_state, char solution[5][5]) {
    SearchStateStack stack = {
        .top = 0, .capacity = 256,
    };
    
    int letter_index = 0;
    SearchState state;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (solution[i][j] == us_state[letter_index]) { 
                state = (SearchState) {
                    .coord = (Coord) { .i = i, .j = j },
                    .extra_life = true,
                    .letter_index = letter_index + 1,
                }; 

            } else {
                state = (SearchState) {
                    .coord = (Coord) { .i = i, .j = j },
                    .extra_life = false,
                    .letter_index = letter_index + 1,
                };
            }
            push(&stack, state); 
        }
    }
    // printf("%d\n", stack.top);

    // for (int i = 0; i < 25; i++) {
    //     SearchState item = stack.stack[i];
    //     printf("%d, %d, %c\n", item.coord.i, item.coord.j, us_state[item.letter_index]);
    // }
    while (stack.top > 0) {
        SearchState state = pop(&stack);
        SearchState new_state;
        int i = state.coord.i;
        int j = state.coord.j;
        char letter = us_state[state.letter_index];

        // printf("(%d, %d) %c, %d\n", i, j, letter, state.extra_life);

        if (letter == '\0') {
            return true;
        }
        for (int di = -1; di < 2; di++) {
            if ((i + di < 0) || (i + di) >= 5) {
                continue;
            }
            for (int dj = -1; dj < 2; dj++) {
                if ((j + dj < 0) || (j + dj >= 5)) {
                    continue;
                }
                if ((di == 0) && (dj == 0)) {
                    continue;
                }
                int new_i = i + di;
                int new_j = j + dj;
                // printf("%d, %d\n", new_i, new_j);
                
                if (solution[new_i][new_j] == letter) {
                    new_state = (SearchState) {
                        .coord = (Coord) { .i = new_i, .j = new_j },
                        .extra_life = state.extra_life,
                        .letter_index = state.letter_index + 1,
                    };
                } else { 
                    if (state.extra_life) {
                        new_state = (SearchState) {
                            .coord = (Coord) { .i = new_i, .j = new_j },
                            .extra_life = false,
                            .letter_index = state.letter_index + 1,
                        };
                    } else {
                        continue;
                    }
                }
                push(&stack, new_state);
            }
        }
    }
    return false;
}

int score(char solution[5][5], Census census) {
    int score = 0;
    for (int i = 0; i < 50; i++) {
        char* us_state = census.us_states[i];
        bool result = search(us_state, solution);
        printf("%s\t%d\n", us_state, result);
        score += result * census.pops[i];
    }
    return score;
}

/*
# initialise variables
$letter = state[0];
$life = 1;
$coordinate = NULL;

# initialise search
loop through all 25 squares looking for $letter

if $letter not found
    life -= 1; 
    recurse with next letter
else 
    $coordinate = Coord {i, j}
    recurse with next $letter and $coordinate
in the recursion, if we reach the last letter, then we return and add the population to the score
*/

/*
Ok, let's see how to do this iteratively

we maintain a stack of coordinates, letters, lives, and then while the stack is not empty, continue processing the next item pushing more stuff onto the stack.  

If we finish all the letters, we break, and chuck the population onto the score 
*/

int main() {
    char solution[5][5] = {
        {'C', 'I', 'F', 'A', 'A'},
        {'A', 'L', 'A', 'N', 'A'},
        {'A', 'A', 'R', 'N', 'A'},
        {'A', 'A', 'I', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'},
    };
   
    Census census;
    read_census(&census);  

    // printf("%d\n", census.pops[1]);
    // printf("%s\n", census.states[1]);
    
    int s = score(solution, census);
    printf("%d", s);

    return 0;
}
