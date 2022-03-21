// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Interpolation.cpp - Interpolation classes implementation
// author: Karl-Mihkel Ott

#define INTERPOLATOR_CPP
#include <Interpolation.h>


namespace DENG {

    Libdas::Quaternion LinearInterpolator::InterpolateQuaternionRotation(float _x0, float _x, float _x1, Libdas::Quaternion &_y0, Libdas::Quaternion &_y1) {
        float hrot0 = acos(_y0.w);
        float hrot1 = acos(_y1.w);
        float sin_hrot0 = sinf(hrot0);
        float sin_hrot1 = sinf(hrot1);

        Libdas::Vector3<float> unit0(_y0.x / sin_hrot0, _y0.y / sin_hrot0, _y0.z / sin_hrot0);
        Libdas::Vector3<float> unit1(_y1.x / sin_hrot1, _y1.y / sin_hrot1, _y1.z / sin_hrot1);

        float rot = LinearInterpolator::ArithmeticInterp<float>(_x0, _x, _x1, hrot0, hrot1);
        Libdas::Vector3<float> unit = LinearInterpolator::ArithmeticInterp<Libdas::Vector3<float>>(_x0, _x, _x1, unit0, unit1);

        sin_hrot0 = sinf(rot);
        return Libdas::Quaternion(sin_hrot0 * unit.first, unit.second * sin_hrot0, unit.third * sin_hrot0, cosf(rot));
    }


    void EquationSolver::_GaussianElimination(std::vector<std::vector<float>> &_mat) {
        // base row iteration
        for(size_t i = 0; i < _mat.size() - 1; i++) {

            for(size_t j = i + 1; j < _mat.size(); j++) {
                // attempt to combine second and third gaussian elementary row operations
                if(_mat[i][i] != 0 && _mat[j][i] / _mat[i][i] != 0) {
                    float mul = -(_mat[j][i] / _mat[i][i]);

                    for(size_t k = 0; k < _mat[j].size(); k++)
                        _mat[j][k] += (_mat[i][k] * mul);
                }
            }
        }
    }


    std::vector<float> EquationSolver::SolveEquation(std::vector<std::vector<float>> &_mat) {
        DENG_ASSERT(_mat.size());
        DENG_ASSERT(_mat.size() != _mat[0].size() + 1);

        _GaussianElimination(_mat);
        std::vector<float> solution(_mat.size());
        for(int i = static_cast<int>(solution.size() - 1); i >= 0; i--) {
            solution[i] = /* 1 / _mat[i][i] * */ _mat[i][_mat.size()];

            for(int j = static_cast<int>(solution.size() - 1); j > i; j--)
                solution[i] -= _mat[i][j] * solution[j];

            solution[i] /= _mat[i][i];
        }

        return solution;
    }


    float CubicSplineInterpolator::InterpolateScalar(float _x0, float _x, float _x1, float _x2, float _y0, float _y1, float _y2) {
        // https://en.wikipedia.org/wiki/Spline_interpolation
        Libdas::Matrix3<float> tri_diag_mat = {
            { 2 / (_x1 - _x0), 1 / (_x1 - _x0), 0 },
            { 1 / (_x1 - _x0), 2 * ( (1 / (_x1 - _x0)) + (1 / (_x2 - _x1)) ), 1 / (_x2 - _x1) },
            { 0, 1 / (_x2 - _x1), 2 / (_x2 - _x1) }
        };

        Libdas::Vector3<float> b = { 
            3 * (_y1 - _y0) / SQR(_x1 - _x0),
            3 * ( (_y1 - _y0) / SQR(_x1 - _x0) + (_y2 - _y1) / SQR(_x2 - _x1) ),
            3 * (_y2 - _y1) / SQR(_x2 - _x1)
        };

        std::vector<std::vector<float>> augmented_matrix = {
            { tri_diag_mat.row1.first, tri_diag_mat.row1.second, tri_diag_mat.row1.third, b.first },
            { tri_diag_mat.row2.first, tri_diag_mat.row2.second, tri_diag_mat.row2.third, b.second },
            { tri_diag_mat.row3.first, tri_diag_mat.row3.second, tri_diag_mat.row3.third, b.third }
        };

        auto k = EquationSolver::SolveEquation(augmented_matrix);

        std::cout << "k = (" << k[0] << ", " << k[1] << ", " << k[2] << ")" << std::endl;

        float a1 = k[0] * (_x1 - _x0) - _y1 + _y0;
        float a2 = k[1] * (_x2 - _x1) - _y2 + _y1; 

        std::cout << "a1 = " << a1 << std::endl;
        std::cout << "a2 = " << a2 << std::endl;

        b.third = -k[2] * (_x2 - _x1) + (_y2 - _y1); 

        const float t = (_x - _x1) / (_x2 - _x1);
        float y = (1 - t) * _y1 + t * _y2 + t * (1 - t) * ( (1 - t) * a2 + t * b.third );
        return y;
    }
}
