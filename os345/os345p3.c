// os345p3.c - Jurassic Park
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the CS345 projects.          **
// ** It comes "as is" and "unwarranted."  As such, when you use part   **
// ** or all of the code, it becomes "yours" and you are responsible to **
// ** understand any algorithm or method presented.  Likewise, any      **
// ** errors or problems become your responsibility to fix.             **
// **                                                                   **
// ** NOTES:                                                            **
// ** -Comments beginning with "// ??" may require some implementation. **
// ** -Tab stops are set at every 3 spaces.                             **
// ** -The function API's in "OS345.h" should not be altered.           **
// **                                                                   **
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// ***********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <time.h>
#include <assert.h>
#include "os345.h"
#include "os345park.h"

// ***********************************************************************
// project 3 variables

// Jurassic Park
extern JPARK myPark;
extern Semaphore* parkMutex;						// protect park access
extern Semaphore* fillSeat[NUM_CARS];			// (signal) seat ready to fill
extern Semaphore* seatFilled[NUM_CARS];		// (wait) passenger seated
extern Semaphore* rideOver[NUM_CARS];			// (signal) ride over
extern TCB tcb[];

int globalDriverId;

Semaphore* tickets;
// ***********************************************************************
// project 3 functions and tasks
void CL3_project3(int, char**);
void CL3_dc(int, char**);

int carTask(int argc, char* argv[]);
int visitorTask(int argc, char* argv[]);
int driverTask(int argc, char* argv[]);
void waitTime(int maximum, Semaphore* sem);
void passDriverWaitSem(Semaphore* sem, int driverId);

// ***********************************************************************
// semaphores
Semaphore* getPassenger;
Semaphore* seatTaken;
Semaphore* needDriver;
Semaphore* wakeupDriver;
Semaphore* roomInPark;
Semaphore* driverReady;
Semaphore* needTicket;
Semaphore* ticketReady;
Semaphore* buyTicket;
Semaphore* needPassengerWait;
Semaphore* passengerResourceReady;
Semaphore* driverResourceReady;
Semaphore* globalSemaphore;
Semaphore* needDriverSem;
Semaphore* passengerResourceAcquired;
Semaphore* driverResourceAcquired;
Semaphore* resourceMutex;
Semaphore* getTicketMutex;
Semaphore* roomInMuseum;
Semaphore* roomInGiftShop;
Semaphore* getDriverMutex;

// ***********************************************************************
// ***********************************************************************
// project3 command
int P3_main(int argc, char* argv[])
{
	char buf[32];
	char idBuf[32];
	char* newArgv[2];
    int i;

	// start park
	sprintf(buf, "jurassicPark");
	newArgv[0] = buf;
	createTask( buf,				// task name
		jurassicTask,				// task
		MED_PRIORITY,				// task priority
		1,								// task count
		newArgv);					// task argument

    tickets = createSemaphore("tickets", COUNTING, MAX_TICKETS);
    roomInPark = createSemaphore("roomInPark",COUNTING, MAX_IN_PARK);
    roomInMuseum = createSemaphore("roomInMuseum",COUNTING, MAX_IN_MUSEUM);
    roomInGiftShop = createSemaphore("roomInGiftShop",COUNTING, MAX_IN_GIFTSHOP);
    wakeupDriver = createSemaphore("wakeupDriver",COUNTING,0);

    resourceMutex = createSemaphore("resourceMutex",BINARY,1);
    getTicketMutex = createSemaphore("getTicketMutex",BINARY,1);
    getDriverMutex = createSemaphore("getDriverMutex",BINARY,1);

    getPassenger = createSemaphore("getPassenger",BINARY,0);
    seatTaken = createSemaphore("seatTaken",BINARY,0);
    needDriver = createSemaphore("needDriver",BINARY,0);
    driverReady = createSemaphore("driverReady",BINARY,0);
    needTicket = createSemaphore("needTicket",BINARY,0);
    ticketReady = createSemaphore("ticketReady",BINARY,0);
    buyTicket = createSemaphore("buyTicket",BINARY,0);

    needPassengerWait = createSemaphore("needPassengerWait",BINARY,0);
    passengerResourceReady = createSemaphore("passengerResourceReady",BINARY,0);
    driverResourceReady = createSemaphore("driverResourceReady",BINARY,0);
    needDriverSem = createSemaphore("needDriverSem",BINARY,0);
    passengerResourceAcquired = createSemaphore("passengerResourceAcquired",BINARY,0);
    driverResourceAcquired = createSemaphore("driverResourceAcquired",BINARY,0);


    // wait for park to get initialized...
	while (!parkMutex) 
        SWAP;
	SWAP printf("\nStart Jurassic Park...");

    //?? create car, driver, and visitor tasks here
    for (i = 0; i < NUM_CARS; i++) {
        SWAP sprintf(buf, "car%d", i);
        SWAP sprintf(idBuf, "%d", i);
        SWAP newArgv[0] = buf;
        SWAP newArgv[1] = idBuf;
        SWAP createTask(buf,
                   carTask,
                   MED_PRIORITY,
                   2,
                   newArgv);	
    }


    for (i = 0; i < NUM_DRIVERS; i++) {
        SWAP sprintf(buf, "driver%d", i);
        SWAP sprintf(idBuf, "%d", i);
        SWAP newArgv[0] = buf;
        SWAP newArgv[1] = idBuf;
        SWAP createTask(buf,
                   driverTask,
                   MED_PRIORITY,
                   2,
                   newArgv);
    }


    for (i = 0; i < NUM_VISITORS; i++) {
        SWAP sprintf(buf, "visitor%d", i);
        SWAP sprintf(idBuf, "%d", i);
        SWAP newArgv[0] = buf;
        SWAP newArgv[1] = idBuf;
        SWAP createTask(buf,
                   visitorTask,
                   MED_PRIORITY,
                   2,
                   newArgv);
    }

    return 0;
}

