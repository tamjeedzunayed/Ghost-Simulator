#include "defs.h"

/*
COMMENTS IN DEFS.H
*/

int main()
{
    //initialize house
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    //initialize ghost
    GhostType* ghost;
    createGhost(&ghost, &house.rooms);

    //get starting room
    RoomType* startingRoom = (RoomType*)(house.rooms.head->data);

    //initialize hunter variables and thread variables
    char name[MAX_STR];
    pthread_t g;
    pthread_t threads[NUM_HUNTERS];
    HunterType* hunters[NUM_HUNTERS];

    //
    for(int i = 0; i < NUM_HUNTERS; i++){
        printf("Please enter the name of a hunter:\n");
        scanf("%63s", name);
        HunterType* hunter = (HunterType*) malloc(sizeof(HunterType));
        initHunter(name, i, hunter, startingRoom, &house);
        hunters[i] = hunter;
    }
    for (int i = 0; i < NUM_HUNTERS; i++){
        pthread_create(&threads[i], NULL, hunterTurn, hunters[i]);
    }

    pthread_create(&g, NULL, ghostTurn, ghost);


    for(int i = 0; i < NUM_HUNTERS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_join(g, NULL);

   
    printResult(&house, ghost);
    
    freeHouse(&house);
    free(ghost);
    
    
    


    return 0;
}
