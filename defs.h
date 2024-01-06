#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define FEAR_RATE       3
#define LOGGING         C_TRUE
#define HUNTER_ACTIONS  3
#define GHOST_ACTIONS   3

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;
struct NodeType;
struct RoomType;


enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, PHANTOM, BULLIES, BANSHEE, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };



typedef struct NodeType{
    void* data;
    struct NodeType* next;
} NodeType;

typedef struct {
    NodeType* head;
    NodeType* tail;
    int size;
    sem_t mutex;
} List;

typedef struct {
    List allHunters;
    List rooms;
    List sharedEvidenceCollected;
} HouseType;

typedef struct Room{
    char name[MAX_STR];
    List connectedRooms;
    List huntersInRoom;
    List evidenceInRoom;
    struct GhostType* ghost;
} RoomType;

typedef struct GhostType{
    RoomType* room;
    int boredom;
    GhostClass type;
} GhostType;


typedef struct{
    char name[MAX_STR];
    RoomType* currentRoom;
    EvidenceType equipment;
    List* sharedEvidence;
    int fear;
    int boredom;
    enum LoggerDetails reasonForLeaving;
} HunterType;

typedef struct {
    EvidenceType evidence;
} EvidenceInHouse;



// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter+

/* Function: addToList
   Description: Adds a new element to the end of the list.
   Input: element   - Pointer to the element to be added.
   Input: list      - Pointer to the list where the element will be added.
   Output:          - The list with the new element added.
   Returns: void
*/
void addToList(void* element, List* list);
/* Function: removeFromList
   Description: Removes an element from the list if it exists. The function uses a semaphore to 
                handle concurrency in a multithreaded environment.
   Input: element   - A pointer to the element to be removed.
   Input: list      - A pointer to the list from which to remove the element.
   Output:          - Updates the list by removing the specified element if it is found.
   Returns: C_TRUE if the element was found and removed, C_FALSE otherwise.
 */
int removeFromList(void* element, List* list);

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

/* 
   Function: addRoom
   Description: Adds a new room to the list of rooms. This is a wrapper function for addToList,
                specifically handling the addition of RoomType elements.
   Input: list  - A pointer to the list to which the room will be added.
   Input: room  - A pointer to the room to be added to the list.
   Output:      - The list is updated with the new room added to it.
   Returns: void
 */
void addRoom(List* list, RoomType* room);

/* 
   Function: initHouse
   Description: Initializes the house structure by setting up lists for all hunters, rooms, and shared evidence.
                Each list is initialized to be empty with the initList function.
   Input: house     - A pointer to the house structure that is to be initialized.
   Output:          - The house structure pointed to by the input parameter is modified with initialized lists.
   Returns: void
 */
void initHouse(HouseType* house);

/* 
   Function: createRoom
   Description: Allocates memory for a new room and initializes it with the provided name.
                The room's lists for connected rooms, hunters in the room, and evidence in the room are also initialized.
   Input: roomName  - A pointer to a character array containing the name of the room to be created.
   Output:          - A pointer to the newly allocated and initialized RoomType structure.
   Returns: RoomType* - A pointer to the newly created room structure.
 */
RoomType* createRoom(char* roomName);

/* 
   Function: connectRooms
   Description: Establishes a bidirectional connection between two rooms by adding each room to the other's list of connected rooms.
   Input: room1     - A pointer to the first room to be connected.
   Input: room2     - A pointer to the second room to be connected.
   Output: Updates the connectedRooms lists for both room1 and room2 to include each other.
   Returns: void
 */
void connectRooms(RoomType* room1, RoomType* room2);

/* 
   Function: initHunter
   Description: Initializes a hunter structure with a name, equipment, and starting room. It also
                sets the hunter's shared evidence pointer to the house's shared evidence list and logs the initialization.
   Input: newName       - A pointer to a character array containing the new name for the hunter.
   Input: equipment     - The type of equipment assigned to the hunter.
   Input: hunter        - A pointer to the hunter structure to be initialized.
   Input: startingRoom  - A pointer to the RoomType structure representing the hunter's starting room.
   Input: house         - A pointer to the HouseType structure representing the house where the hunter is located.
   Output: The hunter structure is modified with the provided details and added to the starting room's list of hunters
           and the house's list of all hunters.
   Returns: void
 */
void initHunter(char* newName, EvidenceType equipment, HunterType* hunter, RoomType* startingRoom, HouseType* house);

