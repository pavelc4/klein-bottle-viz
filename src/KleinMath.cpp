#include "KleinMath.h"
#include <cmath>



Point3D ComputeKlein(float u, float v, float radius){
    float u2 = u / 2.0f;
    float cosU2 = std::cos(u2);
    float sinU2 = std::sin(u2);
    float sinV  = std::sin(v);

    Point3D point;

    float r = 3.0f + cosU2 * sinV - sinU2 * std::sin(2.0f * v);
    point.x = radius * r * std::cos(u);
    point.y = radius * r * std::sin(u);
    point.z = radius * (sinU2 * sinV + cosU2 * std::sin(2.0f * v));

    return point;

}
