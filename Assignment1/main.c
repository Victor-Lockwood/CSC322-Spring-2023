#include <stdio.h>
#include <stdlib.h>


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
    struct Creature *creatures[10];
};


// *** FUNCTION PROTOTYPES ***

struct Room* initializeRooms(int);

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
    int numberOfRooms = 0;

    printf("Please enter the number of rooms:\n");
    scanf("%i", &numberOfRooms);

    //Initialize the rooms and get a pointer to the array of structs back
    struct Room* rooms = initializeRooms(numberOfRooms);

    printf("First room state: %i\n", rooms[0].state);

    free(rooms);
    return 0;
}

//Initialize the rooms and return a pointer to the array.  The array is dynamically
//allocated and must be freed when no longer in use.
struct Room *initializeRooms(int numberOfRooms) {
    struct Room* rooms = malloc(numberOfRooms * sizeof(struct Room));

    for(int i = 0; i < numberOfRooms; i++) {
        int inputSize = 5;
        int inputArray[inputSize];

        //Grab the five numbers given in the input.
        //In order, they are: state, north, south, east, west
        printf("Enter inputs for room #%i: \n", i);
        for(int j = 0; j < inputSize; j ++){
            scanf("%i", &inputArray[j]);
        }

        struct Room room =
                { .id = i,
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

