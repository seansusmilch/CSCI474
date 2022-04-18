/**
 * @file p2.c
 * @author Sean Susmilch
 * @brief this is the main file for CSCI 474 Project 2
 * @version 0.1
 * @date 2022-04-07
 * 
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int print_arr(int arr[], int arr_size){
    // char *out = "[ ";
    // for(int i=0; i<arr_size; i++){
    //     asprintf(&out, "%s%d ", out, arr[i]);
    // }
    // asprintf(&out, "%s]", out);
    // return out;
    printf("[ ");
    for(int i=0; i<arr_size; i++){
        printf("%d ", arr[i]);
    }
    printf("]");
    return 0;
}

/**
 * @brief I'm used to python and how I dont
 * have to add a new line to prints so I'll
 * just make my own function for that here
 */
void pprint(char *msg){
    printf("%s\n", msg);
}

/**
 * @brief returns 0 if is NOT
 * in resident set of process.
 * returns 1 if it is.
 */
int is_resident(int pg, size_t res_size, int resident_set[]){

    for(int i=0; i<res_size; i++){
        if(pg == resident_set[i]){
            // printf("is resident %d %zu\n", pg, res_size);
            return 1;
        }
    }
    return 0;
}

int get_evict_idx(
        int policy, int resident_set[], int current_idx,
        size_t pg_stream_size, int pg_stream[], int last_evicted,
        int frames_allocated, int proc_num_pgs){
    
    int evict_idx = 0;

    // OPT
    if(policy==1){
        int next_used[frames_allocated];
        for(int j=0; j<frames_allocated; j++){
            next_used[j] = 0;
            for(int ahdstart=current_idx; ahdstart<pg_stream_size; ahdstart++){
                next_used[j] = next_used[j] + 1;
                if(resident_set[j] == pg_stream[ahdstart]){
                    break;
                }
            }
        }
        // evict the idx of the highest value in next_used
        for(int x=0; x<frames_allocated; x++)
            if(next_used[x] > next_used[evict_idx])
                evict_idx = x;
    
    // LRU
    }else if(policy == 2){
        int last_used[frames_allocated];
        for(int j=0; j<frames_allocated; j++){
            last_used[j] = 0;
            for(int i=current_idx; i>=0; i--){
                if(resident_set[j] == pg_stream[i]){
                    last_used[j] = i;
                    break;
                }
            }
        }

        // evict the lowest value in the last_used
        for(int x=0; x<frames_allocated; x++)
            if(last_used[x] < last_used[evict_idx])
                evict_idx = x;
    
    // FIFO
    }else if(policy==3){
        // printf("%d\t", last_evicted);
        evict_idx = (last_evicted + 1) % frames_allocated;
    }
    return evict_idx;
}

/**
 * @brief Takes same params as simulate(). Uses the clock policy
 * 
 * @param pg_stream_size 
 * @param pg_stream 
 * @param frames_allocated 
 * @param proc_num_pgs 
 * @return int Number of page faults
 */
int simulate_clock(int pg_stream_size, int *pg_stream, int frames_allocated, int proc_num_pgs){
    int faults = 0;
    int pointer = 0;
    int space_left = frames_allocated;
    // int *resident_set = malloc(frames_allocated * sizeof(int));
    // int *recently_used = malloc(frames_allocated * sizeof(int));
    int resident_set[frames_allocated];
    int recently_used[frames_allocated];
    for(int i=0; i<frames_allocated; i++){
        resident_set[i] = 0;
        recently_used[i] = 0;
    }

    for(int i=0; i<pg_stream_size; i++){
        if(pg_stream[i] > proc_num_pgs)
            continue;
        if(is_resident(pg_stream[i], frames_allocated, resident_set)){
            for(int x=0; x<frames_allocated; x++){
                if(resident_set[x] == pg_stream[i]){
                    recently_used[x] = 1;
                    break;
                }
            }
            
            // printf("%d\tPointer: %d\tUseBits ", pg_stream[i], pointer);
            // print_arr(recently_used, frames_allocated);
            // printf(" Resident ");
            // print_arr(resident_set, frames_allocated);
            // printf("\n");
            continue;
        }

        if(space_left > 0){
            resident_set[pointer] = pg_stream[i];
            recently_used[pointer] = 1;
            space_left = space_left - 1;
            pointer = (pointer + 1) % frames_allocated;
            // printf("%d\tPointer: %d\tUseBits ", pg_stream[i], pointer);
            // print_arr(recently_used, frames_allocated);
            // printf(" Resident ");
            // print_arr(resident_set, frames_allocated);
            // printf("\n");
            continue;
        }

        // IN CASE OF PAGE FAULT
        // printf("Page fault occurred with %d\n", pg_stream[i]);
        while(recently_used[pointer]==1){
            recently_used[pointer] = 0;
            pointer = (pointer + 1) % frames_allocated;
        }
        // when the pointer finds an entry to replace
        resident_set[pointer] = pg_stream[i];
        recently_used[pointer] = 1;
        pointer = (pointer + 1) % frames_allocated;
        faults = faults + 1;
        // printf("%d\tPointer: %d\tUseBits ", pg_stream[i], pointer);
        // print_arr(recently_used, frames_allocated);
        // printf(" Resident ");
        // print_arr(resident_set, frames_allocated);
        // printf("\n");
    }

    // free(resident_set);
    // free(recently_used);
    return faults;
}

/**
 * @brief The main paging simulation function. Returns the number of page faults
 * that ocurr with the specified policy.
 * 
 * @param policy
 * 1: OPT
 * 2: LRU
 * 3: FIFO
 * 4: CLOCK (defers to separate function)
 * 
 */
