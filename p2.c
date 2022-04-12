/**
 * @file p2.c
 * @author Sean Susmilch
 * @brief this is the main file for CSCI 474 Project 2
 * @version 0.1
 * @date 2022-04-07
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void pprint(char *msg){
/**
 * @brief I'm used to python and how I dont
 * have to add a new line to prints so I'll
 * just make my own function for that here
 */
    printf("%s\n", msg);
}

int is_resident(int pg, size_t res_size, int resident_set[]){
/**
 * @brief returns 0 if is NOT
 * in resident set of process.
 * returns 1 if it is.
 */
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
            for(int i=current_idx-1; i>=0; i--){
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

int simulate(int policy, size_t pg_stream_size, int *pg_stream, int frames_allocated, int proc_num_pgs){
    if(policy == 4){
        // defer to dedicated function for CLOCK policy
        return simulate_clock(pg_stream_size, pg_stream, frames_allocated, proc_num_pgs);
    }

    int faults = 0;
    int last_evicted = -1; // for FIFO
    int space_left = frames_allocated;
    int *resident_set = malloc(frames_allocated * sizeof(int));
    for(int i=0; i<pg_stream_size; i++){
        if(is_resident(pg_stream[i], frames_allocated, resident_set)){
            continue;
        }

        // printf("%d %d\n", i, space_left);

        if(space_left > 0){
            resident_set[i] = pg_stream[i];
            space_left = space_left - 1;
            continue;
        }

        // printf("Page fault occurred with %d Last evicted %d\n", pg_stream[i], last_evicted);
        int evict_idx = get_evict_idx(policy, resident_set, i, pg_stream_size, pg_stream, last_evicted, frames_allocated, proc_num_pgs);
        last_evicted = evict_idx;
        resident_set[evict_idx] = pg_stream[i];
        faults = faults + 1;
    }

    free(resident_set);
    return faults;
}

int simulate_clock(size_t pg_stream_size, int *pg_stream, int frames_allocated, int proc_num_pgs){
    int faults = 0;
    int pointer = 0;
    int space_left = frames_allocated;
    int resident_set[frames_allocated];
    short recently_used[frames_allocated];

    for(int i=0; i<pg_stream_size; i++){
        if(is_resident(pg_stream[i], frames_allocated, resident_set)){
            recently_used[i] = 1;
            continue;
        }

        if(space_left > 0){
            resident_set[i] = pg_stream[i];
            space_left = space_left - 1;
            pointer = (pointer + 1) % frames_allocated;
            continue;
        }

        // printf("Page fault occurred with %d\n", pg_stream[i]);
        while(recently_used[pointer]==1){
            recently_used[pointer] = 0;
            pointer = (pointer + 1) % frames_allocated;
        }
        // when the pointer finds an entry to replace
        resident_set[pointer] = pg_stream[i];
        pointer = (pointer + 1) % frames_allocated;
        faults = faults + 1;
    }

    return faults;
}

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
    printf("%d\t%d\t%d\n", min_frames, max_frames, proc_num_pgs);


    printf("Reading page stream from %s\n", input_filename);
    char *line = NULL;
    size_t len = 0;
    ssize_t line_len;
    int pg_ct = 0;
    int *pg_stream;
    FILE *in_file = fopen(input_filename, "r");
    printf("Page stream: [ ");
    while((line_len = getline(&line, &len, in_file)) != -1){
        sscanf(line, "%d\n", &pg_stream[pg_ct]);
        printf("%d ", pg_stream[pg_ct]);
        pg_ct = pg_ct+1;
    }
    printf("]\n");
    pprint("--------");

    printf("OPT\t%d\t%d\n", 3, simulate(1, pg_ct, pg_stream, 3, proc_num_pgs));  // OPT
    printf("LRU\t%d\t%d\n", 3, simulate(2, pg_ct, pg_stream, 3, proc_num_pgs));  // LRU
    printf("FIFO\t%d\t%d\n", 3, simulate(3, pg_ct, pg_stream, 3, proc_num_pgs));  // FIFO
    printf("CLOCK\t%d\t%d\n", 3, simulate_clock(pg_ct, pg_stream, 3, proc_num_pgs));  // CLOCK

    return 0;
}

