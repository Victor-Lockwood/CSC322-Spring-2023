#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// *** GLOBALS ***

// ** Main Cache Values **

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


// *** CALCULATED CACHE VALUES ***

//The number of set index bits
int s = 0;

//The number of block offset bits
int b = 0;

//The number of tag bits
int t = 0;


// ** FUNCTION PROTOTYPES **
void initializeGlobals();

int main() {

    initializeGlobals();

    //Valid bit, hit frequency, tag, set number and block bytes
    typedef int line[4 + B];

    //Cache is an array of lines
    line cache[S];

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            cache[i][0] = 0;    //Valid bit is 0
            cache[i][1] = 0;    //Hit frequency is 0
            cache[i][2] = 0;    //TODO: Fill tag appropriately
            cache[i][3] = i;    //Set numbers

            //Everything else in a line are blocks
        }
    }

    unsigned short isDone = 0;
    char input[64];

    unsigned int intInput = 0;

    printf("You may now begin entering memory addresses.  When you are done, simply enter -1.\n");

    while(!isDone) {
        scanf("%s", input);

        //Sketchy but I wanted the actual integer representation to be unsigned
        if(strcmp(input, "-1") == 0) {
            isDone = 1;
        } else {
            //Referred to here: https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
            intInput = (unsigned int) strtol(input, NULL, 16);

            //Gives us our block offset
            //Will need to add 4 to it when actually indexing to account for the other slots taken
            int blockOffsetShift = 32 - b;
            unsigned int blockOffset = intInput << blockOffsetShift;
            blockOffset = blockOffset >> blockOffsetShift;

            //Gives us our set id
            int setOffsetShift = 32 - (s + b);
            unsigned int setNumber = intInput << setOffsetShift;
            setNumber = setNumber >> setOffsetShift;
            setNumber = setNumber >> b;

            //Finally, get our tag
            int tagOffsetShift = 32 - (t + s + b);
            unsigned int tag = intInput << tagOffsetShift;
            tag = tag >> tagOffsetShift;
            tag = tag >> (s + b);

            printf("-- Info for memory address %s --\n", input);
            printf("Block offset: %x (%i)\n", blockOffset, blockOffset);
            printf("Set ID: %x (%i)\n", setNumber, setNumber);
            printf("Tag: %x (%i)\n", tag, tag);
        }
    }

    return 0;
}

//Get user input to fill in the main parameters of the cache
void initializeGlobals() {
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

    s = (int) log2((float) S);

    b = (int) log2((float) B);

    t = m - (s + b);

    printf("\nTo recap, your cache can be described with:\n (%i, %i, %i, %i)\n", S, E, B, m);
    printf("\nAs a result, you have:\n");
    printf("%i index bits,\n", s);
    printf("%i block offset bits and\n", b);
    printf("%i tag bits.\n", t);
    printf("\nYour replacement policy is %s, your hit time is %i and your miss penalty is %i.\n", replacementPolicy, hitTime, missPenalty);
}