#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ROOM_CAPACITY 10


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
void alterRoomState(int, bool, int, bool, bool);
int move(int, int, bool, int);

// Helper functions

// I've been a fool.  Made mistakes.  Monumental ones.  I really don't need these first two, but here they
// remain as a reminder of my hubris
struct Room *getRoomPointerFromId(int);
struct Creature *getCreaturePointerFromId(int);

bool commandHandler(int, char *command, int, int, int);
int reactUnfit(int);
void reactCreatureDrill();

// *** GLOBALS ***

//Pointer to the array of creatures
struct Creature *creatures;

//Pointer to the array of rooms
struct Room *rooms;

//Pointer to the PC is currently in
struct Room *currentRoom = NULL;

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

    if(pcId > -1) {
        printf("Current room ID is %i\n", currentRoom->id);
    }
    //Here we gooooo!!!
    bool isUserExit = false;
    int max_size = 14; //it has to be this long to handle "get ye flask"
    char *command = malloc(max_size);

    getchar();

    if(numberOfCreatures == 1) {
        if(pcId == -1) {
            printf("Dang, you don't have a player character!\n");
            printf("That's a little sad.\n");
        } else {
            printf("Looks like you're the only one here.\n");
        }
        printf("There is no way to win.\n");
        printf("There is no way to lose.\n");
        printf("Enjoy your stay in purgatory!  Grab a postcard while you're here.\n");
        printf("Oh wait!  You can't.\n\n");
        printf("Anyway, just type 'exit' without quotes and hit enter when you eventually get bored.\n");
    }
    while(respect > 0 && respect < 80 && !isUserExit) {
        // commandHandler takes care of user input - it's going to get long,
        // so I'd rather it get long down there rather than up here
        isUserExit = commandHandler(max_size, command, pcId, numberOfCreatures, numberOfRooms);
    }

    if(isUserExit){
        printf("Goodbye!\n");
    } else if(respect <= 0){
        printf("Everyone dislikes your presence!  You lose.\n");
    } else if(respect >= 80){
        //Could just be an else but if the user leaves the loop otherwise
        //somehow I'd prefer to have nothing happen over undo praise
        printf("Congrats!  Your presence was appreciated!  You win!\n");
    }

    free(rooms);
    free(creatures);
    free(command);
    return 0;
}

