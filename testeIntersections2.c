#include "fovea.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	FILE *arq = fopen(argv[1], "r");
	if(!arq) return 1;

	int x, y, sx, sy;

	Block b;
	fscanf(arq, "%d %d %d %d", &b.x, &b.y, &b.sx, &b.sy);

	std::vector<Block> blocks;
	while(fscanf(arq, "%d %d %d %d", &x, &y, &sx, &sy) == 4) {
		Block a = {x, y, sx, sy};
		blocks.push_back(a);
	}

	std::vector<Block> res;
	res = b.getBlocks(blocks);
	for(int i = 0; i < res.size(); i++)
		printf("%d %d %d %d\n", res[i].x, res[i].y, res[i].sx, res[i].sy);

	fclose(arq);

	return 0;
}

