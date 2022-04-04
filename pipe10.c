#include <stdio.h>
#include <unistd.h>
#include <string.h>
//#include <stdlib.h>

#define BUFF_SIZE 50

void main()
{
   char message[BUFF_SIZE];

//   int fd[2];

//   pipe( fd );

   int pipearray[4][2];		//delcare array for 4 pipes
				//each pipe has 2 elements

				//populate array with pipes
   pipe( pipearray[0] );	//parent-write-child1-read
   pipe( pipearray[1] );	//parent-read--child1-write
   pipe( pipearray[2] );	//parent-write-child2-read
   pipe( pipearray[3] );	//parent-read--child2-write#

   //============================================================
   //=========================  child1  =========================
   //============================================================
   if ( fork() == 0 )		// in child1, index0,1
   {
				//child reads from pipe0
      // pipearray[0][0] );	//parent-write-child1-read
      close( pipearray[0][1] ); //parent-read--child1-write 

				//child writes to pipe1
      close( pipearray[1][0] ); //parent-write-child1-read
      // pipearray[1][1] );	//parent-read--child1-write
 
      sleep(2);

      printf("Child1 trying to read from pipe\n");
      read(pipearray[0][0], message, BUFF_SIZE);
      printf("Child1 reads from parent: %s", message);
      printf("Child1 writes to pipe for parent\n");
      sprintf(&message, "Child1 replies: %s %s", "Hello", "World!");
      printf("%s\n", message);
      write( pipearray[1][1], &message, BUFF_SIZE); 
   }
   //============================================================
   //=========================  child2  =========================
   //============================================================
   else if ( fork() == 0 )	// in child2, index2,3
   {
				//child reads from pipe2
      // pipearray[2][0] );	//parent-write-child1-read
      close( pipearray[2][1] ); //parent-read--child1-write 
				//child writes to pipe3
      close( pipearray[3][0] ); //parent-write-child1-read
      // (pipearray[3][1] );	//parent-read--child1-write

      sleep(4);

      printf("Child2 trying to read from pipe\n");
      read(pipearray[2][0], message, BUFF_SIZE);
      printf("Child2 reads from parent: %s", message);     

      printf("Child2 writes to pipe for parent\n");
      write( pipearray[3][1], "Child2 replies: Hello", BUFF_SIZE); 
   }
   //============================================================
   //=========================  parent  =========================
   //============================================================
   else
   {
				
      close( pipearray[0][0] );	//parent reads child1 from pipe0
      //( pipearray[0][1] );
      //( pipearray[1][0] );
      close( pipearray[1][1] );	//parent writes child1 from pipe1

      close( pipearray[2][0] );	//parent reads child2 from pipe2
      //( pipearray[2][1] );
      //( pipearray[3][0] );
      close( pipearray[3][1] );	//parent writes child2 from pipe3

      sleep(10);
      printf("Parent writing to children\n");
      sprintf(&message, "Hello child%d", 1);
      write( pipearray[0][1], "Hello child1\n", BUFF_SIZE);
      sprintf(&message, "Hello child%d", 2);
      write( pipearray[2][1], "Hello child2\n", BUFF_SIZE); 

      sleep(1);
      read(pipearray[1][0], message, BUFF_SIZE);
      printf("Parent reads from child1: %s\n", message);
      sleep(5);
      read(pipearray[3][0], message, BUFF_SIZE);
      printf("Parent reads from child2: %s\n", message);

      sleep(10);
   }

}
