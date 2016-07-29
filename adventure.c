/*******************************
 *
* Joshua Reed
* reedjosh.adventure.c
* CS 344 (Operating Systems)
* OregonState EECS
* Winter, 2015
*
* This program is a cave game. 
* The user given a room name and
* connections to other rooms. 
*
* The user is expected to find
* his way to the finish room. 
* 
* Once the user has, it plays
* back the user's path.
*
* The rooms are stored in
* individual files in a directory
* named reedjosh.rooms.<process id>
*
*******************************/

#include <stdio.h>
#include <string.h>
#include <dirent.h> // This is needed to manipulate directories
#include <unistd.h> // Used to get PID
#include <sys/stat.h> // For file manipulation
#include <sys/types.h> // For file manipulation
#include <time.h> // To seed the random number function
#include <stdlib.h>
#include <assert.h> // Asserts for testing

typedef struct room room; // I can now use "room" variables without first typing struct
struct room{ // Each room contains an array of doors and a number of doors
	room * door[6]; // Array of doors within the room
	int numDoors; // The number of doors the room has
	char * name; // Room name
	int type; // (-1 = Begining), (0 = Middle), (1 = Finish)
	};

void initRoom(room *, const char *); // Inits a passed room pointer with a given name
void makeCave(room *[]); // Makes the cave for usage in the game
void mixCave(room *[]); // Swaps the cave rooms randomly
void makePath(room *[]); // Populates a cave with linked rooms
void linkRooms(room*, room*); // Used to link two doors together
void addRandDoors(room *[]); // Randomly add doors until each room has 3-6 doors
void printCave(room *[]); // Print out the full cave with stats(for testing)
void fileCave(room *[]); // Writes the cave into files
void play(room *[]); // User interface and file to track user's path
void printRoom(room *); // Print room name and possible connections
void failMessage(); // How!?
void freeCave(room *[]); // Frees the dynamic memory used by the cave
int getMove(room *[], room *); // Get the next room to move to and return its position in the cave array
int hasDoor(room *, room *); // Returns 1 if door exists and 0 otherwise

int main(void){
	srand(time(NULL)); // Seed random number function
	room * cave[10]; // This array of rooms will store the rooms in the constructed cave
	makeCave(cave); // This makes the cave (an array of room pointers) and inits it
	fileCave(cave); // Put the rooms in files
	play(cave); // Play!
	return(0);
	}

void play(room *cave[]){ // This prompts and plays the game when given a cave
	int roomNum=0, i=0, heroPath[100]; // current position in cave
	while((cave[roomNum]->type) != 1){ // Loop until the user enters the final room
		roomNum = getMove(cave, cave[roomNum]);
		heroPath[i] = roomNum; // Record their journey
		i++;
		if(i>98){ // They have entered so many rooms they actually lost
			failMessage(); // How!?
			}
	}
	int j;
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", i);
	for(j=0; j<i; j++){ // Print the path they took
		printf("%s\n",cave[heroPath[j]]->name);
		}
	freeCave(cave); // Deallocate the memory used by the rooms
	}

void freeCave(room *cave[]){
	int i;
	for(i=0; i<10; i++){
		free(cave[i]->name); // Dynamic name 
		free(cave[i]); // Array of pointers was malloced
		}
	}

void failMessage(){ // Prints a message regarding the player's complete failure
		printf("Half due to laziness, and half due to your epic failure you are seeing this message.\n");
		printf("How did you make 98 moves and not find the finish room? Well, anyways this message is\n");
		printf("here to prevent buffer overflow and to tell you how bad you lost! In fact your failure\n");
		printf("is so gross that you've even made the program exit fail."); 
		exit(1);
	}

int getMove(room * cave[], room* aRoom){
	size_t length = 0;  // Used for getline buffer
	char * inString = NULL; // String for getline
	int badInput=1, roundAgain=0,i,doorNum, inLength;
	while(badInput==1){ // Prompt until the user pics a room that they can move to
		if(roundAgain == 1){ // If the have done this once already print I didn't hear you message
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
			}
		printRoom(aRoom);
		printf("WHERE TO? >");
		inLength = getline(&inString, &length, stdin); // Get input and length of input
		inString[inLength-1] = '\0'; // Use length of input to remove trailing \n
		for(i=0; i<aRoom->numDoors; i++){ // Search through the available rooms for a match
			if(!strcmp(inString, aRoom->door[i]->name)){ // A match has occured
				badInput=0; // Exit while loop
				doorNum = i; // This is the spot in the door list that the match was found
				}
			}
		roundAgain = 1; // Set flag to prompt I didn't hear message
		}
	i = 0;
	while(aRoom->door[doorNum] != cave[i]){ // Iterate until the door matches the cave room number
		i++;
		}
	return (i); // return the room number. 
	}
	

void printRoom(room * aRoom){ // Prints the current location and connections
	printf("CURRENT LOCATION: %s\n", aRoom->name);
	printf("POSSIBLE CONNECTIONS:");
	int i;
	for(i=0; i<aRoom->numDoors; i++){
		printf(" %s", aRoom->door[i]->name); // Prints all of the connections
		}
	printf(".\n");
	}

void initRoom(room * aRoom, const char * name){ // Make memory for the name, and set init vals
	aRoom->name = malloc(sizeof(name));
	aRoom->type = 0;
	aRoom->numDoors = 0;
	strcpy(aRoom->name, name); 
	}