/* 
   Function: moveHunter
   Description: Moves a hunter from their current room to a random connected room. The function
                removes the hunter from the current room's list of hunters and adds them to the
                new room's list of hunters.
   Input/Output: hunter    - A pointer to the HunterType structure representing the hunter to be moved.
   Returns: void
 */
void moveHunter(HunterType* hunter);

/* 
   Function: collectEvidence
   Description: Attempts to collect evidence in the hunter's current room using the hunter's equipment.
                If evidence is found and not already present in the shared evidence, it is added to the shared list.
   Input: hunter - A pointer to the HunterType structure representing the hunter collecting evidence.
   Output: Modifies the shared evidence list if new evidence is found and not already present.
   Returns: C_TRUE if evidence was collected, C_FALSE otherwise.
 */
int collectEvidence(HunterType* hunter);

/* 
   Function: randomEvid
   Description: Generates a random type of evidence based on the ghost class.
   Input: ghostClass    - The class of the ghost which influences the type of evidence generated.
   Returns: EvidenceType - The type of evidence generated.
 */
EvidenceType randomEvid(GhostClass ghostClass);

/* 
   Function: createGhost
   Description: Allocates memory for a new ghost and initializes it with default values.
                The ghost is assigned to a random room and given a random type. The ghost's presence
                is also recorded in the assigned room.
   Input: ghost - A pointer to a pointer of GhostType where the new ghost structure will be stored.
   Input: rooms - A pointer to a list of rooms from which one will be randomly chosen for the ghost.
   Output:      - The newly created ghost is assigned to a random room and its type is set.
   Returns: void
 */
void createGhost(GhostType** ghost, List* rooms);

/* 
   Function: pickRandRoom
   Description: Selects a random room from a list of rooms. The randomness is influenced by a starting index 
                (increment) and the total number of rooms.
   Input: rooms     - A pointer to the List structure containing the rooms.
   Input: increment - An integer value used to adjust the starting index for random selection.
   Returns: RoomType* - A pointer to the randomly selected room.
 */
RoomType* pickRandRoom(List* rooms, int increment);

/*
   Function: moveGhost
   Description: Moves the ghost to a randomly selected connected room. The function updates the ghost's 
                current room and ensures that the ghost's presence is removed from the previous room and set in the new one.
   Input: ghost - A pointer to the GhostType structure representing the ghost to be moved.
   Returns: void
 */
void moveGhost(GhostType* ghost);

/* 
   Function: leaveEvidence
   Description: Allows a ghost to leave evidence in its current room. The type of evidence left is determined 
                randomly based on the ghost's type. The evidence is added to the room's list of evidence.
   Input: ghost - A pointer to the GhostType structure representing the ghost leaving evidence.
   Output:      - The room's list of evidence is updated with the new evidence if it's successfully created.
   Returns: void
 */
void leaveEvidence(GhostType* ghost);

/* 
   Function: initEvidence
   Description: Allocates memory for and initializes a new EvidenceInHouse structure. Sets the evidence type 
                to the provided type.
   Input: newEvidence   - A pointer to a pointer of EvidenceInHouse where the new evidence structure will be stored.
   Input: type          - The type of evidence to be assigned to the new evidence structure.
   Output:              - A new EvidenceInHouse structure is created and initialized with the specified evidence type.
   Returns: void
 */
void initEvidence(EvidenceInHouse** newEvidence, EvidenceType type);

/* 
   Function: initList
   Description: Initializes a List structure by setting its head and tail to NULL, size to 0, 
                and initializing its semaphore for thread-safe operations.
   Input: list - A pointer to the List structure to be initialized.
   Output:     - The List structure is initialized with default values and a semaphore.
   Returns: void
 */
void initList(List *list);

/* 
   Function: removeEvidence
   Description: Searches for and removes a specific type of evidence from a list. The function operates in 
                a thread-safe manner using a semaphore. If the specified evidence is found, it is removed from the list.
   Input: list      - A pointer to the List structure containing the evidence.
   Input: equipment - The type of evidence to be removed.
   Output:          - The list is updated by removing the specified evidence if found.
   Returns: EvidenceInHouse* - A pointer to the removed evidence if found, NULL otherwise.
 */
EvidenceInHouse* removeEvidence(List* list, EvidenceType equipment);

