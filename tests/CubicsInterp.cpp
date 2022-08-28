// args: x, x0, x1, x2, y0, y1, y2

#include <iostream>
#include <cmath>
#include <vector>

#include <libdas/include/Vector.h>
#include <libdas/include/Quaternion.h>
#include <Interpolation.h>

int main() {
    float args[7] = {};
    for(int i = 0; i < 7; i++)
        std::cin >> args[i];

    
    float y =DENG::CubicSplineInterpolator::InterpolateScalar(args[1], args[0], args[2], args[3], args[4], args[5], args[6]);
    std::cout << "y = " << y << std::endl;
    
    return 0;
}
