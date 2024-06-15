#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h> 
#include<assert.h>
#include<time.h>

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
    assert(stack->top < stack->capacity);
    stack->stack[stack->top] = item;
    stack->top += 1;
}
SearchState pop(SearchStateStack *stack) {
    assert(stack->top > 0);
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
    
    while (stack.top > 0) {
        SearchState state = pop(&stack);
        SearchState new_state;
        int i = state.coord.i;
        int j = state.coord.j;
        char letter = us_state[state.letter_index];

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
        if (result) {
            printf("us_state: %s\n", us_state);
        }
        score += result * census.pops[i];
    }
    return score;
}

char randomise(char solution[5][5]) { 
    // printf("%c", 65 + rand() % 26);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            solution[i][j] = (char) 65 + rand() % 26;
        }
    }
    return solution;
}

int main() { 
    srand(time(NULL));
    char solution[5][5] = {
        {'C', 'I', 'F', 'A', 'A'},
        {'A', 'L', 'A', 'N', 'A'},
        {'A', 'A', 'R', 'N', 'A'},
        {'A', 'A', 'I', 'A', 'A'},
        {'A', 'A', 'A', 'A', 'A'},
    };
  
    Census census;
    read_census(&census);  

    for (int k = 0; k < 1000; k++) {
        randomise(solution);
        int s = score(solution, census);
        if (s > 0) {
            printf("%d\n", s);
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    printf("%c ", solution[i][j]);
                }
                printf("\n");
            } 
        } 
        printf("\n");
    }

    return 0;
}
