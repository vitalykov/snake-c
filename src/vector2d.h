#ifndef VECTOR2D_H
#define VECTOR2D_H

struct Vector {
    int x;
    int y;
};

typedef struct Vector Vector;

int EqualVectors(Vector v1, Vector v2);

#endif  // VECTOR2D_H