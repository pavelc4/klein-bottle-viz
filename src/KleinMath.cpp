#include "KleinMath.h"
#include <cmath>



Point3D ComputeKlein(float u, float v, float radius){
    float cosU = std::cos(u);
    float sinU = std::sin(u);
    float cosV = std::cos(v);
    float sinV = std::sin(v);

    Point3D point;

    const float HalfTurn = 3.14159265f ;

    if (u < HalfTurn) {
        point.x =  radius * (2.0f + cosU) * cosV;
        point.y = radius * (2.0f + cosU) * sinV;
    } else {
        point.x = radius * (2.0f- cosU ) * cosV;
        point.y = radius * (2.0f - cosU) * sinV;
    }

    point.z = radius * sinU * (u < HalfTurn ? 1.0f : 0.0f);

    return point;

}
