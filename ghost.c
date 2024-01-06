
#include "defs.h"

/*
COMMENTS IN DEFS.H
*/

void createGhost(GhostType** ghost, List* rooms){
    *ghost = (GhostType*) malloc(sizeof(GhostType));
    (*ghost)->boredom = 0;
    (*ghost)->room = pickRandRoom(rooms, 1);
    (*ghost)->type = randomGhost();
    (*ghost)->room->ghost = *ghost;
    l_ghostInit((*ghost)->type, (*ghost)->room->name);
}


void moveGhost(GhostType* ghost){
    ghost->room->ghost = NULL;
    ghost->room = pickRandRoom(&(ghost->room->connectedRooms), 0);
    ghost->room->ghost = ghost;
}

void leaveEvidence(GhostType* ghost){
    EvidenceInHouse* newEvidence = NULL;
    EvidenceType type = randomEvid(ghost->type);
    initEvidence(&newEvidence, type);
    if(newEvidence){
        addToList(newEvidence, &(ghost->room->evidenceInRoom));
        l_ghostEvidence(type, ghost->room->name);
    }
}

void initEvidence(EvidenceInHouse** newEvidence, EvidenceType type){
    *newEvidence = (EvidenceInHouse*) malloc(sizeof(EvidenceInHouse));
    (*newEvidence)->evidence = type;

}

void ghostExit(GhostType* ghost){
    ghost->room->ghost = NULL;
    pthread_exit(NULL);
}

void* ghostTurn(void* args){
    GhostType* ghost = (GhostType*) args;
    int ghostActions;
    while(C_TRUE){
        usleep(GHOST_WAIT);
        ghostActions = GHOST_ACTIONS;
        sem_wait(&(ghost->room->huntersInRoom.mutex));
        if(ghost->room->huntersInRoom.size > 0){
            ghost->boredom = 0;
            ghostActions--;
        }else{
            ghost->boredom++;
        }
        sem_post(&(ghost->room->huntersInRoom.mutex));

        if(ghost->boredom >= BOREDOM_MAX){
            l_ghostExit(LOG_BORED);
            ghostExit(ghost);
        }

        int action = randInt(0, ghostActions);
        
        switch(action){
            case 0:
            //do nothing
                break;
            case 1:
                leaveEvidence(ghost);
                break;
            case 2:
                moveGhost(ghost);
                l_ghostMove(ghost->room->name);
                break;
        }
    }
}

