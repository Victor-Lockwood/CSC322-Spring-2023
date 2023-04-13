#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LINE_SIZE 5

// *** TYPE DEFINITIONS ***

//Valid bit, hit frequency, tag, set number and last cycle line was hit
typedef unsigned int line[LINE_SIZE];


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

//Time it takes to determine a hit or a miss
int hitTime = 0;

//Additional cycles added on a miss
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

//These are floats because we'll need to calculate the miss rate,
//and integer division will result in 0
float totalCycles = 0;

float totalMissPenalty = 0;


// ** FUNCTION PROTOTYPES **
void initializeGlobals();

int main() {

    initializeGlobals();

    int totalLines = S * E;

    //Cache is an array of lines
    line cache[totalLines];

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            cache[i + j][0] = 0;   //Valid bit is 0
            cache[i + j][1] = 0;   //Hit frequency is 0
            cache[i + j][2] = 0;   //Tag
            cache[i + j][3] = i;   //Set numbers
            cache[i + j][4] = 0;   //Last cycle this was hit at

            //Everything else in a line are blocks
        }
    }

    bool isDone = false;
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

            //DON'T ACTUALLY NEED BLOCK OFFSET -- we'll keep this here for kicks though
            //Gives us our block offset
            //Will need to add 4 to it when actually indexing to account for the other slots taken
            //int blockOffsetShift = 32 - b;
            //unsigned int blockOffset = intInput << blockOffsetShift;
            //blockOffset = blockOffset >> blockOffsetShift;

            //Gives us our set id
            //TODO: Need to use masks instead, ints should also be longs
            int setOffsetShift = 32 - (s + b);
            unsigned int setNumber = intInput << setOffsetShift;
            setNumber = setNumber >> setOffsetShift;
            setNumber = setNumber >> b;

            //Finally, get our tag
            int tagOffsetShift = 32 - (t + s + b);
            unsigned int tag = intInput << tagOffsetShift;
            tag = tag >> tagOffsetShift;
            tag = tag >> (s + b);

            // Debug stuff
            // printf("-- Info for memory address %s --\n", input);
            // printf("Block offset: %x (%i)\n", blockOffset, blockOffset);
            // printf("Set ID: %x (%i)\n", setNumber, setNumber);
            // printf("Tag: %x (%i)\n", tag, tag);

            //Find the line the data would be stored in, if it exists

            bool foundLine = false;
            //TODO: Continue with this, but make the optimized version for direct cache that gets the set directly
            //TODO: Only works for direct cache rn
            totalCycles += hitTime;

            for(int i = 0; i < totalLines; i++) {
                if (cache[i][3] == setNumber) {
                    if (cache[i][0] == 1 && cache[i][2] == tag) { //Check valid bit and tag
                        // Got a hit!
                        cache[i][1] += 1;
                        cache[i][4] = totalCycles; //Lowest number will determine what to evict when LRU - need on every read
                        printf("%s H\n", input);
                    } else {
                        totalCycles += missPenalty;
                        totalMissPenalty += missPenalty;

                        //Got a miss - need to add to cache
                        printf("%s M\n", input);

                        cache[i][0] = 1;
                        cache[i][1] += 1;
                        cache[i][2] = tag;
                        cache[i][4] = totalCycles;
                    }
                }
            }
        }
    }

    int missRate = 0;
    if(totalMissPenalty > 0 && totalCycles > 0) {
        missRate = (int) ( (totalMissPenalty / totalCycles) * 100 );
    }

    printf("%i %i", missRate, (int) totalCycles);

    return 0;
}

//Get user input to fill in the main parameters of the cache\git push --set-upstream origin block-clearout
//TODO: Clear out debug statements
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