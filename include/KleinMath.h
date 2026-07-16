#ifndef KLEIN_MATH_H
#define KLEIN_MATH_H


struct Point3D {
    float x, y, z;
};

Point3D ComputeKlein(float u, float v, float radius);

#endif
