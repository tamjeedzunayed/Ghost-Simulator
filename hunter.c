
#include "defs.h"

/*
COMMENTS IN DEFS.H
*/

void initHunter(char* newName, EvidenceType equipment, HunterType* hunter, RoomType* startingRoom, HouseType* house){
    
    strcpy(hunter->name, newName);
    hunter->currentRoom = startingRoom;
    hunter->sharedEvidence = &house->sharedEvidenceCollected;
    hunter->equipment = equipment;
    hunter->boredom = 0;
    hunter->fear = 0;
    hunter->reasonForLeaving = LOG_UNKNOWN;
    addToList(hunter, &startingRoom->huntersInRoom);
    addToList(hunter, &house->allHunters);
    l_hunterInit(hunter->name, hunter->equipment);
}


void moveHunter(HunterType* hunter){
    removeFromList(hunter, &(hunter->currentRoom->huntersInRoom));
    hunter->currentRoom = pickRandRoom(&(hunter->currentRoom->connectedRooms), 0);
    addToList(hunter, &(hunter->currentRoom->huntersInRoom));
}

int collectEvidence(HunterType* hunter){
    EvidenceInHouse* evidenceRemoved = removeEvidence(&(hunter->currentRoom->evidenceInRoom), hunter->equipment);
    if(evidenceRemoved != NULL){
        int evidenceFound = checkEvidence(hunter->sharedEvidence, hunter->equipment);
        if(evidenceFound){
            free(evidenceRemoved);
        } else{
            addToList(evidenceRemoved, hunter->sharedEvidence);
        }
        return C_TRUE;
    }
    return C_FALSE;
}

int reviewEvidence(HunterType* hunter){
    if(hunter->sharedEvidence->size == 3){
        return C_TRUE;
    }
    return C_FALSE;
}

void hunterExit(HunterType* hunter){
    removeFromList(hunter, &(hunter->currentRoom->huntersInRoom));
    pthread_exit(NULL);
}

void* hunterTurn(void* args){
    HunterType* hunter = (HunterType*) args;
    int inHouse = C_TRUE;

    while (inHouse){
        usleep(HUNTER_WAIT);
        if(hunter->currentRoom->ghost != NULL){
            hunter->fear += FEAR_RATE;
            hunter->boredom = 0;
        }else{
            hunter->boredom++;
        }

        
        if(hunter->fear >= FEAR_MAX || hunter->boredom >= BOREDOM_MAX){
            if(hunter->fear >= FEAR_MAX){
                l_hunterExit(hunter->name, LOG_FEAR);
                hunter->reasonForLeaving = LOG_FEAR;
            }else{
                l_hunterExit(hunter->name, LOG_BORED);
                hunter->reasonForLeaving = LOG_BORED;
            }
            
            hunterExit(hunter);
            inHouse = C_FALSE;
            break;
        }
        int action = randInt(0, HUNTER_ACTIONS);
        
        switch(action){
            case 0:
                moveHunter(hunter);
                l_hunterMove(hunter->name, hunter->currentRoom->name);
                break;
            case 1:
                if(collectEvidence(hunter)){
                    l_hunterCollect(hunter->name, hunter->equipment, hunter->currentRoom->name);
                }
                break;
            case 2:
                if(reviewEvidence(hunter)){
                    l_hunterReview(hunter->name, LOG_SUFFICIENT);
                    l_hunterExit(hunter->name, LOG_EVIDENCE);
                    hunter->reasonForLeaving = LOG_EVIDENCE;
                    hunterExit(hunter);
                    inHouse = C_FALSE;
                } else{
                    l_hunterReview(hunter->name, LOG_INSUFFICIENT);
                }
                break;
        }
    }
    return NULL;
}

int checkEvidence(List* list, EvidenceType type){
    sem_wait(&(list->mutex));
    NodeType* currNode = list->head;
    while(currNode != NULL){
        EvidenceInHouse* newEvidence = (EvidenceInHouse*)(currNode->data);
        if(type == newEvidence->evidence && newEvidence){
            sem_post(&(list->mutex));
            return C_TRUE;
        }
        currNode = currNode->next;
    }
    sem_post(&(list->mutex));
    return C_FALSE;
}

GhostClass calculateGhost(List* list){
    if (list->size != 3)
        return GH_UNKNOWN; 
    NodeType* currNode = list->head;
    int number = 0;
    EvidenceInHouse* evidenceIns;
    while (currNode != NULL){
        evidenceIns = (EvidenceInHouse*) currNode->data;
        number += evidenceIns->evidence;
        currNode = currNode->next;
    }
    switch (number){
        case 3:
            return POLTERGEIST;
            break;
        case 6:
            return PHANTOM;
            break;
        case 5:
            return BULLIES;
            break;
        case 4:
            return BANSHEE;
            break;
    default:
        return GH_UNKNOWN;
        break;
    }
}