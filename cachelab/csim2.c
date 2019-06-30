#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>
//#include <bits/stdc++.h>

#include "cachelab.h"

typedef struct {
	int s; 
	int b; 
	int E; 
	int S; 
	int B; 

	int hits;
	int misses;
	int evicts;
} cache_param;


typedef struct {
	int recent;
	int valid;
	unsigned long long int tag;
	char *block;
} set_line;

typedef struct {
	set_line *lines;
} cache_set;

typedef struct {
	 cache_set *sets;
} cache;


int verbosity;

long long bit_pow(int exp) 
{
	long long result = 1;
	result = result << exp;
	return result;
}


void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}



cache make_cache(long long num_sets, int num_lines, long long block_size) 
{

	cache newCache;	
	cache_set set;
	set_line line;
	int setIndex;
	int lineIndex;

	newCache.sets = (cache_set *) malloc(sizeof(cache_set) * num_sets);

	for (setIndex = 0; setIndex < num_sets; setIndex ++) 
	{
		
		set.lines =  (set_line *) malloc(sizeof(set_line) * num_lines);
		newCache.sets[setIndex] = set;

		for (lineIndex = 0; lineIndex < num_lines; lineIndex ++) 
		{
			
			line.recent = 0;
			line.valid = 0;
			line.tag = 0; 
			set.lines[lineIndex] = line;	
		}
		
	} 

	return newCache;
	
}

void clear_cache(cache sim_cache, long long num_sets, int num_lines, long long block_size) 
{
	int setIndex;
	

	for (setIndex = 0; setIndex < num_sets; setIndex ++) 
	{
		cache_set set = sim_cache.sets[setIndex];
		
		if (set.lines != NULL) {	
			free(set.lines);
		}
		
	} 
	if (sim_cache.sets != NULL) {
		free(sim_cache.sets);
	}
}

int find_empty_line(cache_set query_set, cache_param par) {
	int num_lines = par.E;
	int index;
	set_line line;

	for (index = 0; index < num_lines; index ++) {
		line = query_set.lines[index];
		if (line.valid == 0) {
			return index;
		}
	}
	return -1;
}

int find_evict_line(cache_set query_set, cache_param par, int *used_lines) {
	
	int num_lines = par.E;
	int max_used = query_set.lines[0].recent;
	int min_used = query_set.lines[0].recent;
	int min_used_index = 0;

	set_line line; 
	int lineIndex;

	for (lineIndex = 1; lineIndex < num_lines; lineIndex ++) {
		line = query_set.lines[lineIndex];

		if (min_used > line.recent) {
			min_used_index = lineIndex;	
			min_used = line.recent;
		}

		if (max_used < line.recent) {
			max_used = line.recent;
		}
	}

	used_lines[0] = min_used;
	used_lines[1] = max_used;
	return min_used_index;
}

cache_param run_sim(cache sim_cache, cache_param par, unsigned long long int address) {
		
		int lineIndex;
		int cache_full = 1;

		int num_lines = par.E;
		int prev_hits = par.hits;

		int tag_size = (64 - (par.s + par.b));
		
		unsigned long long int input_tag = address >> (par.s + par.b);
		unsigned long long temp = address << (tag_size);
		unsigned long long setIndex = temp >> (tag_size + par.b);
		
  		cache_set query_set = sim_cache.sets[setIndex];

		for (lineIndex = 0; lineIndex < num_lines; lineIndex ++) 	{
			
			set_line line = query_set.lines[lineIndex];
			
			if (line.valid) {
					
				if (line.tag == input_tag) {
						
					line.recent ++;
					par.hits ++;
					query_set.lines[lineIndex] = line;
				}

			} else if (!(line.valid) && (cache_full)) {
				cache_full = 0;		
			}

		}	

		if (prev_hits == par.hits) {
			par.misses++;
		} else {
			return par;
		}
		
		int *used_lines = (int*) malloc(sizeof(int) * 2);
		int min_used_index = find_evict_line(query_set, par, used_lines);	

		if (cache_full) 
		{
			par.evicts++;

			//Found least-recently-used line, overwrite it.
			query_set.lines[min_used_index].tag = input_tag;
			query_set.lines[min_used_index].recent = used_lines[1] + 1;
		
		}

		else
	        {
			int empty_index = find_empty_line(query_set, par);

			query_set.lines[empty_index].tag = input_tag;
			query_set.lines[empty_index].valid = 1;
			query_set.lines[empty_index].recent = used_lines[1] + 1;
		}						

		free(used_lines);
		return par;
}

int main(int argc, char **argv)
{
	
	cache sim_cache;
	cache_param par;
	bzero(&par, sizeof(par));

	long long num_sets;
	long long block_size;	

 
	FILE *get_trace;
	char trace_cmd;
	unsigned long long int address;
	int size;
	
	char *trace_file;
	char opt;
    while( (opt=getopt(argc,argv,"s:E:b:t:vh")) != -1)
	{
        switch(opt)
		{
        case 's':
            par.s = atoi(optarg);
            break;
        case 'E':
            par.E = atoi(optarg);
            break;
        case 'b':
            par.b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    if (par.s == 0 || par.E == 0 || par.b == 0 || trace_file == NULL) 
	{
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

	
	// you need to compute S and B yourself
	num_sets = (1<<par.s);
	block_size = (1<<par.b);	
	par.hits = 0;
	par.misses = 0;
	par.evicts = 0;
	
	sim_cache = make_cache(num_sets, par.E, block_size);
 	
	// fill in rest of the simulator routine
	get_trace  = fopen(trace_file, "r");
	
	
	if (get_trace != NULL) {
		while (fscanf(get_trace, " %c %llx,%d", &trace_cmd, &address, &size) == 3) {

		
			switch(trace_cmd) {
				case 'I':
					break;
				case 'L':
					par = run_sim(sim_cache, par, address);
					break;
				case 'S':
					par = run_sim(sim_cache, par, address);
					break;
				case 'M':
					par = run_sim(sim_cache, par, address);
					par = run_sim(sim_cache, par, address);	
					break;
				default:
					break;
			}
		}
	}
	
	
    printSummary(par.hits, par.misses, par.evicts);
	clear_cache(sim_cache, num_sets, par.E, block_size);
	fclose(get_trace);

    return 0;
}