/* 
   Function: checkEvidence
   Description: Checks if a specific type of evidence is present in a given list. The function operates in 
                a thread-safe manner using a semaphore. It iterates through the list to find if the specified 
                evidence type is present.
   Input: list  - A pointer to the List structure containing evidence items.
   Input: type  - The type of evidence to check for in the list.
   Returns: C_TRUE if the specified evidence type is found in the list, C_FALSE otherwise.
 */
int checkEvidence(List* list, EvidenceType type);

/* 
   Function: populateRooms
   Description: Initializes and connects a series of rooms within a house. This function creates various rooms,
                connects them to form a network, and then adds each room to the house's list of rooms. 
                The connections between rooms are bidirectional.
   Input: house - A pointer to the HouseType structure where the rooms will be added.
   Output:      - The house's rooms list is populated with newly created and interconnected rooms.
   Returns: void
 */
void populateRooms(HouseType* house);

/* 
   Function: calculateGhost
   Description: Determines the type of ghost based on the collected evidence.
   Input: list - A pointer to the List structure containing the collected evidence items.
   Returns: GhostClass - The class of the ghost as determined by the evidence, or GH_UNKNOWN if it cannot be determined.
 */
GhostClass calculateGhost(List* list);

/* 
   Function: hunterTurn
   Description: Represents a hunter's turn in the simulation. The hunter performs actions based on the situation in the house, 
                such as moving, collecting evidence, and reviewing evidence. The hunter's fear and boredom levels are 
                also updated. The turn ends if the hunter reaches maximum fear or boredom, or if sufficient evidence is collected.
   Input: args      - A pointer to the HunterType structure representing the hunter's state and actions.
   Output:          - Updates the hunter's state based on the actions taken and the situation in the house.
   Returns: void* - Always returns NULL. The return type is void* to comply with pthreads' requirements.
 */
void* hunterTurn(void* args);

/* 
   Function: ghostTurn
   Description: Represents a ghost's turn in the simulation. The ghost can take actions such as leaving evidence or moving to another room. 
                The ghost's boredom level is also updated based on the presence of hunters in the room. The turn ends if the ghost 
                reaches maximum boredom.
   Input: args      - A pointer to the GhostType structure representing the ghost's state and actions.
   Output:          - Updates the ghost's state and the state of the house based on the actions taken.
   Returns: void* - Always returns NULL. The return type is void* to comply with pthreads' requirements.
 */
void* ghostTurn(void* args);

/* 
   Function: freeListData
   Description: Frees the memory allocated for the data stored in each node of a List. 
                This function traverses the list and frees the data pointed to by each node.
   Input: list      - A pointer to the List structure whose data elements are to be freed.
   Returns: void
 */
void freeListData(List* list);

/* 
   Function: freeListNodes
   Description: Frees the memory allocated for the nodes of a List structure. 
                The function traverses the list and frees each node.
   Input: list      - A pointer to the List structure whose nodes are to be freed.
   Returns: void
 */
void freeListNodes(List* list);

/* 
   Function: freeRoom
   Description: Frees the memory allocated for a RoomType structure and its associated lists. 
                It frees both the data and nodes of the connectedRooms, huntersInRoom, and evidenceInRoom lists.
   Input: room      - A pointer to the RoomType structure to be freed.
   Returns: void
 */
void freeRoom(RoomType* room);

/*
  Function: freeHouse
  Description: Frees the memory allocated for a HouseType structure, including all rooms and lists 
               associated with the house. This function iterates through each room, freeing them, 
               and then proceeds to free the nodes and data of the other lists.
  Input: house - A pointer to the HouseType structure to be freed.
  Returns: void
 */
void freeHouse(HouseType* house);

/*
  Function: printResult
  Description: Prints the final results of the simulation, including lists of hunters who left due to fear or boredom, 
               the outcome of the game (whether the ghost or hunters won), the ghost's type determined by the hunters, 
               and the collected evidence.
  Input: house - A pointer to the HouseType structure containing information about the hunters and evidence.
  Input: ghost - A pointer to the GhostType structure representing the ghost.
  Returns: void
 */
void printResult(HouseType* house, GhostType* ghost);

/*
  Function: reviewEvidence
  Description: Evaluates the collected evidence by a hunter to determine if the required amount for 
               drawing a conclusion about the ghost's presence has been met. 
  Input: hunter - A pointer to the HunterType structure containing the hunter's details including 
                  the shared evidence list.
  Returns: C_TRUE if the shared evidence list size is three, indicating enough evidence has been 
           collected to confirm the ghost's presence. Returns C_FALSE otherwise.
 */
int reviewEvidence(HunterType* hunter);