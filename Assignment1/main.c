#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// *** STRUCTS ***
struct Creature {
    //Self-explanatory, the ID of the creature
    int id;

    //Type of the creature: 0 = PC, 1 = animal, 2 = NPC
    int creatureType;

    //ID of the room the creature is currently in
    int roomId;
};

struct Room {
    //Self-explanatory, the ID of the room
    int id;

    //State values: 0 for clean, 1 for half-dirty, 2 for dirty
    int state;

    //ID of the room to the north of this one; -1 if there is none
    int northRoomId;

    //ID of the room to the south of this one; -1 if there is none
    int southRoomId;

    //ID of the room to the south of this one; -1 if there is none
    int eastRoomId;

    //ID of the room to the west of this one; -1 if there is none
    int westRoomId;

    //Array of pointers to the creatures in the room
    //This is nit an array of Creatures as that would make moving them around to different
    //rooms a major pain
    struct Creature *creatures[10];
};


// *** FUNCTION PROTOTYPES ***

struct Room* initializeRooms(int);
struct Creature* initializeCreatures(int, struct Room[]);

// *** GLOBALS ***

//Main metric of the game
//If this hits 0 or under, player loses
//If this hits 80 or above, player wins
int respect = 40;

int main() {
    printf("                    | |                                              \n"
           "  ___ _ __ ___  __ _| |_ _   _ _ __ ___    __ _  __ _ _ __ ___   ___ \n"
           " / __| '__/ _ \\/ _` | __| | | | '__/ _ \\  / _` |/ _` | '_ ` _ \\ / _ \\\n"
           "| (__| | |  __/ (_| | |_| |_| | | |  __/ | (_| | (_| | | | | | |  __/\n"
           " \\___|_|  \\___|\\__,_|\\__|\\__,_|_|  \\___|  \\__, |\\__,_|_| |_| |_|\\___|\n"
           "                                           __/ |                     \n"
           "                                          |___/ \n");

    int numberOfCreatures = 0;
    int numberOfRooms = 0;
    bool isValid = false;

    //Get the number of rooms to create, validating the input is within range
    while(!isValid) {
        printf("Please enter the number of rooms (between 1 and 100 inclusive):\n");
        scanf("%i", &numberOfRooms);

        if(numberOfRooms <= 100 && numberOfRooms >= 1) {
            isValid = true;
        } else {
            printf("Invalid input.\n");
        }
    }
    isValid = false;

    //Initialize the rooms and get a pointer to the array of structs back
    struct Room* rooms = initializeRooms(numberOfRooms);

    //Get the number of creatures to create, validating the input is within range
    while(!isValid){
        printf("Please enter the number of creatures (between 1 and 100 inclusive):\n");
        scanf("%i", &numberOfCreatures);

        if(numberOfCreatures <= 100 && numberOfCreatures >=1) {
            isValid = true;
        } else {
            printf("Invalid input.\n");
        }
    }
    isValid = false;

    //Initialize the creatures and get a pointer to the array of structs back
    struct Creature* creatures = initializeCreatures(numberOfCreatures, rooms);

    //printf("Rooms first pointer: %p", rooms[0].creatures[0]);

    free(rooms);
    free(creatures);
    return 0;
}

struct Creature *initializeCreatures(int numberOfCreatures, struct Room rooms[]) {


    struct Creature* creatures = malloc(numberOfCreatures * sizeof(struct Creature));

    //rooms[0].creatures[0] = &creatures[0];
    for(int i = 0; i < numberOfCreatures; i++) {
        int inputSize = 2; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for Creature #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: creatureType, roomId
        //TODO: Check inputs
        for(int j = 0; j < numberOfCreatures; j++) {
            scanf("%i", &inputArray[j]);
        }

        struct Creature creature =
                {
                    .id = i,
                    .creatureType = inputArray[0],
                    .roomId = inputArray[1]
                };

        creatures[i] = creature;
    }

    return creatures;
}

//Initialize the rooms and return a pointer to the array.  The array is dynamically
//allocated and must be freed when no longer in use.
struct Room *initializeRooms(int numberOfRooms) {
    struct Room* rooms = malloc(numberOfRooms * sizeof(struct Room));

    for(int i = 0; i < numberOfRooms; i++) {
        int inputSize = 5; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for room #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: state, north, south, east, west
        //TODO: Check inputs
        for(int j = 0; j < inputSize; j ++){
            scanf("%i", &inputArray[j]);
        }

        struct Room room =
                {
                        .id = i,
                        .state = inputArray[0],
                        .northRoomId = inputArray[1],
                        .southRoomId = inputArray[2],
                        .eastRoomId = inputArray[3],
                        .westRoomId = inputArray[4]
                };

        //clear out the creature pointer array
        for(int j = 0; j < 10; j++) {
            room.creatures[j] = NULL;
        }

        //stick the struct in the rooms array
        rooms[i] = room;
    }

    return rooms;
}

