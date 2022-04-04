/**
 * @file p1.c
 * @author Sean Susmilch
 * @brief this is the main file for CSCI 474 Project 1
 * @version 0.1
 * @date 2022-03-20
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define CHILDREN 8
#define BUFFER_SIZE 50

int pipearray[CHILDREN*2][2];
char message[BUFFER_SIZE];

void announce(char *msg){
/**
 * @brief This is just a helper function
 * that is used to announce which requirement
 * of the project is being satisfied. Makes it
 * look nicer in the code too
 * 
 */
    printf("\n[Satisfying Requirement]: '%s'\n------------------------------\n", msg);
}

void pprint(char *msg){
/**
 * @brief I'm used to python and how I dont
 * have to add a new line to prints so I'll
 * just make my own function for that here
 * 
 */
    printf("%s\n", msg);
}

int parent_func(int child_pids[], int argc, char *argv[]){
    /**
     * @brief This is the code that
     * will only be executed by the 
     * main PARENT process.
     * 
     */
    time_t start, stop;
    start = time(NULL);
    clock_t cpu_start, cpu_stop;
    cpu_start = clock(); 

    for(int i=0; i<CHILDREN; i++){
        close(pipearray[(2*i) + 0][0]);
        close(pipearray[(2*i) + 1][1]);
    }

    sleep(4);
    announce("Have a process that forks multiple children where each child is a direct descendent of the parent process.");
    printf("\nPARENT PID: %d", getpid());
    printf("\nCHILD_IDS: [");
    for(int i=0; i<CHILDREN; i++){
        printf("\t%d", child_pids[i]);
    }
    printf("\t]\n");

    sleep(1);
    announce("Your process must allow the parent to communicate with each of its children using a pipe.");
    pprint("Parent writing to all children");
    for(int i=0; i<CHILDREN; i++){
        sprintf(message, "Hello child %d!", i);
        printf("Writing '%s' to child %d\n", message, i);
        write(pipearray[(2*i) + 0][1], message, BUFFER_SIZE);
        // sleep(.1);
    }

    sleep(5);
    announce("Your process must allow each child to communicate with its parent using a pipe.");
    for(int i=0; i<CHILDREN; i++){
        read(pipearray[(2*i) + 1][0], message, BUFFER_SIZE);
        printf("Parent reads from child %d: %s\n", i, message);
    }


    announce("Determine whether writing to a pipe is a blocking process.");
    pprint("Writing 10 messages to PIPE 0");
    close(pipearray[0][0]);
    close(pipearray[0][1]);
    for(int i=0; i<10; i++){
        printf("%d\t", i);
        write(pipearray[0][1], "message", BUFFER_SIZE);
        sleep(.5);
    }
    pprint("\n"
        "FINDINGS:\n"
        "The writing to a pipe is a NON BLOCKING process.\n"
        "This can be seen by the 10 messages that were pushed\n"
        "to the pipe without reading a single one..."
        );


    announce("Determine whether writing to a pipe is a blocking process.");
    pprint("\n"
        "FINDINGS:\n"
        "Reading from a pipe is a BLOCKING process.\n"
        "This can be seen by the earlier stages of this\n"
        "program. When a child is created, they reach the\n"
        "'read' statement before the parent writes anything\n"
        "and stays there until the parent writes something."
        );


    announce("Read input from the keyboard");
    char output_filename[50];
    printf("Enter desired output filename: ");
    scanf("%s", output_filename);
    // output_filename = "out.txt";
    printf("\nYou chose %s", output_filename);


    announce("Process command line arguments");
    char *input_file;
    pprint("Listing arguments...");
    for(int i=0; i<argc; i++){
        printf("\targ%d: %s\n", i, argv[i]);
    }
    pprint("Using first argument argv[1] as the input file");
    input_file = argv[1];


    announce("Write output to a text file");
    pprint("Generating random values to write 😉😉...");
    char *lines[5];
    char *strs[5] = {"I", "loooooove", "programming", "in", "C!"};
    int ints[5] = {1,2,3,4,5};
    float floats[5] = {1.1,2.2,3.3,4.4,5.5};
    pprint("Generating lines...");
    for(size_t j=0; j<5; j++){
        asprintf(&lines[j], "%d\t%s\t%f\n", ints[j], strs[j], floats[j]);
        printf("%s", lines[j]);
    }
    printf("Writing the above text to %s\n", output_filename);
    FILE *out_file = fopen(output_filename, "w");
    for(size_t k=0; k<5; k++){
        fprintf(out_file, lines[k]);
    }
    fclose(out_file);
    pprint("File closed!\n");

    
    announce("Read in a text file");
    char read_lines[5][50];
    FILE *read_file = fopen(output_filename, "r");
    for(size_t i=0; i<5; i++){
        fgets(read_lines[i], 255, (FILE*)read_file);
    }
    int read_ints[5];
    char read_strs[5][50];
    float read_floats[5];
    for(size_t i=0; i<5; i++){
        printf("Reading Line: %s", read_lines[i]);
        sscanf(read_lines[i], "%d\t%s\t%f\n", &read_ints[i], &read_strs[i], &read_floats[i]);
        printf("Parsed out: int %d\tstr %s\t float %f\n", read_ints[i], read_strs[i], read_floats[i]);
    }
    fclose(read_file);


    announce("Use array(s)");
    pprint("Creating an array of 4 ints");
    int int_arr[4];
    pprint("Adding values to array -> [ 3, 2, 1, 0 ]");
    int_arr[0] = 3;
    int_arr[1] = 2;
    int_arr[2] = 1;
    int_arr[3] = 0;
    pprint("Printing out contents of array");
    printf("[");
    for(size_t i=0; i<4; i++){
        printf(" %d", int_arr[i]);
    }
    printf(" ]\n");


    announce("Show that you can time how long a process runs using a wall-clock/calendar timer. & Show that you can time the CPU usage of a process.");
    time(&stop);
    cpu_stop = clock();
    pprint("Turns out, I've been timing the entire parent function 😎");
    printf("The parent function has taken\n\t %ld seconds (real time) and \n\t%f sec of CPU time so far\n", stop-start, ((double)(cpu_stop-cpu_start))/ CLOCKS_PER_SEC);

    pprint("Parent all done.");
}

