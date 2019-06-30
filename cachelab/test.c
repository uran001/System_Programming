#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>
#include <string>

//#define getName(var, str)  sprintf(str, "%s", #var)

#define getName(var) #var
enum {
	uran,
	naru
};

int main(int argc, char** argv){


	int type = uran;

	printf("%s\n", getName(uran));

	/*
	switch (type) {
            case mod:
                printf("M ");
                break;
            case ld:
                printf("L ");
                break;
            case sv:
                printf("S ");
                break;
        }*/
}