int carTask(int argc, char* argv[])
{

    SWAP char buf[32];
    SWAP int carId = atoi(argv[1]);
    sprintf(buf,"%sTourFinished", argv[0]);
    Semaphore* passengerWait[NUM_SEATS];
    Semaphore* driverDone;
    Semaphore* tempSem;
    SWAP printf("Starting carTask%d", carId);

    while (1) {
        // wait till all seats are filled
        for (int i = 0; i < NUM_SEATS; i++) {
            
            // wait for a passenger to be ready
            SWAP semWait(fillSeat[carId]);               

            // fill seat
            SWAP semSignal(getPassenger);      

            // signal that seat is taken
            SWAP semWait(seatTaken); 

            // wait for passenger to be ready
            SWAP semSignal(needPassengerWait);        
            SWAP semSignal(passengerResourceReady);  
            SWAP tempSem = globalSemaphore;
            SWAP passengerWait[i] = tempSem;
            SWAP semSignal(passengerResourceAcquired);
           
            
            // fill seat in car
            SWAP semSignal(seatFilled[carId]);             
        }

        SWAP semWait(getDriverMutex);
        {
            // signal that driver is needed
            SWAP semSignal(needDriver);
            SWAP semSignal(wakeupDriver);

            // get mailboxed semaphore
            SWAP semSignal(needDriverSem);
            SWAP semWait(driverResourceReady);
            SWAP tempSem = globalSemaphore;

            // fill driver in car
            SWAP semWait(parkMutex);
            {
                SWAP myPark.drivers[globalDriverId] = (carId + 1);
            }
            SWAP semSignal(parkMutex);
            // signal driver in car
            SWAP semSignal(driverResourceAcquired);

            //set driverDone to the mailboxed semaphore
            SWAP driverDone = tempSem;
        }
        SWAP semSignal(getDriverMutex);

        // wait for ride to be over
        SWAP semWait(rideOver[carId]);                   

        // signal driver that ride is over
        SWAP semSignal(driverDone);     

        // signal to each passenger that ride is over
        for (int i = 0; i < NUM_SEATS; i++)
        {
            SWAP semSignal(passengerWait[i]);           
        }
    }

    return 0;
}

