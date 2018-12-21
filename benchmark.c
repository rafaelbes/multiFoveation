#include "fovea.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>


int main(int argc, char **argv) {

	MultiFovea mf;
	FILE *arqs[4];
	char filename[30];

	//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
	//n, w0, wn, ws, u0, un, us, m0, mn, ms, f0, fn, fs
	int params[13];

	for(int i = 0; i < 4; i++) { //for each method
		sprintf(filename, "m%d.txt", i);
		arqs[i] = fopen(filename, "w");
	}

	int randLimit = 90;

	for(int i = 1; i <= 13; i++)
		params[i-1] = atoi(argv[i]);

	srand(time(NULL));

	for(int nf = params[10]; nf <= params[11]; nf += params[12]) {
		mf.foveas.clear();
		for(int i = 0; i < nf; i++) {
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
	}



	return 0;
}