int child_func(int given_id){
    /**
     * @brief This is the function that will
     * only be executed by CHILD processes
     * 
     */

    // children read from even pipes, write on odd pipes
    // p_read = pipearray[(2*given_id) + 0];
    // p_write = pipearray[(2*given_id) + 1];
    int p_read, p_write;
    p_read = (2*given_id) + 0;
    p_write = (2*given_id) + 1;

    close(pipearray[p_read][1]);
    close(pipearray[p_write][0]);

    printf("CHILD %d\tREAD_PIPE: %d\t WRITE_PIPE: %d\tCHILD_ID: %d\tPARENT_ID: %d\n", given_id, p_read, p_write, getpid(), getppid());

    printf("CHILD %d ready to read\n", given_id);
    read(pipearray[p_read][0], message, BUFFER_SIZE);
    printf("CHILD %d read '%s'\n", given_id, message);

    sprintf(message, "Hello from child %d!", given_id);
    printf("CHILD %d responding to parent with '%s'\n", given_id, message);
    write(pipearray[p_write][1], message, BUFFER_SIZE);
}

int main(int argc, char *argv[]){
/**
 * @brief this function is the main
 * entry point to the project.
 * 
 */
    // declare a read and write pipe for each child
    for(int i=0; i<CHILDREN*2; i++){
        pipe(pipearray[i]);
    }


    int child_pids[CHILDREN];
    int fork_res = -1;
    int given_id = -1;
    for(int i=0; i<CHILDREN && fork_res != 0; i++){
        fork_res = fork();
        given_id = i;    // assign a particular pipe set to this child
        if(fork_res != 0){
            memcpy(&child_pids[i], &fork_res, sizeof(fork_res));
        }
        sleep(.25);
    }
    if(fork_res != 0){
        parent_func(child_pids, argc, argv);
    }else{
        child_func(given_id);
    }

    return 0;
}