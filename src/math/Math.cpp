#include "Math.hpp"

#include "math/cubicSolver_adsk.h"

int math::cubic_solver(float a, float b, float c, float d, float *roots)
{
    // using Autodesk's Cubic Solver as mine is not working properly every time...
    float poly[4];
    poly[3] = a;
    poly[2] = b;
    poly[1] = c;
    poly[0] = d;
    float dt = 0.05f;
    init_tolerance();
    int numRoots = polyZeroes(poly, 3, 0.0 - dt, 1, 1.0 + dt, 1, roots);
    numRoots = numRoots == -1 ? 0 : numRoots;
    return numRoots;
}