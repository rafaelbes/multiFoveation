#include "fovea.h"
#include <string.h>

int main(int argc, char **argv) {

	Block a = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
	Block b = {atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), atoi(argv[8])};

	RefVertex rv = a.getReferenceVertex(b);
	printf("intersect? %d\nreference vertex: %d %d %s\n", a.intersect(b), rv.x, rv.y, directionNames[rv.d]);




	return 0;
}

