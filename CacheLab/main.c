#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LINE_SIZE 5

// *** TYPE DEFINITIONS ***

//Valid bit, hit frequency, tag, set number and last cycle line was hit
typedef unsigned long line[LINE_SIZE];


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

//These are doubles because we'll need to calculate the miss rate,
//and integer division will result in 0
double totalCycles = 0;
double totalRuns = 0;
double totalMisses = 0;


// ** FUNCTION PROTOTYPES **
void initializeGlobals();

int main() {

    initializeGlobals();

    int totalLines = S * E;

    // Cache is an array of lines
    line cache[totalLines];

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            cache[i + j][0] = 0;   //Valid bit is 0
            cache[i + j][1] = 0;   //Hit frequency is 0
            cache[i + j][2] = 0;   //Tag
            cache[i + j][3] = i;   //Set numbers
            cache[i + j][4] = 0;   //Last cycle this was hit at
        }
    }

    bool isDone = false;
    char input[64];

    unsigned int intInput = 0;

    while(!isDone) {
        scanf("%s", input);

        // Sketchy but I wanted the actual integer representation to be unsigned
        if(strcmp(input, "-1") == 0) {
            isDone = 1;
        } else {
            // Referred to here: https://stackoverflow.com/questions/10156409/convert-hex-string-char-to-int
            intInput = (unsigned int) strtol(input, NULL, 16);

            // Gives us our set id
            unsigned long setNumber = intInput >> b; // Clear out block offset bits
            unsigned long setMask = setNumber >> s;  // Clear out set bits for the mask
            setMask = setMask << s;                  // Shift it back
            setNumber = setNumber ^ setMask;         // xor the mask - only s bits should remain

            // Gives us our tag
            unsigned long tag = intInput >> (b + s);
            unsigned long tagMask = tag >> t;
            tagMask = tagMask << t;
            tag = tag ^ tagMask;

            // Debug stuff
            //printf("-- Info for memory address %s --\n", input);
            ////printf("Block offset: %x (%i)\n", blockOffset, blockOffset);
            //printf("Set ID: %x (%i)\n", setNumber, setNumber);
            //printf("Tag: %x (%i)\n", tag, tag);

            //Find the line the data would be stored in, if it exists

            bool foundLine = false;
            totalCycles += hitTime;
            totalRuns += 1;

            for(int i = 0; i < S; i++) {
                if( foundLine ) break;
                if (cache[i][3] == setNumber) {
                    for(int j = 0; j < E; j++) {
                        if (cache[i + j][0] == 1 && cache[i][2] == tag) { //Check valid bit and tag
                            // Got a hit!
                            // Tag and valid bit don't change
                            cache[i + j][1] += 1;
                            cache[i + j][4] = (unsigned long) totalCycles; //Lowest number will determine what to evict when LRU - need on every read
                            printf("%s H\n", input);

                            foundLine = true;
                            break;
                        }
                    }

                    // If we get in here, we don't have what we need in the cache - we need to add it
                    if(!foundLine) {
                        for(int j = 0; j < E; j++) {
                            if(cache[i + j][0] != 1){
                                totalCycles += missPenalty;
                                totalMisses += 1;

                                //Got a miss - need to add to cache
                                printf("%s M\n", input);

                                cache[i + j][0] = 1;
                                cache[i + j][1] += 1;
                                cache[i + j][2] = tag;
                                cache[i + j][4] = (unsigned long) totalCycles;

                                foundLine = true;
                                break;
                            }
                        }
                    }

                    // If we get in here, we don't have what we need in the cache and the set is full - need to evict a line
                    if(!foundLine) {
                        unsigned int index = 0;
                        // Default to the first line in the set
                        unsigned int lowestVal = strcmp(replacementPolicy, "LRU") == 0 ? cache[i][4] : cache[i][1];

                        for(int j = 0; j < E; j++) {
                            // Get least recently used
                            if(strcmp(replacementPolicy, "LRU") == 0) {
                                if(cache[i + j][4] <= lowestVal) {  // Grab the last cycle hit
                                    index = i + j;
                                    lowestVal = cache[index][4];
                                }
                            } else {
                                // Otherwise, get least frequently used
                                if(cache[i + j][1] <= lowestVal) {  // Grab the frequency of hits
                                    index = i + j;
                                    lowestVal = cache[index][1];
                                }
                            }
                        }

                        totalCycles += missPenalty;
                        totalMisses += 1;

                        printf("%s M\n", input);

                        cache[index][0] = 1;
                        cache[index][1] = 1;
                        cache[index][2] = tag;
                        cache[index][4] = (unsigned long) totalCycles;

                        foundLine = true;
                    }
                }
            }

            foundLine = false;
        }
    }

    int missRate = 0;
    if(totalMisses > 0 && totalCycles > 0) {
        missRate = (int) round(( totalMisses / totalRuns ) * 100);
    }

    printf("%i %i\n", missRate, (int) totalCycles);

    //Cave Johnson.  We're done here.
    return 0;
}

//Get user input to fill in the main parameters of the cache
void initializeGlobals() {
    // Input values
    scanf("%i", &S);
    scanf("%i", &E);
    scanf("%i", &B);
    scanf("%i", &m);
    scanf("%s", replacementPolicy);
    scanf("%i", &hitTime);
    scanf("%i", &missPenalty);

    // Calculated values
    s = (int) log2((float) S);
    b = (int) log2((float) B);
    t = m - (s + b);
}