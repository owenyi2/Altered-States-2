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

        // printf("%c, (%d, %d)\n", letter, i, j);

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

int score_print(char solution[5][5], Census census) {
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

int score(char solution[5][5], Census census) {
    int score = 0;
    for (int i = 0; i < 50; i++) {
        char* us_state = census.us_states[i];
        bool result = search(us_state, solution);
        score += result * census.pops[i];
    }
    return score;
}

void randomise(char solution[5][5]) { 
    // printf("%c", 65 + rand() % 26);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            solution[i][j] = (char) 65 + rand() % 26;
        }
    }
}

void print_solution(char solution[5][5]) {    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%c ", solution[i][j]);
        }
        printf("\n");
    } 
    printf("\n");
}

const int GENERATION_SIZE = 1000;

int sample(int fitness[GENERATION_SIZE], int max_fitness) {
    bool choice = false;
    int ind;

    while (!choice) {
        ind = rand() % GENERATION_SIZE;
        float r = ((float)rand()/(float)(RAND_MAX));
        float s = ((float)fitness[ind]/(float)(max_fitness));

        choice = r <= s;
    }
    return ind;
}

void cross_over(char result[5][5], char ind_A[5][5], char ind_B[5][5]) {
    for (int i = 0; i < 3; i++) { 
        for (int j = 0; j < 5; j++) {
            result[i][j] = ind_A[i][j];
        } 
    } 
    for (int i = 3; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            result[i][j] = ind_A[i][j];
        } 
    }
}

void mutate(char ind[5][5]) {
    for (int i = 0; i < 3; i++) { 
        ind[rand() % 5][rand() % 5] = (char)(65 + rand() % 26);
    }
}

int main() {
    srand(time(NULL));
    char generation[GENERATION_SIZE][5][5];
    char next_generation[GENERATION_SIZE][5][5];

    char (*pgeneration)[GENERATION_SIZE][5][5] = &generation;
    char (*pnext_generation)[GENERATION_SIZE][5][5] = &next_generation;
    char (*temp)[GENERATION_SIZE][5][5];

    int fitness[GENERATION_SIZE];

    Census census;
    read_census(&census);  

    int generational_max_fitness = 0;
    int max_fitness = 0;
    int max_ind = 0;

    for (int ind = 0; ind < GENERATION_SIZE; ind++) {
        randomise(generation[ind]);
        fitness[ind] = score(generation[ind], census);
        if (fitness[ind] >= max_fitness) {
            max_fitness = fitness[ind];
            max_ind = ind;
        }
    }
    printf("%d\n", max_fitness);

    while (true) { 
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {     
                (*pnext_generation)[0][i][j] = (*pgeneration)[max_ind][i][j];
            }
        }
        int n = 1;
        while (n < GENERATION_SIZE) {
            int ind_A = sample(fitness, max_fitness);
            int ind_B = sample(fitness, max_fitness);
            
            cross_over((*pnext_generation)[n], (*pgeneration)[ind_A], (*pgeneration)[ind_B]);
            mutate((*pnext_generation)[n]);
            n++;
        }

        // swap generation with next_generation 
        temp = pnext_generation;
        pnext_generation = pgeneration;
        pgeneration = pnext_generation;
        
        max_fitness = 0;
        for (int ind = 0; ind < GENERATION_SIZE; ind++) {
            fitness[ind] = score((*pgeneration)[ind], census);
            
            if (fitness[ind] >= max_fitness) {
                max_fitness = fitness[ind];
                max_ind = ind;
            }
        }
        printf("%d\n", max_fitness);
        if ((max_fitness >= 165379868) && max_fitness > (generational_max_fitness)) {
            generational_max_fitness = max_fitness;
            print_solution(generation[max_ind]);
        }

    }
}

