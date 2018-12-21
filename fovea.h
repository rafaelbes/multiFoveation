#ifndef FOVEA_H
#define FOVEA_H

#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <assert.h>

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

//colors representing how many times a pixel was processed, up to 5 times!
unsigned int colors[] = {0x0, 0xFF0000, 0x00FF00, 0x0000FF, 0xFF00FF};
//how many times a pixel was processed
unsigned char *pCounter;
//image width and height, set at main function
int imgW, imgH;
//count how many features was processed
int count = 0;
//just an aux variable to be used on computeFeature function
int q = 0;
//feature, number of useless iterations
int nIteracoes = 10;


//write a image in PPM format, each pixel has a color associated with how many times it has been processed
void writeImg(int w, int h, char *filename) {
	fprintf(stdout, "Writing to file %s\n", filename);
	FILE *arq = fopen(filename, "w");
	fprintf(arq, "P6\n%d %d\n255\n", w, h);
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++) {
			fwrite(&colors[pCounter[i*w+j]], 3, 1, arq); 
		}
	fclose(arq);
}

//for print purposes
char directionNames[][30] = {"sudeste", "nordeste", "sudoeste", "noroeste"};
enum directions {SOUTHEAST, NORTHEAST, SOUTHWEST, NORTHWEST};

typedef struct RefVertex {
	int x, y, d; //d, 0 (southeast), 1 (northeast), 2 (southwest), 3 (northwest)
} RefVertex;

//compares two reference vertex to be used in the sort algorithm
bool compRV(RefVertex a, RefVertex b) {
	return a.x < b.x;
}

//pretend to process a feature
void computeFeature(int x, int y) {
	count++;
	for(int p = 0; p < nIteracoes; p++) //useless process to take a time
		q += (x*x)%count;
#ifdef WRITEIMG
	pCounter[imgW*y + x]++;
#endif
	//usleep(1); //not works very well
}

typedef struct Block {
	int x, y, sx, sy;

	//tests whether this block intersects with block b
	int intersect(Block b) { //block with same size!
		return MAX(x + sx, b.x + b.sx) - MIN(x, b.x) < 2*sx && MAX(y + sy, b.y + b.sy) - MIN(y, b.y) < 2*sy;
	}

	//returns the reference vertex of the another block b, assumption: they intersect
	//this function is tested on testeIntersections.c
	RefVertex getReferenceVertex(Block b) {
		RefVertex v;
		v.d = 0; //its southeast at first
		if(x >= b.x) {
			v.x = b.x + b.sx;
			v.d += 2; //direction is changed to west
		} else
			v.x = b.x;
		if(y < b.y) {
			v.y = b.y;
			v.d += 1; //direction is changed to north
		} else 
			v.y = b.y + b.sy;
		return v;
	}

	//returns a vector of blocks with pixels that are not at blocks (parameter)
	std::vector<Block> getBlocks(std::vector<Block> blocks) {
		std::vector<Block> res;
		std::vector<RefVertex> rv;

		//get all reference vertex and put them on rv vector
		for(int i = 0; i < blocks.size(); i++) {
			if(!intersect(blocks[i])) continue;
			RefVertex v = getReferenceVertex(blocks[i]);
			rv.push_back(v);
#ifdef DEBUG
			printf("Adding %d %d (%s) as reference vertex\n", v.x, v.y, directionNames[v.d]);
#endif
		}
		//if there is no intersection
		if(rv.size() == 0) {
			Block r = {x, y, sx, sy};
			res.push_back(r);
			return res;
		}

		//creates (t+1) regions
		std::sort(rv.begin(), rv.end(), compRV);
		std::vector<int> lowerBound;
		std::vector<int> upperBound;
		for(int i = 0; i <= rv.size(); i++) {
			lowerBound.push_back(y);
			upperBound.push_back(y+sy);
		}
#ifdef DEBUG
		for(int i = 0; i <= rv.size(); i++) {
			printf("block %d = [%d %d]\n", i, lowerBound[i], upperBound[i]);
		}
#endif
		//adjusts the lower and upperBound for each region
		for(int i = 0; i < rv.size(); i++) {
			if(rv[i].d == NORTHWEST) {
				for(int j = 0; j <= i; j++)
					upperBound[j] = MIN(upperBound[j], rv[i].y);
			} else if(rv[i].d == NORTHEAST) {
				for(int j = i+1; j <= rv.size(); j++)
					upperBound[j] = MIN(upperBound[j], rv[i].y);
			} else if(rv[i].d == SOUTHEAST) {
				for(int j = i+1; j <= rv.size(); j++)
					lowerBound[j] = MAX(lowerBound[j], rv[i].y);
			} else {
				for(int j = 0; j <= i; j++)
					lowerBound[j] = MAX(lowerBound[j], rv[i].y);
			}
		}
#ifdef DEBUG
		for(int i = 0; i <= rv.size(); i++) {
			printf("block %d = [%d %d]\n", i, lowerBound[i], upperBound[i]);
		}
#endif

		//creates blocks using lower and upperBound vectors, append them to res vector
		Block b;
		b.x = x;
		b.y = lowerBound[0];
		b.sx = rv[0].x - b.x;
		b.sy = upperBound[0] - lowerBound[0];
		if(b.sx > 0 && b.sy > 0)
			res.push_back(b);
		for(int i = 0; i < rv.size(); i++) {
			b.x = rv[i].x;
			b.y = lowerBound[i+1];
			if(i == rv.size()-1)
				b.sx = x + sx - b.x;
			else
				b.sx = rv[i+1].x - b.x;
			b.sy = upperBound[i+1] - lowerBound[i+1];
			if(b.sx > 0 && b.sy > 0)
				res.push_back(b);
		}
		return res;

	}

} Block;