int simulate(int policy, int pg_stream_size, int *pg_stream, int frames_allocated, int proc_num_pgs){
    if(policy == 4){
        // defer to dedicated function for CLOCK policy
        return simulate_clock(pg_stream_size, pg_stream, frames_allocated, proc_num_pgs);
    }

    int faults = 0;
    int last_evicted = -1; // for FIFO
    int space_left = frames_allocated;
    // int *resident_set = malloc(frames_allocated * sizeof(int));
    int resident_set[frames_allocated];
    for(int i=0; i<frames_allocated; i++){
        resident_set[i] = 0;
    }
    for(int i=0; i<pg_stream_size; i++){
        if(pg_stream[i] > proc_num_pgs){
            // pprint("Page request is invalid");
            continue;
        }
        if(is_resident(pg_stream[i], frames_allocated, resident_set)){
            continue;
        }

        // printf("%d %d\n", i, space_left);

        if(space_left > 0){
            resident_set[frames_allocated-space_left] = pg_stream[i];
            space_left = space_left - 1;
            continue;
        }

        // printf("Page fault occurred with %d Last evicted %d", pg_stream[i], last_evicted);
        // print_arr(&resident_set, frames_allocated);
        // pprint("");
        int evict_idx = get_evict_idx(policy, resident_set, i, pg_stream_size, pg_stream, last_evicted, frames_allocated, proc_num_pgs);
        last_evicted = evict_idx;
        resident_set[evict_idx] = pg_stream[i];
        faults = faults + 1;
    }

    // free(resident_set);
    return faults;
}

char *get_policy_name(int policy){
    char *policy_name = "Unknown";
    switch (policy){
        case 1:
            policy_name = "OPT";
            break;
        case 2:
            policy_name = "LRU";
            break;
        case 3:
            policy_name = "FIFO";
            break;
        case 4:
            policy_name = "CLOCK";
            break;
        default:
            policy_name = "Unknown";
            break;
        }
    return policy_name;
}

/**
 * @brief The main function for CSCI474 project 2.
 * 
 * @param argc arguments from command line
 * arg1: Page stream input file
 * arg2: Minimum frames allocated
 * arg3: Maximum frames allocated
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[]){
    pprint("Running with these args...");
    for(int i=0; i<argc; i++){
        printf("\targ%d: %s\n", i, argv[i]);
    }
    const char *input_filename = argv[1];
    int min_frames, max_frames, proc_num_pgs;
    sscanf(argv[2], "%d", &min_frames);
    sscanf(argv[3], "%d", &max_frames);
    sscanf(argv[4], "%d", &proc_num_pgs);

    printf("Reading page stream from %s\n", input_filename);
    char *line = NULL;
    size_t len = 0;
    ssize_t line_len;
    int pg_ct_unfiltered = 0;
    int *pg_stream_unfiltered = malloc(5000 * sizeof(int));
    FILE *in_file = fopen(input_filename, "r");
    // printf("Page stream: [ ");
    while((line_len = getline(&line, &len, in_file)) != -1){
        sscanf(line, "%d\n", &pg_stream_unfiltered[pg_ct_unfiltered]);
        // printf("%d ", pg_stream_unfiltered[pg_ct_unfiltered]);
        pg_ct_unfiltered = pg_ct_unfiltered+1;
    }
    // printf("]\n");
    // pprint("Page stream obtained");
    printf("Unfiltered page stream length: %d\n", pg_ct_unfiltered);
    // realloc(pg_stream_unfiltered, pg_ct_unfiltered * sizeof(int));
    pprint("Filtering page stream to amount of pages in process");
    int pg_ct = 0;
    int *pg_stream = (int*)malloc(pg_ct_unfiltered * sizeof(int));
    for(int i=0; i<pg_ct_unfiltered; i++){
        if(pg_stream_unfiltered[i] > proc_num_pgs)
            continue;
        pg_stream[pg_ct] = pg_stream_unfiltered[i];
        pg_ct = pg_ct + 1;
    }
    printf("Filtered page stream length: %d\n", pg_ct);

    // realloc(pg_stream, pg_ct * sizeof(int));
    // free(pg_stream_unfiltered);
    // printf("Filtered pg stream: ");
    // print_arr(pg_stream, pg_ct);
    printf("Min Frames: %d\nMax Frames: %d\n# of Pages in Process: %d\n", min_frames, max_frames, proc_num_pgs);
    pprint("\n--------");
    
    for(int policy_num = 1; policy_num < 5; policy_num++){
        char *policy_name = get_policy_name(policy_num);
        
        for(int frames_alloc = min_frames; frames_alloc <= max_frames; frames_alloc++){
            printf("%s,\t%d,\t%d\n", policy_name, frames_alloc, simulate(policy_num, pg_ct_unfiltered, pg_stream_unfiltered, frames_alloc, proc_num_pgs));
        }
    }
    // printf("%s\t%d\t%d\n", policy_names[1], 3, simulate(1, pg_ct, pg_stream, 3, proc_num_pgs));  // OPT
    // printf("LRU\t%d\t%d\n", 3, simulate(2, pg_ct, pg_stream, 3, proc_num_pgs));  // LRU
    // printf("FIFO\t%d\t%d\n", 3, simulate(3, pg_ct, pg_stream, 3, proc_num_pgs));  // FIFO
    // printf("CLOCK\t%d\t%d\n", 3, simulate(4, pg_ct, pg_stream, 3, proc_num_pgs));  // CLOCK
    
    return 0;
}

