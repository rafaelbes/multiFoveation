#include "fovea.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	MultiFovea mf;

	int randLimit = 90;

	if(argc == 1) {
		printf("please provide one method at command-line\n");
		printf("0: reexecutation\n");
		printf("1: bitmask\n");
		printf("2: px/px method\n");
		printf("3: sendblock method\n");
		return 1;
	}
	srand(time(NULL));
	for(int i = 0; i < 5; i++) {
		Fovea f = {100, 100, 800, 800, 3, 0, 0};
		if(randLimit != 0) {
			f.fx += randLimit - rand()%(randLimit*2);
			f.fy += randLimit - rand()%(randLimit*2);
		}
		f.step.push_back(8);
		f.step.push_back(4);
		f.step.push_back(2);
		f.step.push_back(1);
		mf.foveas.push_back(f);
	}

	imgW = mf.foveas[0].ux;
	imgH = mf.foveas[0].uy;
	pCounter = (unsigned char *) malloc(sizeof(unsigned char)*mf.foveas[0].ux*mf.foveas[0].uy);

	struct timespec tstart={0,0}, tend={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tstart);

	for(int i = 0; i < 1; i++) {
		count = 0;
		if(atoi(argv[1]) == 0)
			mf.reexecutationMethod();
		else if(atoi(argv[1]) == 1)
			mf.pxpxBitmaskMethod();
		else if(atoi(argv[1]) == 2) {
			mf.precomputeLevels();
			mf.pxpxPrevFoveaMethod();
		} else if(atoi(argv[1]) == 3)
			mf.sendBlockMethod();
	//	printf("%d\n", count);
	}
	//printf("%d\n", q); //force the q value to be printed in order to avoid any compiler obscure optimization

	clock_gettime(CLOCK_MONOTONIC, &tend);
	printf("%.5f\n",
			((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
			((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	free(pCounter);

	return 0;
}

