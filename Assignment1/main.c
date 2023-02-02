#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


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

// Scaffolding
void initializeRooms(int);
int initializeCreatures(int);

// Actions
void look(void);
void alterRoomState(int, bool);
void move(int, int);

// Helper functions
struct Room* getRoomPointerFromId(int);
struct Creature* getCreaturePointerFromId(int);

bool commandHandler(int, char *command, int);

// *** GLOBALS ***

//Pointer to the first element in the array of creatures
struct Creature* creatures;

//Pointer to the first element in the array of rooms
struct Room* rooms;

//Pointer to the room the PC is currently in
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
    int pcId;
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


    initializeRooms(numberOfRooms);

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

    //We get the player's id back
    pcId = initializeCreatures(numberOfCreatures);

    printf("Current room: %i\n", currentRoom->id);

    //Here we gooooo!!!
    bool isUserExit = false;
    int max_size = 256;
    char *command = malloc(max_size);

    getchar();
    while(respect > 0 && respect < 80 && !isUserExit) {
        // commandHandler takes care of user input - it's going to get long,
        // so I'd rather it get long down there rather than up here
        isUserExit = commandHandler(max_size, command, pcId);
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

// Handles user input and executes actions accordingly
bool commandHandler(int max_size, char *command, int pcId) {

    //scanf doesn't like input with spaces, plus apparently this is safer
    //in regard to buffer overflows and stuff
    fgets(command, max_size, stdin);

    //Assistance from here: https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf
    //This trims out a new line if one's there
    if ((strlen(command) > 0) && (command[strlen (command) - 1] == '\n'))
        command[strlen (command) - 1] = '\0';

    //Big ol' if-else-if chunk to handle inputs
    if(strcmp(command, "exit") == 0) {
        //Leave the loop, self-explanatory
        return true;
    } else if(strcmp(command, "get ye flask") == 0) {
        // Never you mind
        printf("You can't get ye flask!\n");
    } else if(strcmp(command, "look") == 0) {
        look();
    } else if(strcmp(command, "clean") == 0) {
        alterRoomState(pcId, true);
    } else if(strcmp(command, "dirty") == 0) {
        alterRoomState(pcId, false);
    } else if(strcmp(command, "north") == 0){
        if(currentRoom->northRoomId != -1){
            move(currentRoom->northRoomId, pcId);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the north, so here you remain.\n");
        }
    } else if(strcmp(command, "south") == 0) {
        if(currentRoom->southRoomId != -1){
            move(currentRoom->southRoomId, pcId);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the south, so here you remain.\n");
        }
    } else if(strcmp(command, "east") == 0){
        if(currentRoom->eastRoomId != -1){
            move(currentRoom->eastRoomId, pcId);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the east, so here you remain.\n");
        }
    } else if(strcmp(command, "west") == 0) {
        if(currentRoom->westRoomId != -1){
            move(currentRoom->westRoomId, pcId);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the west, so here you remain.\n");
        }
    } else if(strcmp(command, "help") == 0) {
        printf("*** AVAILABLE ACTIONS ***\n");
        printf("help    - What you did just now\n");
        printf("look    - Get information about the current room and what creatures are in it\n");
        printf("north   - Try to send the PC north\n");
        printf("south   - Try to send the PC south\n");
        printf("east    - Try to send the PC east\n");
        printf("west    - Try to send the PC west\n");
        printf("clean   - Try to clean the current room\n");
        printf("dirty   - Try to dirty the current room\n");
        printf("exit    - Quit the game\n");

    } else {
        // This block handles making creatures do stuff

        // Here we might get some cases where we have to parse it
        // Referenced: https://stackoverflow.com/questions/46821605/how-to-seperate-user-input-word-delimiter-as-space-using-strtok
        char* creatureId = strtok(command, ":");
        char* action = strtok(NULL, " ");
        //printf("Action: %s\n", action);
        //printf("Creature ID: %s\n", creatureId);

        if(strcmp(action, "clean") == 0) {
            alterRoomState(atoi(creatureId), true);
        } else if(strcmp(action, "dirty") == 0) {
            alterRoomState(atoi(creatureId), false);
        } else {
            printf("Invalid command. Type \"help\" for a list of commands.\n");
            return false;
        }
    }

    return false;
}


// Given a room's id, return its pointer
// TODO: Some sort of check to see if the room exists should be in place.  Preferably not here, but putting the note here while I think of it.
struct Room* getRoomPointerFromId(int roomId) {
    struct Room* roomPointer = rooms + (sizeof(struct Room) * roomId);
    return roomPointer;
}

// Given a creature's id, return its pointer
// TODO: Some sort of check to see if the creature exists should be in place.  Preferably not here, but putting the note here while I think of it.
struct Creature* getCreaturePointerFromId(int creatureId) {
    struct Creature* activeCreaturePointer = creatures + (sizeof(struct Creature) * creatureId);
    return activeCreaturePointer;
}


// Initialize the rooms and set the global pointer to the array of set rooms.
// The array is dynamically allocated and must be freed when no longer in use.
void initializeRooms(int numberOfRooms) {
    rooms = malloc(numberOfRooms * sizeof(struct Room));

    for(int i = 0; i < numberOfRooms; i++) {

        int inputSize = 5; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for room #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: state, north, south, east, west
        for(int j = 0; j < inputSize; j ++){
            scanf("%i", &inputArray[j]);
        }

        struct Room* activeRoomPointer = getRoomPointerFromId(i);

        activeRoomPointer->id = i;
        activeRoomPointer->state = inputArray[0];
        activeRoomPointer->northRoomId = inputArray[1];
        activeRoomPointer->southRoomId = inputArray[2];
        activeRoomPointer->eastRoomId = inputArray[3];
        activeRoomPointer->westRoomId = inputArray[4];

        //clear out the creature pointer array
        for(int j = 0; j < 10; j++) {
            activeRoomPointer->creatures[j] = NULL;
        }

    }

}

// Initialize the creatures and set the global pointer to the array of set rooms, as
// well as stick the pointer of each creature into the associated creature list
// of the room it's in and set currentRoom to the pointer of the room the PC is in.
// The array is dynamically allocated and must be freed when no longer in use.
int initializeCreatures(int numberOfCreatures) {
    creatures = malloc(numberOfCreatures * sizeof(struct Creature));
    int roomCapacity = 10;
    int pcId = -1;

    //rooms[0].creatures[0] = &creatures[0];
    for(int i = 0; i < numberOfCreatures; i++) {
        int inputSize = 2; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for Creature #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: creatureType, roomId
        for(int j = 0; j < inputSize; j++) {
            scanf("%i", &inputArray[j]);
        }

        struct Creature* activeCreaturePointer = getCreaturePointerFromId(i);
        //struct Creature creature = *activeCreaturePointer;

        activeCreaturePointer->id = i;
        activeCreaturePointer->creatureType = inputArray[0];
        activeCreaturePointer->roomId = inputArray[1];

        struct Room* activeRoom = getRoomPointerFromId(activeCreaturePointer->roomId);

        //Stick the creature's pointer into the first empty slot
        //of the pointer array of the room it's associated with
        for(int j = 0; j < roomCapacity; j++) {

            if(activeRoom->creatures[j] == NULL) {
                activeRoom->creatures[j] = activeCreaturePointer;
                break;
            }
        }

        //If the creature is the PC, set the currentRoom pointer
        //to the address of its associated room
        if(activeCreaturePointer->creatureType == 0) {
            currentRoom = activeRoom;
            pcId = i;
        }
    }

    return pcId;
}

// Describes the room, its properties and the creatures in the current room
void look() {
    printf("Room %i, ", currentRoom->id);

    switch(currentRoom->state){
        case 0:
            printf("clean, ");
            break;
        case 1:
            printf("half-dirty, ");
            break;
        case 2:
            printf("dirty, ");
            break;
    }

    if(
            currentRoom->eastRoomId == -1 &&
            currentRoom->northRoomId == -1 &&
            currentRoom->southRoomId == -1 &&
            currentRoom->westRoomId == -1
            )
    {
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
            printf("human %i\n", creature->id);
        }
    }
}

