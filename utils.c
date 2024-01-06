#include "defs.h"

/*
COMMENTS IN DEFS.H
*/

/*
    Returns a pseudo randomly generated number, in the range min to (max - 1), inclusively
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated integer in the range [0, max-1) 
*/
int randInt(int min, int max){
    return (int) randFloat(min, max);
}

/*
    Returns a pseudo randomly generated floating point number.
    A few tricks to make this thread safe, just to reduce any chance of issues using random
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated floating point number in the range [min, max)
*/
float randFloat(float min, float max) {
    static __thread unsigned int seed = 0;
    if (seed == 0) {
        seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();
    }

    float random = ((float) rand_r(&seed)) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

/* 
    Returns a random enum GhostClass.
*/
enum GhostClass randomGhost() {
    return (enum GhostClass) randInt(0, GHOST_COUNT);
}

/*
    Returns the string representation of the given enum EvidenceType.
        in: type - the enum EvidenceType to convert
        out: str - the string representation of the given enum EvidenceType, minimum 16 characters
*/
void evidenceToString(enum EvidenceType type, char* str) {
    switch (type) {
        case EMF:
            strcpy(str, "EMF");
            break;
        case TEMPERATURE:
            strcpy(str, "TEMPERATURE");
            break;
        case FINGERPRINTS:
            strcpy(str, "FINGERPRINTS");
            break;
        case SOUND:
            strcpy(str, "SOUND");
            break;
        default:
            strcpy(str, "UNKNOWN");
            break;
    }
}

/* 
    Returns the string representation of the given enum GhostClass.
        in: ghost - the enum GhostClass to convert
        out: buffer - the string representation of the given enum GhostClass, minimum 16 characters
*/
void ghostToString(enum GhostClass ghost, char* buffer) {
    switch(ghost) {
        case BANSHEE:
            strcpy(buffer, "Banshee");
            break;
        case BULLIES:
            strcpy(buffer, "Bullies");
            break;
        case PHANTOM:
            strcpy(buffer, "Phantom");
            break;
        case POLTERGEIST:
            strcpy(buffer, "Poltergeist");
            break;
        default:
            strcpy(buffer, "Unknown");
            break;
        
    }
}

void addToList(void* element, List* list){
    sem_wait(&(list->mutex));
    NodeType* newNode = (NodeType*) malloc(sizeof(NodeType));
    newNode->data = element;
    newNode->next = NULL;

    if(list->head == NULL){
        list->head = newNode;
        list->tail = newNode;

    }else{
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size += 1;
    sem_post(&(list->mutex));
}

int removeFromList(void* element, List* list){
    sem_wait(&(list->mutex));
    NodeType* currNode = list->head;
    NodeType* prevNode = NULL;
    int found = C_FALSE;
    while (currNode != NULL){
        if (currNode->data == element){
            found = C_TRUE;
            break;
        }
        prevNode = currNode;
        currNode = currNode->next;
        
    }
    
    if (found){
        if (prevNode != NULL){
            prevNode->next = currNode->next;
        }else{
            list->head = currNode->next;
        }

        if (currNode->next == NULL){
            list->tail = prevNode;
        }
        
        free(currNode);
        sem_post(&(list->mutex));
        list->size -= 1;
        return C_TRUE;
    }
    sem_post(&(list->mutex));
    return C_FALSE;    
}

EvidenceInHouse* removeEvidence(List* list, EvidenceType equipment){
    sem_wait(&(list->mutex));
    NodeType* currNode = list->head;
    NodeType* prevNode = NULL;

    while(currNode != NULL){
        EvidenceInHouse* currEvidence = (EvidenceInHouse*)currNode->data;
        if(currEvidence && currEvidence->evidence == equipment){
            if(prevNode == NULL){
                list->head = currNode->next;
            } else{
                prevNode->next = currNode->next;
            }
            if(list->tail == currNode){
                list->tail = prevNode;
            }
            EvidenceInHouse* temp = currNode->data;
            free(currNode);
            list->size -= 1;
            sem_post(&(list->mutex));
            return temp;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    sem_post(&(list->mutex));
    return NULL;
}


void freeListNodes(List* list){
    NodeType *currNode = list->head;
    NodeType *nextNode = NULL;
    while (currNode != NULL){
        nextNode = currNode->next;
        free(currNode);
        currNode = nextNode;
    }
    list->head=NULL;
    list->tail=NULL;
}

void freeListData(List* list){
    NodeType *currNode = list->head;
    NodeType *nextNode = NULL;
    while (currNode != NULL){
        nextNode = currNode->next;
        free(currNode->data);
        currNode = nextNode;
    }
}

EvidenceType randomEvid(GhostClass ghostClass){
    int index = 0;
    index += ghostClass;
    int length = randInt(0, 3);
    EvidenceType evidenceArray[EV_COUNT] = { EMF, TEMPERATURE, FINGERPRINTS, SOUND };
    for(int i = 0; i < length; i++){
            index++;
            if(index == 4){
                index = 0;
            }
            
        }
    return evidenceArray[index];
}

void initList(List *list) {
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
        sem_init(&(list->mutex), 0, 1);
    }
}

RoomType* pickRandRoom(List* rooms, int increment){
    int roomNumber = randInt(increment, rooms->size);

    NodeType* currentRoom = rooms->head;
    for (int i = 0; i < roomNumber; i++){
        currentRoom = currentRoom->next;
    }
    return currentRoom->data;
}

void printResult(HouseType* house, GhostType* ghost){

    HunterType* scaredHunters[NUM_HUNTERS];
    int list1Size = 0;
    HunterType* boredHunters[NUM_HUNTERS];
    int list2Size = 0;

    NodeType* currNode = house->allHunters.head;
    while (currNode != NULL){
        HunterType* hunter = (HunterType*) currNode->data;
        if (hunter->reasonForLeaving == LOG_BORED){
            boredHunters[list2Size++] = hunter;
        }else if(hunter->reasonForLeaving == LOG_FEAR){
            scaredHunters[list1Size++] = hunter;
        }
        
        currNode = currNode->next;
    }

    printf("====================================\nAll done! Let's tally the results...\n====================================\n------------------------------------\n");
    printf("List of Hunters that left in fear:\n");
    for (int i = 0; i < list1Size; i++){
        printf(" - %s\n", scaredHunters[i]->name);
    }
    printf("\n");
    printf("List of Hunters that left bored:\n");
    for (int i = 0; i < list2Size; i++){
        printf(" - %s\n", boredHunters[i]->name);
    }
    printf("\n");
    
    if (list1Size+list2Size == NUM_HUNTERS){
        printf("All the hunters left bored or in fear \n");
        printf("The ghost has won!\n");
    }else{
        printf("The hunters won!\n");
    }
    printf("Using the evidence they found, ");
    
    GhostClass ghostType = calculateGhost(&house->sharedEvidenceCollected);
    char ghost_str[MAX_STR];
    ghostToString(ghostType, ghost_str);
    if (ghostType != GH_UNKNOWN){
        printf("The hunters were able to determine that the ghost was a %s.\n", ghost_str);
    }else{
        printf("the hunters incorrectly determined the ghost was an %s.\n", ghost_str);
        ghostToString(ghost->type, ghost_str);
        printf("The ghost was actually a %s\n", ghost_str);
    }

    printf("The hunters collected the following Evidence:\n");
    currNode = house->sharedEvidenceCollected.head;
    char str[MAX_STR];
    while (currNode != NULL)
    {
        EvidenceInHouse* ev = (EvidenceInHouse*) currNode->data;
        evidenceToString(ev->evidence, str);
        printf("%5s %s\n", "*", str);
        currNode = currNode->next;
    }
    
}