typedef struct Fovea {

	int wx, wy, ux, uy, m, fx, fy;
	std::vector<int> step;
	std::vector<Block> levels;

	//compute any block b using step between pixels
	void computeBlock(Block b, int step) {
		int x0 = step*(b.x/step);
		int y0 = step*(b.y/step);
		for(int j = y0; j < b.y + b.sy; j += step)
			for(int i = x0; i < b.x + b.sx; i += step) {
				//printf("%d %d with step %d\n", i, j, step);
				computeFeature(i, j);
			}
	}

	void precomputeLevels() {
		for(int k = 0; k < step.size(); k++)
			levels.push_back(getBlock(k));
	}

	int delta(int k, int u, int w, int f) {
		return (k*(u - w + 2*f))/(2*m);
	}

	int size(int k, int u, int w, int f) {
		return (k*w - k*u + m*u)/m;
	}

	//return the block of level k
	Block getBlock(int k) {
		Block b = {delta(k, ux, wx, fx), delta(k, uy, wy, fy), size(k, ux, wx, fx), size(k, uy, wy, fy)};
		return b;
	}

	//test whether the point (x, y) is inside level k
	int insideLevel(int x, int y, int k) {
		Block b = levels[k];//getBlock(k);
		return x >= b.x && x < b.x + b.sx && y >= b.y && y < b.y + b.sy;
	}

	//compute all the levels
	void compute() {
		for(int k = 0; k < step.size(); k++) {
			Block b = getBlock(k);
			computeBlock(b, step[k]);
		}
	}
	

} Fovea;

typedef struct MultiFovea {

	std::vector<Fovea> foveas;
	unsigned char *bitmask;

	void precomputeLevels() {
		for(int i = 0; i < foveas.size(); i++)
			foveas[i].precomputeLevels();
	}

	//apply compute method for all foveas
	void reexecutationMethod() {
		for(int i = 0; i < foveas.size(); i++)
			foveas[i].compute();
	}

	//apply sending blocks method
	void sendBlockMethod() {
		for(int k = 0; k < foveas[0].step.size(); k++) {
#ifdef WRITEIMG
			memset(pCounter, 0, imgW*imgH);
#endif
			for(int f = 0; f < foveas.size(); f++) {
				Fovea c = foveas[f]; //current fovea
				std::vector<RefVertex> rv;
				Block a = foveas[f].getBlock(k); //current block (level)
				std::vector<Block> pb; //previous blocks from previous foveas
				for(int pf = 0; pf < f; pf++) {
					Block b = foveas[pf].getBlock(k);
					pb.push_back(b);
				}
				std::vector<Block> set = a.getBlocks(pb);
				for(int i = 0; i < set.size(); i++) {
#ifdef DEBUG
					printf("Sending block at %d %d with size %d %d, step %d\n", set[i].x, set[i].y, set[i].sx, set[i].sy, foveas[f].step[k]);
#endif
					c.computeBlock(set[i], foveas[f].step[k]);
				}
			}
#ifdef WRITEIMG
			char filename[100];
			sprintf(filename, "level%d.ppm", k);
			writeImg(imgW, imgH, filename);
#endif
		}
	}

	void pxpxBitmaskMethod() {
		int ux = foveas[0].ux;
		int uy = foveas[0].uy;
		bitmask = (unsigned char *) malloc(ux*uy*sizeof(unsigned char));
		for(int k = 0; k < foveas[0].step.size(); k++) {
			memset(bitmask, 0, ux*uy);
#ifdef WRITEIMG
			memset(pCounter, 0, imgW*imgH);
#endif
			for(int f = 0; f < foveas.size(); f++) {
				Block b = foveas[f].getBlock(k);
				int step = foveas[f].step[k];
				int x0 = step*(b.x/step);
				int y0 = step*(b.y/step);
#ifdef DEBUG
				printf("Bitmask block at %d %d with size %d %d, step %d\n", x0, y0, b.sx, b.sy, step);
#endif
				for(int j = y0; j < b.y + b.sy; j += step) {
					for(int i = x0; i < b.x + b.sx; i += step) {
						if(!bitmask[j*ux+i]) {
							//printf("%d %d with step %d (at %d)\n", i, j, step, j*ux + i);
							computeFeature(i, j);
							bitmask[j*ux+i] = 1;
						} else {
							//printf("%d %d with step %d (at %d) [skipping]\n", i, j, step, j*ux + i);
						}
					}
				}
			}
#ifdef WRITEIMG
			char filename[100];
			sprintf(filename, "level%d.ppm", k);
			writeImg(imgW, imgH, filename);
#endif
		}
		free(bitmask);
	}

	//the pixel-by-pixel method
	void pxpxPrevFoveaMethod() {
		//you should call precomputeLevels before this function!
		assert(foveas[0].levels.size() > 0);
		for(int k = 0; k < foveas[0].step.size(); k++) {
#ifdef WRITEIMG
			memset(pCounter, 0, imgW*imgH);
#endif
			for(int f = 0; f < foveas.size(); f++) {
				Block b = foveas[f].getBlock(k);
				int step = foveas[f].step[k];
				int x0 = step*(b.x/step);
				int y0 = step*(b.y/step);
				for(int j = y0; j < b.y + b.sy; j += step) {
					for(int i = x0; i < b.x + b.sx; i += step) {
						int inside = 0;
						for(int pf = 0; pf < f; pf++) {
							if(foveas[pf].insideLevel(i, j, k)) {
								inside = 1;
								break;
							}
						}
						if(!inside) {
							computeFeature(i, j);
						}
					}
				}
			}
#ifdef WRITEIMG
			char filename[100];
			sprintf(filename, "level%d.ppm", k);
			writeImg(imgW, imgH, filename);
#endif
		}
	}


} MultiFovea;


#endif