// Cleans the current room.  If creatureId
// is not -1, have the specified creature clean.
// TODO: Implement creature reactions
void alterRoomState(int creatureId, bool isClean){
    int roomCapacity = 10;


    struct Creature* creaturePointer = getCreaturePointerFromId(creatureId);

    if(creaturePointer->roomId != currentRoom->id) {
        printf("Creature with ID #%i is not in the current room!  No cleaning can take place here.\n", creatureId);
        return;
    }


    if(currentRoom->state == 0 && isClean) {
        printf("Room already clean - nothing happened.\n");
    } else if(currentRoom->state == 2 && !isClean) {
        printf("Room already dirty - nothing happened.\n");
    } else {
        // TODO: If the room is clean, humans should leave

        // Making a creature do something
        if(creaturePointer->creatureType != 0) {
            if(creaturePointer->creatureType == 2){
                if(isClean) {
                    respect -= 3;
                    printf("Human #%i grumbles a lot.  Respect is now %i.\n", creaturePointer->id, respect);
                } else {
                    respect += 3;
                    printf("Human #%i smiles a lot.  Respect is now %i.\n", creaturePointer->id, respect);
                }
            } else {
                if(isClean) {
                    respect += 3;
                    printf("Animal #%i licks your face a lot.  Respect is now %i.\n", creaturePointer->id, respect);
                } else {
                    respect -= 3;
                    printf("Animal #%i growls a lot.  Respect is now %i.\n", creaturePointer->id, respect);
                }
            }
        }

        // Update room state accordingly
        if(isClean) {
            currentRoom->state -= 1;
        } else {
            currentRoom->state += 1;
        }

        if(currentRoom->state == 2) {
            printf("Room is now dirty.\n");
        } else if(currentRoom->state == 1){
            printf("Room is now half-dirty.\n");
        } else if(currentRoom->state == 0) {
            printf("Room is now clean.\n");
        }

        // Loop through the current room's creatures, have them react accordingly
        for(int i = 0; i<roomCapacity; i++) {
            if(currentRoom->creatures[i] != NULL) {
                if(currentRoom->creatures[i] != creaturePointer) {

                    // If we specified a creature to clean, their action's been taken care of
                    if(currentRoom->creatures[i]->creatureType == 1) {

                        if(isClean) {
                            // Animals like clean rooms - they react positively
                            respect += 1;
                            printf("Animal with ID #%i licks your face.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
                        } else {
                            // Animals don't like dirty rooms - they react negatively
                            respect -= 1;
                            printf("Animal with ID #%i growls.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
                        }

                    } else if(currentRoom->creatures[i]->creatureType == 2) {
                        if(isClean) {
                            // Humans don't like clean rooms - they react negatively
                            respect -= 1;
                            printf("Human with ID #%i grumbles.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
                        } else {
                            // Humans like dirty rooms - they react positively
                            respect += 1;
                            printf("Human with ID #%i smiles.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
                        }
                    }
                }
            }
        }

    }
}

// Move the creature into the specified room
void move(int roomId, int creatureId) {
    int roomCapacity = 10;
    struct Creature* creaturePointer = getCreaturePointerFromId(creatureId);

    if(roomId == currentRoom->id){
        if(creaturePointer->creatureType == 0){
            printf("Turns out that door was a portal!  You pop back into room #%i.\n", currentRoom->id);
            return;
        } else {
            printf("Turns out that door was a portal!  Creature #%i pops back into room #%i.\n", creatureId, currentRoom->id);
            return;
        }
    }

    // Grab pointers for the room to move into and the creature being moved
    //TODO: Make sure the room exists!
    struct Room* newRoomPointer = getRoomPointerFromId(roomId);

    bool foundSpace = false;

    // Make sure the new room has enough space to stick the creature in
    for(int i=0; i<roomCapacity; i++) {
        if(newRoomPointer->creatures[i] == NULL) {
            newRoomPointer->creatures[i] = creaturePointer;
            foundSpace = true;
            break;
        }
    }

    // If the room has no space, let the user know and return
    if(!foundSpace) {
        printf("Room #%i is at full capacity - cannot move creature #%i into there.\n", roomId, creatureId);
        return;
    }

    bool foundInOldSpace = false;
    // Clear out the creature's pointer from the old room
    for(int i=0; i<roomCapacity; i++) {
        if(currentRoom->creatures[i] == creaturePointer) {
            currentRoom->creatures[i] = NULL;
            foundInOldSpace = true;
            break;
        }
    }

    //Something pretty bad happened!  Let the user know
    if(!foundInOldSpace) {
        printf("Something went wrong - creature with ID %i was not found in the current room with ID %i.\n", creatureId, roomId);
        return;
    }

    //If the creature was the PC, update the currentRoom
    if(creaturePointer->creatureType == 0) {
        currentRoom = newRoomPointer;
    }

    printf("Creature #%i moved to room #%i\n", creatureId, roomId);

    return;
}