// Handles user input and executes actions accordingly
bool commandHandler(int max_size, char *command, int pcId, int numberOfCreatures, int numberOfRooms) {

    //scanf doesn't like input with spaces, plus apparently this is safer
    //in regard to buffer overflows and stuff
    fgets(command, max_size, stdin);

    //Assistance from here: https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf
    //This trims out a new line if one's there
    if ((strlen(command) > 0) && (command[strlen (command) - 1] == '\n'))
        command[strlen (command) - 1] = '\0';

    if(strlen(command) == 0) {
        printf("Please enter a command.\n");
        return false;
    }

    if(strcmp(command, "exit") == 0) {
        //Leave the loop, self-explanatory
        return true;
    } else if(strcmp(command, "help") == 0) {
        printf("*** AVAILABLE ACTIONS ***\n");
        printf("help          - What you did just now\n");
        printf("exit          - Quit the game; this and 'help' are the only things you can do without a player\n");
        printf("look          - Try to get information about the current room and what creatures are in it\n");
        printf("north         - Try to send the PC north\n");
        printf("<id>:north    - Try to send the creature with id <id> north\n");
        printf("south         - Try to send the PC south\n");
        printf("<id>:south    - Try to send the creature with id <id> south\n");
        printf("east          - Try to send the PC east\n");
        printf("<id>:east     - Try to send the creature with id <id> east\n");
        printf("west          - Try to send the PC west\n");
        printf("<id>:west     - Try to send the creature with id <id> west\n");
        printf("clean         - Try to clean the current room\n");
        printf("<id>:clean    - Try to have creature with id <id> clean the current room\n");
        printf("dirty         - Try to dirty the current room\n");
        printf("<id>:dirty    - Try to have creature with id <id> dirty the current room\n");
        printf("get ye flask  - Never you mind\n");
        return false;
    } else if(pcId == -1) {
        printf("You need a player character to try and do anything other than 'help' or 'exit!\n");
        printf("Think about that next time you play.\n");
        return false;
    }

    //Big ol' if-else-if chunk to handle inputs
    if(strcmp(command, "get ye flask") == 0) {
        // Never you mind
        printf("You can't get ye flask!\n");
    } else if(strcmp(command, "look") == 0) {
        look();
    } else if(strcmp(command, "clean") == 0) {
        alterRoomState(pcId, true, numberOfCreatures, false, false);
    } else if(strcmp(command, "dirty") == 0) {
        alterRoomState(pcId, false, numberOfCreatures, false, false);
    } else if(strcmp(command, "north") == 0){
        if(currentRoom->northRoomId != -1){
            move(currentRoom->northRoomId, pcId, false, numberOfCreatures);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the north, so here you remain.\n");
        }
    } else if(strcmp(command, "south") == 0) {
        if(currentRoom->southRoomId != -1){
            move(currentRoom->southRoomId, pcId, false, numberOfCreatures);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the south, so here you remain.\n");
        }
    } else if(strcmp(command, "east") == 0){
        if(currentRoom->eastRoomId != -1){
            move(currentRoom->eastRoomId, pcId, false, numberOfCreatures);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the east, so here you remain.\n");
        }
    } else if(strcmp(command, "west") == 0) {
        if(currentRoom->westRoomId != -1){
            move(currentRoom->westRoomId, pcId, false, numberOfCreatures);
        } else {
            printf("You gaze into the void.  The void gazes back.  There is no room to the west, so here you remain.\n");
        }
    } else {
        // This block handles making creatures do stuff
        if(numberOfCreatures == 1) {
            printf("What are you trying to do? You're alone here, buddy!\n");
            return false;
        }

        // Here we might get some cases where we have to parse it
        // Referenced: https://stackoverflow.com/questions/46821605/how-to-seperate-user-input-word-delimiter-as-space-using-strtok
        char* creatureId = strtok(command, ":");
        char* action = strtok(NULL, " ");
        //printf("Action: %s\n", action);
        //printf("Creature ID: %s\n", creatureId);

        if(action == NULL) {
            printf("Invalid command. Type \"help\" for a list of commands.\n");
            return false;
        }

        // MAKING CREATURES CLEAN AND DIRTY THE ROOM
        if(strcmp(action, "clean") == 0) {
            alterRoomState(atoi(creatureId), true, numberOfCreatures, false, false);
        } else if(strcmp(action, "dirty") == 0) {
            alterRoomState(atoi(creatureId), false, numberOfCreatures, false, false);

        // MOVING CREATURES
        } else if(strcmp(action, "north") == 0) {
            if(currentRoom->northRoomId != -1){
                move(currentRoom->northRoomId, atoi(creatureId), false, numberOfCreatures);
            } else {
                printf("Creature #%s gazes into the void.  The void gazes back.  There is no room to the north, so here creature #%s remains.\n",
                       creatureId, creatureId);
            }
        } else if(strcmp(action, "south") == 0) {
            if(currentRoom->southRoomId != -1){
                move(currentRoom->southRoomId, atoi(creatureId), false, numberOfCreatures);
            } else {
                printf("Creature #%s gazes into the void.  The void gazes back.  There is no room to the south, so here creature #%s remains.\n",
                       creatureId, creatureId);
            }
        } else if(strcmp(action, "east") == 0) {
            if(currentRoom->eastRoomId != -1){
                move(currentRoom->eastRoomId, atoi(creatureId), false, numberOfCreatures);
            } else {
                printf("Creature #%s gazes into the void.  The void gazes back.  There is no room to the east, so here creature #%s remains.\n",
                       creatureId, creatureId);
            }
        } else if(strcmp(action, "west") == 0) {
            if(currentRoom->westRoomId != -1){
                move(currentRoom->westRoomId, atoi(creatureId), false, numberOfCreatures);
            } else {
                printf("Creature #%s gazes into the void.  The void gazes back.  There is no room to the west, so here creature #%s remains.\n",
                       creatureId, creatureId);
            }
        } else {
            printf("Invalid command. Type \"help\" for a list of commands.\n");
            return false;
        }
    }

    return false;
}


// Given a room's id, return its pointer
// This function is not necessary - you can really just do rooms[roomId] and get the struct -
// but here it remains as a reminder
struct Room *getRoomPointerFromId(int roomId) {
    //I'm leaving the next line commented out as a reminder of how NOT to do this
    //struct Room* roomPointer = rooms + (sizeof(struct Room) * roomId);
    struct Room *roomPointer = rooms + roomId;
    return roomPointer;
}

