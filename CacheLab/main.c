#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// ** GLOBALS **
int *cache;

// ** FUNCTION PROTOTYPES **
void initializeCache(int, int, int, int, int, int, int);

int main() {
    // *** MAIN CACHE VALUES ***

    //The number of sets
    int S = 0;

    //The number of lines per set
    int E = 0;

    //The block size, in bytes
    int B = 0;

    //The number of address bits
    int m = 0;

    //TODO: Figure out what these are for
    int hitTime = 0;

    int missPenalty = 0;

    //Either LFU or LRU
    char replacementPolicy[3] = "LFU";

    //Fill in those values
    printf("Please enter an integer value for S:\n");
    scanf("%i", &S);

    printf("Please enter an integer value for E:\n");
    scanf("%i", &E);

    printf("Please enter an integer value for B:\n");
    scanf("%i", &B);

    printf("Please enter an integer value for m:\n");
    scanf("%i", &m);

    printf("Please enter an integer value for hit time:\n");
    scanf("%i", &hitTime);

    printf("Please enter an integer value for miss penalty:\n");
    scanf("%i", &missPenalty);

    unsigned short isDone = 0;
    while (!isDone){
        printf("Please enter the replacement policy:\n");
        scanf("%s", replacementPolicy);

        if(!(strcmp(replacementPolicy, "LFU") == 0 || strcmp(replacementPolicy, "LRU") == 0)) {
            printf("Invalid replacement policy.  Options are LFU (Least Frequently Used) or LRU (Least Recently Used).\n");
        } else {
            isDone = 1;
        }
    }

    // *** CALCULATED CACHE VALUES ***

    //The number of set index bits
    int s = (int) log2((float) S);

    //The number of block offset bits
    int b = (int) log2((float) B);

    //The number of tag bits
    int t = m - (s + b);

    printf("\nTo recap, your cache can be described with:\n (%i, %i, %i, %i)\n", S, E, B, m);
    printf("\nAs a result, you have:\n");
    printf("%i index bits,\n", s);
    printf("%i block offset bits and\n", b);
    printf("%i tag bits.\n", t);
    printf("\nYour replacement policy is %s, your hit time is %i and your miss penalty is %i.\n", replacementPolicy, hitTime, missPenalty);

    initializeCache(S, E, B, m, s, b, t);

    isDone = 0;
    char input[2];

    //We need this signed because of the -1, but we should be fine anyway since no memory addresses are negative
    int intInput = 0;

    printf("You may now begin entering memory addresses.  When you are done, simply enter -1.\n");

    while(!isDone) {
        scanf("%s", input);

        //Referred to here: https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
        intInput = strtol(input, NULL, 16);

        if(intInput == -1) {
            isDone = 1;
        } else {
            printf("%i\n", intInput);
        }
    }

    free(cache);
    return 0;
}

void initializeCache(int S, int E, int B, int m, int s, int b, int t) {
    //Valid bit, hit frequency, tag, set number and block bytes
    typedef int line[4 + B];

    //Cache is an array of lines
    //line cache[S];
    cache = malloc(S * E * sizeof(line));

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            int arrayOffset = 4 * i;    //Four cells for sure to offset by
            if(j > 0) arrayOffset *= j;
            int blockArrayOffset = (i + j) * B;
            arrayOffset += blockArrayOffset;
            cache[i + j + arrayOffset] = 0;        //Valid bit is 0
            cache[i + j + arrayOffset + 1] = 0;    //Hit frequency is 0
            cache[i + j + arrayOffset + 2] = 0;    //TODO: Fill tag appropriately
            cache[i + j + arrayOffset + 3] = i;    //Set numbers
        }
    }
}