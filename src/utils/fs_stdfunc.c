#include "fs_stdfunc.h"

unsigned long hash(unsigned const char* str) {
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}


Point* cnormal(Point a, Point b, Point c) {
    Point p, q;
    Point* r = (Point*)calloc(1, sizeof(Point));
    double val;
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;

    r->x = p.y * q.z - p.z * q.y;
    r->y = p.z * q.x - p.x * q.z;
    r->z = p.x * q.y - p.y * q.x;

    val = sqrt(r->x * r->x + r->y * r->y + r->z * r->z);
    r->x /= val;
    r->y /= val;
    r->z /= val;
    return r;
}
