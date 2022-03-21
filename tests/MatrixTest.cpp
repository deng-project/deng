
// attempt to solve equation with 3 unknowns using gaussian elimination
// FOR CONTEXT: let's assume that we have following equation system:
//   a1 * x1 + b1 * x2 + c1 * x3 = d1
//   a2 * x1 + b2 * x2 + c2 * x3 = d2
//   a3 * x1 + b3 * x2 + c3 * x3 = d3
// INPUT: floating point numbers in following order
//   a1 b1 c1 d1
//   a2 b2 c2 d2
//   a3 b3 c3 d3

#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

#include <libdas/include/Vector.h>
#include <libdas/include/Quaternion.h>
#include <Interpolation.h>

typedef std::vector<std::vector<float>> arr_2d;

//void g_eliminate(arr_2d &_arr) {

    //// base row iteration
    //for(size_t i = 0; i < _arr.size() - 1; i++) {

        //for(size_t j = i + 1; j < _arr.size(); j++) {
            //// attempt to combine second and third gaussian elementary row operations
            //if(_arr[j][i] / _arr[i][i] != 0) {
                //float mul = -(_arr[j][i] / _arr[i][i]);

                //for(size_t k = 0; k < _arr[j].size(); k++)
                    //_arr[j][k] += (_arr[i][k] * mul);
            //}
            //else { assert(false); }
        //}
    //}
//}

//float *solve(arr_2d &_arr) {
    //static float solution[3] = {};

    //g_eliminate(_arr);
    //solution[2] = 1 / _arr[2][2] * _arr[2][3];
    //solution[1] = 1 / _arr[1][1] * (_arr[1][3] - _arr[1][2] * solution[2]);
    //solution[0] = 1 / _arr[0][0] * (_arr[0][3] - _arr[0][2] * solution[2] - _arr[0][1] * solution[1]);

    //return solution;

//}


int main() {
    arr_2d params(3);
    for(int i = 0; i < 3; i++) {
        params[i].resize(4);
        for(int j = 0; j < 4; j++)
            std::cin >> params[i][j];
    }

    std::vector<float> sol = DENG::EquationSolver::SolveEquation(params);
    for(float v : sol)
        std::cout << v << " ";
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
