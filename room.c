
#include "defs.h"

/*
COMMENTS IN DEFS.H
*/

RoomType* createRoom(char* roomName){
    RoomType* newRoom;
    newRoom = (RoomType*) malloc(sizeof(RoomType));
    strcpy(newRoom->name, roomName);
    newRoom->ghost = NULL;

    initList(&newRoom->connectedRooms);
    initList(&newRoom->huntersInRoom);
    initList(&newRoom->evidenceInRoom);
    
    return newRoom;
}

void connectRooms(RoomType* room1, RoomType* room2){
    addToList(room2, &(room1->connectedRooms));
    addToList(room1, &(room2->connectedRooms));
}

void freeRoom(RoomType* room){
    freeListNodes(&room->connectedRooms);
    freeListNodes(&room->huntersInRoom);
    freeListData(&room->evidenceInRoom);
    freeListNodes(&room->evidenceInRoom);
    free(room);
}