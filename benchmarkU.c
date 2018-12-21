#include "fovea.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	MultiFovea mf;
	FILE *arqs[4];
	char filename[30];

	for(int i = 0; i < 4; i++) { //for each method
		sprintf(filename, "m%d.txt", i);
		arqs[i] = fopen(filename, "w");
	}

	int randLimit = 90;

	srand(time(NULL));
	int U;


	int max = 30000;
	for(U = 500; U <= max; U += max/10) {
		mf.foveas.clear();
		for(int i = 0; i < 5; i++) {
			Fovea f = {100, 100, U, U, 3, 0, 0};
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
		for(int m = 0; m < 4; m++) {
			printf("Method %d with size %d\n", m, U);
			struct timespec tstart={0,0}, tend={0,0};
			clock_gettime(CLOCK_MONOTONIC, &tstart);

			count = 0;
			switch(m) {
				case 0:
					mf.reexecutationMethod();
					break;
				case 1:
					mf.pxpxBitmaskMethod();
					break;
				case 2:
					mf.precomputeLevels();
					mf.pxpxPrevFoveaMethod();
					break;
				case 3:
					mf.sendBlockMethod();
					break;
			}

			clock_gettime(CLOCK_MONOTONIC, &tend);
			fprintf(arqs[m], "%.5f\n",
					((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
					((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
			printf("%d\n", q);
		}
	}

	return 0;
}

