#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


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
    //This is not an array of Creatures as that would make moving them around to different
    //rooms a major pain
    struct Creature *creatures[10];
};


// *** FUNCTION PROTOTYPES ***

struct Room* initializeRooms(int);
struct Creature* initializeCreatures(int, struct Room[]);
void look(void);

// *** GLOBALS ***

struct Room* currentRoom = NULL;

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

    printf("Current room: %i\n", currentRoom->id);

    //Here we gooooo!!!
    bool isUserExit = false;
    int max_size = 256;
    char *command = malloc(max_size);

    while(respect > 0 && respect < 80) {

        //scanf doesn't like input with spaces, plus apparently this is safer
        //in regard to buffer overflows and stuff
        fgets(command, max_size, stdin);

        //Assistance from here: https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf
        //This trims out a new line if one's there
        if ((strlen(command) > 0) && (command[strlen (command) - 1] == '\n'))
            command[strlen (command) - 1] = '\0';

        if(strcmp(command, "exit") == 0) {
            //Leave the loop, self-explanatory
            isUserExit = true;
            break;
        } else if(strcmp(command, "get ye flask") == 0) {
            //Easter egg - don't worry about it
            printf("You can't get ye flask!\n");
        } else if(strcmp(command, "look") == 0) {
            look();
        }
    }

    if(isUserExit){
        printf("Goodbye!");
    } else if(respect <= 0){
        printf("Everyone dislikes your presence!  You lose.");
    } else if(respect >= 80){
        //Could just be an else but if the user leaves the loop otherwise
        //somehow I'd prefer to have nothing happen over undo praise
        printf("Congrats!  Your presence was appreciated!  You win!");
    }

    free(rooms);
    free(creatures);
    free(command);
    return 0;
}

//We do a lil lookin
void look() {
    printf("Room %i, ", currentRoom->id);

    switch(currentRoom->state){
        case 0:
            printf("dirty, ");
            break;
        case 1:
            printf("half-dirty, ");
            break;
        case 2:
            printf("dirty, ");
            break;
    }

    if(currentRoom->eastRoomId == currentRoom->northRoomId == currentRoom->southRoomId == currentRoom->westRoomId == -1){
        printf("no neighbors, ");
    } else {
        printf("neighbors ");
    }

    if(currentRoom->northRoomId != -1) {
        printf("%i to the north, ", currentRoom->northRoomId);
    }
    if(currentRoom->southRoomId != -1){
        printf("%i to the south, ", currentRoom->northRoomId);
    }
    if(currentRoom->eastRoomId != -1){
        printf("%i to the east, ", currentRoom->eastRoomId);
    }
    if(currentRoom->westRoomId != -1) {
        printf("%i to the west, ", currentRoom->westRoomId);
    }

    printf("contains \n");

    for(int i = 0; i < 10; i++){
        struct Creature *creature = currentRoom->creatures[i];
        if (creature == NULL) {
            continue;
        }

        if(creature->creatureType == 0) {
            printf("PC\n");
        } else if(creature->creatureType == 1) {
            printf("animal %i\n", creature->id);
        } else {
            printf("NPC %i\n", creature->id);
        }
    }
}

struct Creature *initializeCreatures(int numberOfCreatures, struct Room rooms[]) {


    struct Creature* creatures = malloc(numberOfCreatures * sizeof(struct Creature));
    int roomCapacity = 10;

    //rooms[0].creatures[0] = &creatures[0];
    for(int i = 0; i < numberOfCreatures; i++) {
        int inputSize = 2; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for Creature #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: creatureType, roomId
        //TODO: Check inputs
        for(int j = 0; j < inputSize; j++) {
            scanf("%i", &inputArray[j]);
        }

        struct Creature creature =
                {
                    .id = i,
                    .creatureType = inputArray[0],
                    .roomId = inputArray[1]
                };

        creatures[i] = creature;

        //Stick the creature's pointer into the first empty slot
        //of the pointer array of the room it's associated with
        for(int j = 0; j < roomCapacity; j++) {
            if(rooms[creature.roomId].creatures[j] == NULL) {
                rooms[creature.roomId].creatures[j] = &creature;
                break;
            }
        }

        //If the creature is the PC, set the currentRoom pointer
        //to the address of its associated room
        if(creature.creatureType == 0) {
            currentRoom = &rooms[creature.roomId];
        }
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

