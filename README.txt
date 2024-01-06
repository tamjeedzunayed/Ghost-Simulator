NAME: Kevin Wright, Tamjeed Zunayed
STUDENT NUMBER: 100991075, 101269330

## PROGRAM PURPOSE

This program simulates a haunted house scenario with hunters and a ghost. The house is populated with rooms, and hunters are assigned to explore them. 
A ghost is created within the house, and hunters navigate through rooms to gather evidence or encounter the ghost. The simulation runs in multiple threads, 
each representing a hunter or the ghost. At the end of the simulation, the results are printed, showing the outcome of the encounters and the evidence collected.

## List of Files

COMP2401_KEVIN_WRIGHT_TAMJEED_ZUNAYED_100991075_101269330_FINAL.tar - Archive that contains defs.h, main.c, house.c, ghost.c, room.c, utils.c, logger.c, hunter.c.

defs.h      - Header file containing function and structure declarations, libraries, and constants.
main.c      - The main file that runs the simulation, handling the creation of rooms, hunters, and the ghost, as well as the execution and joining of threads.
utils.c     - Source file containing helper functions to improve readability and implementation of the other source files.
ghost.c     - Source file containing functions related to the GhostType, all of their functionality and control flow.
hunter.c    - Source file containing functions related to the HunterTypes, all of their functionality and control flow.
house.c     - Source file containing functions to initialize the HouseType struct, creating, populating, and freeing all dynamically allocated memory.
room.c      - Source file containing functions to initialize the RoomType struct, creating, connecting rooms, and freeing all dynamically allocated memory.
logger.c    - Source file used to output different print functions for the ghost and hunter actions depending on the state of the enumerator passed in.
README.txt  - Contains this text.

## Compiling and Executing

1. Navigate to the folder containing COMP2401_KEVIN_WRIGHT_TAMJEED_ZUNAYED_100991075_101269330_FINAL.tar in a terminal.
2. Extract files in terminal: tar -xf COMP2401_KEVIN_WRIGHT_TAMJEED_ZUNAYED_100991075_101269330_FINAL.tar
3. Compile the program by typing in the terminal within the directory containing the source files: make
4. Execute the program by typing: ./game

## Program Instructions

Upon running, the program will prompt the user to enter the names of hunters. These hunters are then simulated in the haunted house scenario,
exploring rooms and encountering ghosts. The user does not need to input any commands during the simulation. After all threads have completed,
the program will display the results of the simulation, including encounters with the ghost and evidence collected. 
Finally, the program will clean up all allocated resources and exit.