void mixCave(room * cave[]){ // Randomly mix up the room ordering
	room * temp;
	int randNum;
	int i;
	for(i=0; i<200 ;i++){ // Mix the rooms up now that they're initiated
		randNum = rand() % 10; // Returns a random 0-9 integer
		temp = cave[1];
		cave[1] = cave[randNum]; // Swap locations
		cave[randNum] = temp;
		}
	}

void makeCave(room * cave[]){
	int i;
	for(i=0; i<10 ;i++){ // Make the memory for the array of rooms
		cave[i] = malloc(sizeof(room));
		}
	// Inits rooms with names (will be shuffled)
	initRoom(cave[0], "Tryxan"); 
	initRoom(cave[1], "Delvis");
	initRoom(cave[2], "Slam"); 
	initRoom(cave[3], "Trick");
	initRoom(cave[4], "Rax");
	initRoom(cave[5], "aFendel"); 
	initRoom(cave[6], "Spen");
	initRoom(cave[7], "Blue");
	initRoom(cave[8], "Fopp");
	initRoom(cave[9], "Peclis");
	mixCave(cave); // Randomly Swaps Rooms around
	makePath(cave); // Makes a path from start to finish
	addRandDoors(cave); // Add in random doors until each room has 3-6 doors	
	}

void makePath(room *cave[]){ // Generate a path from start to finish of length 2 to 7
	int pathLength = (rand() % 6) + 2; // Generate an int between 2 and 7
	int i;
	cave[0]->type = -1; // Make this the starting room
	cave[pathLength-1]->type = 1; // Make this the end room
	for(i=0; i<pathLength-1; i++){
		linkRooms(cave[i], cave[i+1]); // Link a winning path
		}
	}

void linkRooms(room* r1, room* r2){ // Used to link two doors together
	assert(r1->numDoors < 6); // Don't add another if the room already has six
	assert(r2->numDoors < 6); // Don't add another if the room already has six
	r1->door[r1->numDoors] = r2; // Add r2 to r1's list of doors
	r1->numDoors++;	// Increment r1's doorcount
	r2->door[r2->numDoors] = r1; // Add r1 to r2's list of doors
	r2->numDoors++;	// Increment r2's doorcount
	}

// Returns int > 0 if there is a door between the two rooms
// Returns 0 if the room doesn't have a door to the requested room
int hasDoor(room * r1, room * r2){
	int i, boolean = 0;
	for(i=0; i<r1->numDoors ;i++){
		if(r1->door[i] == r2){ // If rooms are connected
			boolean = 1;
			}
		}
	return (boolean);
	}

void addRandDoors(room *cave[]){ // Now that a winning path exists, this adds random doors 
	int i, randRoomNum;
	for(i=0; i<7; i++){ // Loop through the 7 rooms
		while(cave[i]->numDoors < 3){ // Loop until enough doors are added
			randRoomNum = rand() % 7;
			if( (i != randRoomNum) && (!hasDoor(cave[i], cave[randRoomNum])) ){
				// Don't add a door it already has, or a door to itself
				 linkRooms(cave[i], cave[randRoomNum]);
				}
			}
		}
	}

/***************************************************
void printCave(room * cave[]){ // This is a diagnostic print for before I had the file writing done
	int i;
	int j;
	for(i=0; i<10 ;i++){
		if(cave[i]->numDoors){
			printf("Connection between %d and %d (0 or 1): %d\n", i, i+1, hasDoor(cave[i], cave[i+1]));
			printf("%d).\nRoom: %s \nType: %d \nNumDoors: %d \nDoors: ", i+1, cave[i]->name, cave[i]->type, cave[i]->numDoors);
			for(j=0; j<cave[i]->numDoors; j++){
				printf("%s ", cave[i]->door[j]->name);
				}
			printf("\n\n");
			}
		}
	}
***************************************************/

void fileCave(room * cave[]){ // Puts the cave into a directory with a file for each.
	FILE * pFile;
	char directory[100]; // For use as a directory name
	char file[100]; // For file writing
	sprintf(directory, "./joshua.maps.%d", (int)getpid()); // Format the directory name with PID
	mkdir(directory, 0700); // Make directory

	char oneLiner[100];	 // This will be used several times for fputs
	int i, j;
	for(i=0; i<7; i++){
		sprintf(file, "%s/%s", directory, cave[i]->name); // Format the file name with room name 
		pFile = fopen(file, "w"); // Write a file for the room and assign it to pFile
		
		sprintf(oneLiner, "ROOM NAME: %s\n", cave[i]->name); // Format oneLiner with room name
		fputs(oneLiner, pFile); // Write room name to file
		for(j=0; j<cave[i]->numDoors; j++){
			sprintf(oneLiner, "CONNECTION %d: %s\n", j+1, cave[i]->door[j]->name);// Format oneLiner for writing room connections 
			fputs(oneLiner, pFile); // Write a connection to the file
			}
		if(cave[i]->type == 0){
			sprintf(oneLiner, "ROOM TYPE: MID_ROOM\n"); // Format oneLiner for type in file
			}
		else if(cave[i]->type== -1){
			sprintf(oneLiner, "ROOM TYPE: START_ROOM\n"); // Format oneLiner for type in file
			}
		else{
			sprintf(oneLiner, "ROOM TYPE: END_ROOM\n"); // Format oneLiner for type in file
			}	
		fputs(oneLiner, pFile); // Write file type
		fclose(pFile); // Close the file
		}
	//unlink(file); // Deletes myfile.txt
	//rmdir(directory); // Removes an empty directory
	}




















