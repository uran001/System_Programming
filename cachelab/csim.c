#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#include "cachelab.h"


typedef unsigned long int ull;
typedef unsigned int ui;

enum {
    mod,
    ld,
    sv
};

enum {
    hits,
    miss,
    evicts
};

typedef struct {
    int avai;
    int set_num;
    int entry;
} cache_sz;

typedef struct {
    ull valid;
    ull tag;
} cache_line;

typedef struct {
    char op;
    ull addr;
    ull i;
    ull tag;
    ui mem_size;
} oper;


typedef struct {
    int hitss;
    int misses;
    int evictss;
} output;

void upd_cache(cache_line *cache, oper an_op, cache_sz sz, output *ans) {

    int i;
    int type;
    int cur;

    ull temp_rag;

    // get type
        if (an_op.op == 'M')
            type = mod;
            
        else if (an_op.op == 'L')
            type = ld;
            
        else if(an_op.op == 'S')
            type = sv;
        
    
    cache_line *pcache = cache + an_op.i * sz.entry;

    cur = evicts;

    for (i = 0; i < sz.entry; i++) {
        if (!(pcache + i)->valid) {
            cur = miss;
            break;
        }
        if ((pcache + i)->tag == an_op.tag) {
            cur = hits;
            break;
        }
    }

  
        if(cur == hits){
            temp_rag = (pcache + i)->tag;
            for ( ; i < sz.entry - 1 && (pcache + i + 1)->valid; i++) {
                (pcache + i)->tag = (pcache + i + 1)->tag;
            }
            (pcache + i)->tag = temp_rag;
            ans->hitss++;
            }

        else if (cur == miss){
            ans->misses++;
            (pcache + i)->valid = 1;
            (pcache + i)->tag = an_op.tag;
            }

        else if (cur == evicts){
            ans->misses++;
            ans->evictss++;
            for (i = 0; i < sz.entry - 1; i++) 
                (pcache + i)->tag = (pcache + i + 1)->tag;
            
            (pcache + i)->tag = an_op.tag;

            }
    

    if (type == mod)
        ans->hitss++;
        
    if (sz.avai) {
        
        if(type == mod)
            printf("M ");
        
        else if(type == ld)
            printf("L ");
        
        else
            printf("S ");
                    
        }

        printf("%lx,%d ", an_op.addr, an_op.mem_size);
        
        if(cur == hits)
                printf("hits");
        else if (cur == miss)
                printf("miss");
        else if (cur == evicts)
                printf("miss evicts");

        if (type == mod)
            printf(" hits");
        puts("");
    }



int main(int argc, char **argv) {

    char opt;
    int h = 0, avai = 0, set_pw = 0, set_num = 0, entry = 0, block_pw = 0, trace = 0;

    char *file;

    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                h = 1;
                break;
            case 'v':
                avai = 1;
                break;
            case 's':
                set_pw = atoi(optarg);
                set_num = 1 << set_pw;
                break;
            case 'E':
                entry = atoi(optarg);
                break;
            case 'b':
                block_pw = atoi(optarg);
                break;
            case 't':
                trace = 1;
                file = optarg;
                break;
            default:
                h = 1;
                break;
        }
    }

    if (h) {
        exit(0);
    }
    if (!set_pw || !entry  || !block_pw || !trace) {
        printf("Arguments Error!\n\n");
        exit(1);
    }

    // Build cache
    output ans;
    cache_sz sz;
    
    ans.hitss = ans.misses = ans.evictss = 0;
    sz.avai = avai, sz.set_num = set_num, sz.entry = entry;
    cache_line *cache = (cache_line *)malloc(sizeof(cache_line) * set_num * entry);
    
    if (cache == NULL) {
        printf("Memory Error!\n");
        exit(1);
    }

    for (int i = 0; i < set_num * entry; i++) {
        (cache + i)->valid = 0;
        (cache + i)->tag = 0xffffffff;
    }

    // Read file
    FILE *trace_file = fopen(file, "r");
    oper an_op;

    char line[80];
    char *pline = NULL;
    
    while (fgets(line, 80, trace_file) != NULL) {
    
        pline = line;
        if (*pline++ == 'I')
            continue;
        sscanf(pline, "%c %lx,%u", &an_op.op, &an_op.addr, &an_op.mem_size);
    
        an_op.i = (an_op.addr >> block_pw) & ~(~0 << set_pw);
        an_op.tag = an_op.addr >> (block_pw + set_pw);
    
        upd_cache(cache, an_op, sz, &ans);
    }
    free(cache);

    printSummary(ans.hitss, ans.misses, ans.evictss);
    return 0;
}