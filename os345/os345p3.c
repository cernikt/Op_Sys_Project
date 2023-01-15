// os345p3.c - Jurassic Park 07/27/2020
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
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "os345.h"
#include "os345park.h"

// ***********************************************************************
// project 3 variables

// Jurassic Park
extern JPARK myPark;
extern Semaphore *parkMutex;            // protect park access
extern Semaphore *fillSeat[NUM_CARS];   // (signal) seat ready to fill
extern Semaphore *seatFilled[NUM_CARS]; // (wait) passenger seated
extern Semaphore *rideOver[NUM_CARS];   // (signal) ride over

Semaphore *roomInPark;
Semaphore *tickets;

// ***********************************************************************
// project 3 functions and tasks
void CL3_project3(int, char **);
void CL3_dc(int, char **);

// ***********************************************************************
// ***********************************************************************
// project3 command
int P3_main(int argc, char *argv[])
{
    char buf[32];
    char *newArgv[2];
    parkMutex = NULL;

    roomInPark = createSemaphore("roomInPark", 1, MAX_IN_PARK);
    tickets = createSemaphore("tickets", 1, MAX_TICKETS);

    // start park
    sprintf(buf, "jurassicPark");
    newArgv[0] = buf;
    createTask(buf,          // task name
               jurassicTask, // task
               MED_PRIORITY, // task priority
               1,            // task count
               newArgv);     // task argument

    // wait for park to get initialized...
    while (!parkMutex)
        SWAP;
    printf("\nStart Jurassic Park...");

    //?? create car, driver, and visitor tasks here
    for (int i = 0; i< NUM_VISITORS; i++) {
        sprintf(buf, "visitor %d", i);
        newArgv[0] = buf;
        createTask(buf,          // task name
                   visitorTask, // task
                   MED_PRIORITY, // task priority
                   1,            // task count
                   newArgv);     // task argument
    }

    return 0;
} // end project3


int workerTask(int argc, char *argv[]) {
    do 
    {
        // SEM_WAIT(workerNeeded);
        // if(needTicket->state) 
        // {
        //     // only 1 worker can sell tickets at a time
        //     // update park vars to show that I am in the ticket booth
        //     // wait for random amount of time to print ticket
        //     SEM_SIGNAL(ticketReady);
        //     SEM_WAIT(ticketBought);
        //     // update park to show that I am sleeping

        // }
        // else if (//need driver
        // ) {

        // }
    }
    while(myPark.numExitedPark < MAX_IN_PARK);
    return 0;
}


int visitorTask(int argc, char *argv[]) {
    SWAP
    // Arrive at park
    SEM_WAIT(parkMutex);
    {
        SWAP myPark.numOutsidePark++;
    }
    SWAP SEM_SIGNAL(parkMutex);

    // visitors wait for random time and try to enter park
    SWAP SEM_WAIT(roomInPark);
    {
        SWAP SEM_WAIT(parkMutex);
        {
            SWAP myPark.numOutsidePark--;
            SWAP myPark.numInPark++;
            SWAP myPark.numInTicketLine++;
        }
        SWAP SEM_SIGNAL(parkMutex);

        //wait, then buy a ticket

        SWAP SEM_WAIT(tickets);
        {
            // signal needTicket
            // signal needDriver
            // wait for driver to sell ticket
            // signal ticketBought
            // SWAP SEM_SIGNAL(needTicket);
            // SWAP SEM_SIGNAL(needWorker);
            // SWAP SEM_WAIT(ticketReady);
            // SWAP SEM_SIGNAL(ticketBought);

            SWAP SEM_SIGNAL(parkMutex);
            {
                SWAP myPark.numInTicketLine--;
                SWAP myPark.numInMuseumLine++;
                SWAP myPark.numInMuseumLine++;
            }
            SWAP SEM_SIGNAL(parkMutex);
            // get out of ticket line and get into museum line
            // wait for random amount of time
            // try to get in museum
            // wait for time in museum
            // exit museum and get into car line
            // wait for random amount of time in car line
            // try to get in car
            // get in car
        }
        SWAP SEM_SIGNAL(tickets);

        SWAP SEM_SIGNAL(tickets);
        {
            
            // wit for car ride to finish
            // get in gift shop line
            // wait for random amount of time in gift shop line
            // try to get in gift shop
            // wait for random amount of time in gift shop
            // exit gift shop
            // get in exit line
            // wait for random amount of time in exit line
        }

        // Leave park

        SWAP SEM_WAIT(parkMutex);
        {
            SWAP myPark.numInPark--;
            SWAP myPark.numExitedPark++;
        }
        SWAP SEM_SIGNAL(parkMutex);
    }
    SWAP SEM_SIGNAL(roomInPark);

    


    return 0;
}

// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char *argv[])
{
    printf("\nDelta Clock");
    // ?? Implement a routine to display the current delta clock contents
    printf("\nTo Be Implemented!");
    return 0;
} // end CL3_dc

/*
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char* argv[])
{
    printf("\nDelta Clock");
    // ?? Implement a routine to display the current delta clock contents
    //printf("\nTo Be Implemented!");
    int i;
    for (i=0; i<numDeltaClock; i++)
    {
        printf("\n%4d%4d  %-20s", i, deltaClock[i].time, deltaClock[i].sem->name);
    }
    return 0;
} // end CL3_dc


// ***********************************************************************
// display all pending events in the delta clock list
void printDeltaClock(void)
{
    int i;
    for (i=0; i<numDeltaClock; i++)
    {
        printf("\n%4d%4d  %-20s", i, deltaClock[i].time, deltaClock[i].sem->name);
    }
    return;
}


// ***********************************************************************
// test delta clock
int P3_tdc(int argc, char* argv[])
{
    createTask( "DC Test",			// task name
        dcMonitorTask,		// task
        10,					// task priority
        argc,					// task arguments
        argv);

    timeTaskID = createTask( "Time",		// task name
        timeTask,	// task
        10,			// task priority
        argc,			// task arguments
        argv);
    return 0;
} // end P3_tdc



// ***********************************************************************
// monitor the delta clock task
int dcMonitorTask(int argc, char* argv[])
{
    int i, flg;
    char buf[32];
    // create some test times for event[0-9]
    int ttime[10] = {
        90, 300, 50, 170, 340, 300, 50, 300, 40, 110	};

    for (i=0; i<10; i++)
    {
        sprintf(buf, "event[%d]", i);
        event[i] = createSemaphore(buf, BINARY, 0);
        insertDeltaClock(ttime[i], event[i]);
    }
    printDeltaClock();

    while (numDeltaClock > 0)
    {
        SEM_WAIT(dcChange)
        flg = 0;
        for (i=0; i<10; i++)
        {
            if (event[i]->state ==1)			{
                    printf("\n  event[%d] signaled", i);
                    event[i]->state = 0;
                    flg = 1;
                }
        }
        if (flg) printDeltaClock();
    }
    printf("\nNo more events in Delta Clock");

    // kill dcMonitorTask
    tcb[timeTaskID].state = S_EXIT;
    return 0;
} // end dcMonitorTask


extern Semaphore* tics1sec;

// ********************************************************************************************
// display time every tics1sec
int timeTask(int argc, char* argv[])
{
    char svtime[64];						// ascii current time
    while (1)
    {
        SEM_WAIT(tics1sec)
        printf("\nTime = %s", myTime(svtime));
    }
    return 0;
} // end timeTask
*/