// Given a creature's id, return its pointer
// This function is not necessary - you can really just do creatures[creatureId] and get the struct -
// but here it remains as a reminder
struct Creature *getCreaturePointerFromId(int creatureId) {
    //struct Creature* activeCreaturePointer = creatures + (sizeof(struct Creature) * creatureId);
    struct Creature* activeCreaturePointer = creatures + creatureId;
    return activeCreaturePointer;
}


// Initialize the rooms and set the global pointer to the array of set rooms.
// The array is dynamically allocated and must be freed when no longer in use.
void initializeRooms(int numberOfRooms) {
    rooms = malloc((numberOfRooms + 1) * sizeof(struct Room));

    for(int i = 0; i < numberOfRooms; i++) {

        int inputSize = 5; //not strictly necessary but makes me feel better
        int inputArray[inputSize];

        printf("Enter inputs for room #%i: \n", i);

        //Grab the five numbers given in the input.
        //In order, they are: state, north, south, east, west
        for(int j = 0; j < inputSize; j ++){
            scanf("%i", &inputArray[j]);
        }

        struct Room *activeRoomPointer = getRoomPointerFromId(i);

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
    creatures = malloc((numberOfCreatures + 1) * sizeof(struct Creature));

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

        struct Creature *activeCreaturePointer = getCreaturePointerFromId(i);

        activeCreaturePointer->id = i;
        activeCreaturePointer->creatureType = inputArray[0];
        activeCreaturePointer->roomId = inputArray[1];

        struct Room *activeRoom = getRoomPointerFromId(activeCreaturePointer->roomId);

        //Stick the creature's pointer into the first empty slot
        //of the pointer array of the room it's associated with
        for(int j = 0; j < ROOM_CAPACITY; j++) {

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
        printf("%i to the south, ", currentRoom->southRoomId);
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
// is not that of the PC, have the specified creature clean.
void alterRoomState(int creatureId, bool isClean, int numberOfCreatures, bool isAutoAction, bool isMove){

    struct Creature *creaturePointer = getCreaturePointerFromId(creatureId);

    struct Room *roomToCleanPointer = currentRoom;
    if(creaturePointer->roomId != currentRoom->id) {
        roomToCleanPointer = getRoomPointerFromId(creaturePointer->roomId);
    }


    if(roomToCleanPointer->state == 0 && isClean) {
        printf("Room already clean - nothing happened.\n");
    } else if(roomToCleanPointer->state == 2 && !isClean) {
        printf("Room already dirty - nothing happened.\n");
    } else {
        // Making a creature do something
        //If it's an autoAction though, do not do this
        if(creaturePointer->creatureType != 0 && !isAutoAction && !isMove) {
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
            roomToCleanPointer->state -= 1;
        } else {
            roomToCleanPointer->state += 1;
        }

        if(!isAutoAction) {
            if(roomToCleanPointer->state == 2) {
                printf("Room is now dirty.\n");
            } else if(roomToCleanPointer->state == 1){
                printf("Room is now half-dirty.\n");
            } else if(roomToCleanPointer->state == 0) {
                printf("Room is now clean.\n");
            }
        }

        // Loop through the current room's creatures, have them react accordingly
        // Only react if it's the current room, though
        if(roomToCleanPointer == currentRoom) {
            for(int i = 0; i<ROOM_CAPACITY; i++) {
                if(roomToCleanPointer->creatures[i] != NULL) {
                    if(roomToCleanPointer->creatures[i] != creaturePointer) {

                        // If we specified a creature to clean, their action's been taken care of
                        if(roomToCleanPointer->creatures[i]->creatureType == 1) {

                            if(isClean) {
                                // Animals like clean rooms - they react positively
                                respect += 1;
                                printf("Animal with ID #%i licks your face.  Respect is now %i.\n", roomToCleanPointer->creatures[i]->id, respect);
                            } else {
                                // Animals don't like dirty rooms - they react negatively
                                respect -= 1;
                                printf("Animal with ID #%i growls.  Respect is now %i.\n", roomToCleanPointer->creatures[i]->id, respect);
                            }

                        } else if(roomToCleanPointer->creatures[i]->creatureType == 2) {
                            if(isClean) {
                                // Humans don't like clean rooms - they react negatively
                                respect -= 1;
                                printf("Human with ID #%i grumbles.  Respect is now %i.\n", roomToCleanPointer->creatures[i]->id, respect);
                            } else {
                                // Humans like dirty rooms - they react positively
                                respect += 1;
                                printf("Human with ID #%i smiles.  Respect is now %i.\n", roomToCleanPointer->creatures[i]->id, respect);
                            }
                        }
                    }
                }
            }

            //This should only run once - if a creature is prompted to do this after a leave attempt,
            // it will be half dirty and should stop any other creatures from trying to leave.
            if(currentRoom->state == 2 || currentRoom->state == 0) {
                reactUnfit(numberOfCreatures);
            }
        }

    }
}

//If the room becomes unfit for a creature type - either dirty or clean - handle it here
int reactUnfit(int numberOfCreatures) {
    int r = rand() % 3;

    bool allRoomsFull = false;

    //If there are no available rooms to move to, we need to know so we can have the creature drill out
    if((currentRoom->northRoomId == -1) &&
       (currentRoom->southRoomId == -1) &&
       (currentRoom->eastRoomId == -1) &&
       (currentRoom->westRoomId == -1)
      )
    {
        allRoomsFull = true;
    }

    struct Room *nextRoomPointer = NULL;

    //Loop through creatures in the room
    for(int i=0; i<ROOM_CAPACITY; i++) {
        if((currentRoom->creatures[i] == NULL) || (currentRoom->creatures[i]->creatureType == 0)) continue;
        struct Creature *currentCreature = currentRoom->creatures[i];
        if((currentCreature->creatureType == 2 && currentRoom->state == 0) ||
           (currentCreature->creatureType == 1 && currentRoom->state == 2))   {
            bool foundRoom = false;

            printf("Current room is unsuitable for creature with ID #%i!\n", currentCreature->id);

            // 4 failures means it's time to drill
            int numberOfFailures = 0;
            int roomIdArray[4];

            //I sacrificed being able to do this easily with an array at start for the sake of readability in the Room struct
            //It is here that I pay the price for a random room order
            switch(r) {
                case 0:
                    roomIdArray[0] = currentRoom->northRoomId;
                    roomIdArray[1] = currentRoom->southRoomId;
                    roomIdArray[2] = currentRoom->eastRoomId;
                    roomIdArray[3] = currentRoom->westRoomId;
                    break;
                case 1:
                    roomIdArray[3] = currentRoom->northRoomId;
                    roomIdArray[0] = currentRoom->southRoomId;
                    roomIdArray[1] = currentRoom->eastRoomId;
                    roomIdArray[2] = currentRoom->westRoomId;
                    break;
                case 2:
                    roomIdArray[2] = currentRoom->northRoomId;
                    roomIdArray[3] = currentRoom->southRoomId;
                    roomIdArray[0] = currentRoom->eastRoomId;
                    roomIdArray[1] = currentRoom->westRoomId;
                    break;
                case 3:
                    roomIdArray[1] = currentRoom->northRoomId;
                    roomIdArray[2] = currentRoom->southRoomId;
                    roomIdArray[3] = currentRoom->eastRoomId;
                    roomIdArray[0] = currentRoom->westRoomId;
                    break;
            }

            //Try to go through each room in the array - we can return if we found what we need
            for(int i = 0; i<4; i++) {
                if(roomIdArray[i] != -1) {
                    if(roomIdArray[i] == currentRoom->id) {
                        if(currentCreature->creatureType == 2 && currentRoom->state == 0) {
                            printf("Creature with ID %i pops back into the room and decides to make a mess!\n", currentCreature->id);
                            alterRoomState(currentCreature->id, false, numberOfCreatures, true, false);
                            //Room is suitable for everyone now (state should be 1) - we can stop
                            return 0;
                        }

                        if(currentCreature->creatureType == 1 && currentRoom->state == 2) {
                            printf("Creature with ID %i pops back into the room and decides to clean up!\n", currentCreature->id);
                            alterRoomState(currentCreature->id, true, numberOfCreatures, true, false);
                            //Room is suitable for everyone now (state should be 1) - we can stop
                            return 0;
                        }

                    }
                    nextRoomPointer = getRoomPointerFromId(roomIdArray[i]);

                    int responseCode =  move(nextRoomPointer->id, currentCreature->id, true, numberOfCreatures);

                    if(responseCode == 0 || responseCode == 1) {
                        foundRoom = true;
                        break;
                    }
                    if(responseCode == -1) numberOfFailures++;
                } else {
                    numberOfFailures++;
                }
            }

            if(foundRoom){
                foundRoom = false;
                continue;
            }
            if(numberOfFailures == 4 || allRoomsFull) {
                //Here's where the creature would drill out of the house
                //We don't return here - any other creatures needing to leave due to room conditions still
                //need to do so
                printf("Creature can't move to a room! Creature with ID #%i drills out of the house due to your negligence!\n", currentCreature->id);
                currentRoom->creatures[i] = NULL;
                reactCreatureDrill();
            } else {
                printf("Uh oh, you shouldn't have gotten here!  Something went wrong.\n");

                return -1;
            }
        }
    }

    return 1;
}


void reactCreatureDrill() {

    for(int i = 0; i<ROOM_CAPACITY; i++) {
        if(currentRoom->creatures[i] != NULL) {
            if(currentRoom->creatures[i]->creatureType == 1) {
                // Animals growl for chasing someone out
                respect -= 1;
                printf("Animal with ID #%i growls.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
            } else if(currentRoom->creatures[i]->creatureType == 2) {
                // Humans grumble for chasing someone out.
                respect -= 1;
                printf("Human with ID #%i grumbles.  Respect is now %i.\n", currentRoom->creatures[i]->id, respect);
            }

        }
    }
}

// Move the creature into the specified room
// Return -1 if the creature cannot go into the room, 0 if
// the creature moved into the same room, and 1 if the creature was able to move successfully
// isLeave marks whether this is a creature trying to leave a room due to bad conditions.  Set to false
// when the PC is moving or trying to move another creature
int move(int roomId, int creatureId, bool isLeave, int numberOfCreatures) {

    if(creatureId >= numberOfCreatures) {
        printf("Creature with ID #%i does not exist!\n", creatureId);
        return -1;
    }

    struct Creature *creaturePointer = getCreaturePointerFromId(creatureId);

    if(roomId == currentRoom->id){
        if(creaturePointer->creatureType == 0){
            printf("Turns out that door was a portal!  You pop back into room #%i.\n", currentRoom->id);
            return 0;
        } else {
            printf("Turns out that door was a portal!  Creature #%i pops back into room #%i.\n", creatureId, currentRoom->id);
            return 0;
        }
    }

    // Grab pointers for the room to move into and the creature being moved
    struct Room *newRoomPointer = getRoomPointerFromId(roomId);

    bool foundSpace = false;
    int oldRoomIndex = -1;

    bool foundInOldSpace = false;
    // Clear out the creature's pointer from the old room
    for(int i=0; i<ROOM_CAPACITY; i++) {
        if(currentRoom->creatures[i] == creaturePointer) {
            oldRoomIndex = i;
            foundInOldSpace = true;
            break;
        }
    }

    if(!foundInOldSpace) {
        printf("Creature with ID %i was not found in the current room with ID %i.\n", creatureId, roomId);
        return -1;
    }

    // Make sure the new room has enough space to stick the creature in
    for(int i=0; i<ROOM_CAPACITY; i++) {
        if(newRoomPointer->creatures[i] == NULL) {
            newRoomPointer->creatures[i] = creaturePointer;
            foundSpace = true;
            break;
        }
    }

    // If the room has no space, let the user know and return
    if(!foundSpace) {
        if(!isLeave) {
            // Not interested in this print out for auto actions
            printf("Room #%i is at full capacity - cannot move creature #%i into there.\n", roomId, creatureId);
            if(creaturePointer->creatureType == 1) {
                printf("Creature with ID #%i growls.\n", creatureId);
                respect--;
            } else if(creaturePointer->creatureType == 2) {
                printf("Creature with ID #%i grumbles.\n", creatureId);
                respect--;
            }
        }
        return -1;
    }

    currentRoom->creatures[oldRoomIndex] = NULL;


    //If the creature was the PC, update the currentRoom
    if(creaturePointer->creatureType == 0) {
        currentRoom = newRoomPointer;
    }

    creaturePointer->roomId = newRoomPointer->id;

    //Adjust the room accordingly
    if(newRoomPointer->state == 0 && creaturePointer->creatureType==2) {
        printf("Room #%i is too clean for creature #%i!  Room will now be dirtied.\n", roomId, creatureId);
        alterRoomState(creatureId, false, numberOfCreatures, isLeave, true);
    } else if(newRoomPointer->state == 2 && creaturePointer->creatureType == 1){
        printf("Room #%i is too dirty for creature #%i!  Room will now be cleaned.\n", roomId, creatureId);
        alterRoomState(creatureId, true, numberOfCreatures, isLeave, true);
    }


    printf("Creature #%i moved to room #%i\n", creatureId, roomId);

    return 1;
}

