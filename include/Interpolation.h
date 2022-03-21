// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Interpolation.h - Interpolation classes header
// author: Karl-Mihkel Ott

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#ifdef INTERPOLATOR_CPP
    #include <cmath>
    #include <string>
    #include <vector>
    #include <cstring>
    #include <iostream>

    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <ErrorDefinitions.h>
    #define SQR(x) ((x)*(x))
#endif


namespace DENG {

    struct LinearInterpolator {
        template <typename T>
        inline static T ArithmeticInterp(float _x0, float _x, float _x1, T &_y0, T &_y1) {
            return (_y0 * (_x1 - _x) + _y1 * (_x - _x0)) / (_x1 - _x0);
        }

        static Libdas::Quaternion InterpolateQuaternionRotation(float _x0, float _x, float _x1, Libdas::Quaternion &_y0, Libdas::Quaternion &_y1);
    };


    struct EquationSolver {
        private:
            static void _GaussianElimination(std::vector<std::vector<float>> &_mat);

        public:
            static std::vector<float> SolveEquation(std::vector<std::vector<float>> &_mat);
    };


    struct CubicSplineInterpolator {
        static float InterpolateScalar(float _x0, float _x, float _x1, float _x2, float _y0, float _y1, float _y2);
    };
}

#endif
