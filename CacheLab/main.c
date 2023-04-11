#include <stdio.h>
#include <math.h>

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

    //Fill in those values
    printf("Please enter an integer value for S:\n");
    scanf("%i", &S);

    printf("\nPlease enter an integer value for E:\n");
    scanf("%i", &E);

    printf("\nPlease enter an integer value for B:\n");
    scanf("%i", &B);

    printf("\nPlease enter an integer value for m:\n");
    scanf("%i", &m);


    // *** CALCULATED CACHE VALUES ***

    //The number of set index bits
    int s = (int) log2((float) S);

    //The number of block offset bits
    int b = (int) log2((float) B);

    //The number of tag bits
    int t = m - (s + b);

    printf("\n\ns = %i\n", s);
    printf("b = %i\n", b);
    printf("t = %i\n", t);
}