// ***********************************************************************
// ***********************************************************************
int visitorTask(int argc, char* argv[])
{
    // set up variables
    char buf[32];
    int visitorId = atoi(argv[1]);
    sprintf(buf,"%sWait",argv[0]);
    Semaphore* visitorWait = createSemaphore(buf,BINARY,0);

    // get in line outside of park
    SWAP semWait(parkMutex);
    {
        SWAP myPark.numOutsidePark++;
    }
    SWAP semSignal(parkMutex);

    // wait a random amount of time outside of park
    SWAP waitTime(100, visitorWait);

    // wait till there is room in park to enter
    SWAP semWait(roomInPark);

    // move to ticket line
    SWAP semWait(parkMutex);        
    {
        // modifying variables
        SWAP myPark.numOutsidePark--;
        SWAP myPark.numInPark++;     
        SWAP myPark.numInTicketLine++;
    }
    SWAP semSignal(parkMutex);

    // wait random amount of time to ask for a ticket
    waitTime(30, visitorWait);
    // wait for you to be at the front of the line
    SWAP semWait(getTicketMutex);         
    {
        // signal need ticket
        SWAP semSignal(needTicket);		  

        // wakeup driver
        SWAP semSignal(wakeupDriver);

        // wait ticket available
        SWAP semWait(ticketReady);		  

        // buy ticket
        SWAP semSignal(buyTicket);		  

        // reduce number of available tickets
        SWAP semWait(parkMutex);             
        {
            SWAP myPark.numTicketsAvailable--;
        }
        SWAP semSignal(parkMutex);   
    }
    // allow next visitor to purchase ticket
    SWAP semSignal(getTicketMutex);

    // get in line to enter museum
    SWAP semWait(parkMutex);        
    {
        SWAP myPark.numInTicketLine--;
        SWAP myPark.numInMuseumLine++;     
    }
    SWAP semSignal(parkMutex);  

    // wait a random amount of time to enter museum
    SWAP waitTime(30, visitorWait);

    // wait for there to be room in museum
    SWAP semWait(roomInMuseum);
    {
        //SWAP moveToMuseum();
        // enter musuem
        SWAP semWait(parkMutex);        
        {
            SWAP myPark.numInMuseumLine--;
            SWAP myPark.numInMuseum++;     
        }
        SWAP semSignal(parkMutex); 

        // wait a random amount of time in museum
        SWAP waitTime(30, visitorWait);
    }
    // allow other visitor to enter museum
    SWAP semSignal(roomInMuseum);

    // enter car line
    SWAP semWait(parkMutex);        
    {
        SWAP myPark.numInMuseum--;
        SWAP myPark.numInCarLine++;     
    }
    SWAP semSignal(parkMutex); 

    //SWAP waitForTourExperience(visitorWait);

    // wait random amount of time to get on car
    SWAP waitTime(30, visitorWait);

    // wait for car to be available
    SWAP semWait(getPassenger);

    // get in car
    SWAP semWait(parkMutex);             
    {
        SWAP myPark.numInCarLine--;       
        SWAP myPark.numInCars++;          
    }
    SWAP semSignal(parkMutex);

    // get global passenger semaphore
    SWAP semWait(resourceMutex);   
    SWAP semSignal(seatTaken);     
    SWAP semWait(needPassengerWait);

    SWAP globalSemaphore = visitorWait;          

    SWAP semSignal(passengerResourceReady); 
    SWAP semWait(passengerResourceAcquired);

    SWAP semSignal(resourceMutex);

    SWAP semWait(visitorWait);

    // enter gift shop line
    SWAP semWait(parkMutex);       
    {
        SWAP myPark.numInCars--;    
        SWAP myPark.numInGiftLine++;

        // return ticket
        SWAP myPark.numTicketsAvailable++;

        SWAP semSignal(tickets);
    }
    SWAP SEM_SIGNAL(parkMutex); 

    //SWAP waitForGiftShopExperience(visitorWait);
    // wait random amount of time to enter gift shop
    SWAP waitTime(30, visitorWait);
    
    // wait for room in gift shop
    SWAP semWait(roomInGiftShop);

    // enter gift shop
    SWAP semWait(parkMutex);
    {
        SWAP myPark.numInGiftLine--;
        SWAP myPark.numInGiftShop++;
    }
    SWAP semSignal(parkMutex);

    // look around gift shop for a random amount of time
    SWAP waitTime(30, visitorWait);

    // leave gift shop
    SWAP semSignal(roomInGiftShop);

    // exit park
    SWAP semWait(parkMutex);       
    {
        // access inside park variables
        SWAP myPark.numInPark--;    
        SWAP myPark.numInGiftShop--;
        SWAP myPark.numExitedPark++;
        SWAP semSignal(roomInGiftShop);
    }
    // release protect shared memory access
    SWAP semSignal(parkMutex); 

    // signal that there is more room in park
    SWAP semSignal(roomInPark);

    return 0;
}

int driverTask(int argc, char* argv[])
{
    char buf[32];
    Semaphore* driverDone;
    SWAP int myID = atoi(argv[1]);
    SWAP printf(buf, "Starting driverTask%d", myID);		
    SWAP sprintf(buf, "driverDone%d", myID); 		    
    SWAP driverDone = createSemaphore(buf, BINARY, 0);

    while(1)
    {
        semWait(wakeupDriver);
        SWAP;
        if (semTryLock(needDriver))
        {
            SWAP passDriverWaitSem(driverDone, myID);
            SWAP semSignal(driverReady);
            SWAP semWait(driverDone);

            SWAP semWait(parkMutex);
            {
                SWAP myPark.drivers[myID] = 0;
            }
            SWAP semSignal(parkMutex);
        }
        else if (semTryLock(needTicket))
        {

            SWAP semWait(parkMutex);
            {
                SWAP myPark.drivers[myID] = -1;
            }
            SWAP semSignal(parkMutex);

            SWAP semWait(tickets);
            SWAP semSignal(ticketReady);

            SWAP semWait(buyTicket);

            SWAP semWait(parkMutex);
            {
                SWAP myPark.drivers[myID] = 0;
            }
            SWAP semSignal(parkMutex);
        }
    }
    return 0;

}

// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char* argv[])
{
    // print delta clock
    SWAP printf("\nDelta Clock\n");
	SWAP printClock();

	return 0;
}


void waitTime(int max, Semaphore* sem)
{
    // Randomize time before signaling a semaphore
    SWAP int time = (rand() % max) + 1;
    SWAP enQueueDC(time, sem);

    SWAP semWait(sem);
}

void passDriverWaitSem(Semaphore *sem, int driverId)
{
    SWAP semWait(resourceMutex);
    SWAP semWait(needDriverSem);
    SWAP globalSemaphore = sem;
    SWAP globalDriverId = driverId;
    SWAP semSignal(driverResourceReady);
    SWAP semWait(driverResourceAcquired);
    SWAP semSignal(resourceMutex);
}
