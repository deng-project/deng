/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat4.cpp - DENG 4x4 matrix class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE INTO SEPERATE UNIT!!!
#ifndef __MAT4_CPP
#define __MAT4_CPP

namespace dengMath {
    
    /// Add two matrices together
    template<typename T>
    mat4<T> mat4<T>::operator+(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out;
            out.row1 = {row1.first + mat.row1.first, row1.second + mat.row1.second, row1.third + mat.row1.third, row1.fourth + mat.row1.fourth};
            out.row2 = {row2.first + mat.row2.first, row2.second + mat.row2.second, row2.third + mat.row2.third, row2.fourth + mat.row2.fourth};
            out.row3 = {row3.first + mat.row3.first, row3.second + mat.row3.second, row3.third + mat.row3.third, row3.fourth + mat.row3.fourth};
            out.row4 = {row4.first + mat.row4.first, row4.second + mat.row4.second, row4.third + mat.row4.third, row4.fourth + mat.row4.fourth};

            return out;
        }

        else return mat4<T>{};
    }


    /// Add constant to the current matrix
    template<typename T>
    mat4<T> mat4<T>::operator+(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out{};
            out.row1 = vec4<T>{row1.first + c, row1.second + c, row1.third + c, row1.fourth + c};
            out.row2 = vec4<T>{row2.first + c, row2.second + c, row2.third + c, row2.fourth + c};
            out.row3 = vec4<T>{row3.first + c, row3.second + c, row3.third + c, row3.fourth + c};
            out.row4 = vec4<T>{row4.first + c, row4.second + c, row4.third + c, row4.fourth + c};

            return out;
        }

        else return mat4<T>{};
    }


    /// Substract current matrix with given matrix
    template<typename T>
    mat4<T> mat4<T>::operator-(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out{};
            out.row1 = vec4<T>{row1.first - mat.row1.first, row1.second - mat.row1.second, row1.third - mat.row1.third, row1.fourth - mat.row1.fourth};
            out.row2 = vec4<T>{row2.first - mat.row2.first, row2.second - mat.row2.second, row2.second - mat.row2.third, row2.fourth - mat.row2.fourth};
            out.row3 = vec4<T>{row3.first - mat.row3.first, row3.second - mat.row3.second, row3.second - mat.row3.third, row3.fourth - mat.row3.fourth};
            out.row4 = vec4<T>{row4.first - mat.row4.first, row4.second - mat.row4.second, row4.second - mat.row4.third, row4.fourth - mat.row4.fourth};

            return out;
        }

        else return mat4<T>{};
    }


    /// Substract a constant number from the current matrix
    template<typename T>
    mat4<T> mat4<T>::operator-(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out{};
            out.row1 = vec4<T>{row1.first - c, row1.second - c, row1.third - c, row1.fourth - c};
            out.row2 = vec4<T>{row2.first - c, row2.second - c, row2.third - c, row2.fourth - c};
            out.row3 = vec4<T>{row3.first - c, row3.second - c, row3.third - c, row3.fourth - c};
            out.row4 = vec4<T>{row4.first - c, row4.second - c, row4.third - c, row4.fourth - c};

            return out;
        }

        else return mat4<T>{};
    }


    /// Multiply all matrix members with a constant
    template<typename T>
    mat4<T> mat4<T>::operator*(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out;
            out.row1 = {row1.first * c, row1.second * c, row1.third * c, row1.fourth * c};
            out.row2 = {row2.first * c, row2.second * c, row2.third * c, row2.fourth * c};
            out.row3 = {row3.first * c, row3.second * c, row3.third * c, row3.fourth * c};
            out.row4 = {row4.first * c, row4.second * c, row4.third * c, row4.fourth * c};
            return out;
        }

        else return mat4<T>{};
    }

    
    /// Find the dot product of two matrices
    template<typename T>
    mat4<T> mat4<T>::operator*(const mat4<T> &matrix) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out_mat;
            out_mat.row1 = vec4<T>{
                (
                    (row1.first * matrix.row1.first) + 
                    (row1.second * matrix.row2.first) + 
                    (row1.third * matrix.row3.first) + 
                    (row1.fourth * matrix.row4.first)
                ), 
                (
                    (row1.first * matrix.row1.second) + 
                    (row1.second * matrix.row2.second) + 
                    (row1.third * matrix.row3.second) + 
                    (row1.fourth * matrix.row4.second)
                ), 
                (
                    (row1.first * matrix.row1.third) + 
                    (row1.second * matrix.row2.third) + 
                    (row1.third * matrix.row3.third) + 
                    (row1.fourth * matrix.row4.third)
                ), 
                (
                    (row1.first * matrix.row1.fourth) + 
                    (row1.second * matrix.row2.fourth) + 
                    (row1.third * matrix.row3.fourth) + 
                    (row1.fourth * matrix.row4.fourth)
                )
            };

            out_mat.row2 = vec4<T>{ 
                (
                    (row2.first * matrix.row1.first) + 
                    (row2.second * matrix.row2.first) + 
                    (row2.third * matrix.row3.first) + 
                    (row2.fourth * matrix.row4.first)
                ), 
                (
                    (row2.first * matrix.row1.second) + 
                    (row2.second * matrix.row2.second) + 
                    (row2.third * matrix.row3.second) + 
                    (row2.fourth * matrix.row4.second)
                ), 
                (
                    (row2.first * matrix.row1.third) + 
                    (row2.second * matrix.row2.third) + 
                    (row2.third * matrix.row3.third) + 
                    (row2.fourth * matrix.row4.third)
                ), 
                (
                    (row2.first * matrix.row1.fourth) + 
                    (row2.second * matrix.row2.fourth) + 
                    (row2.third * matrix.row3.fourth) + 
                    (row2.fourth * matrix.row4.fourth)
                )
            };

            out_mat.row3 = vec4<T>{
                (
                    (row3.first * matrix.row1.first) + 
                    (row3.second * matrix.row2.first) + 
                    (row3.third * matrix.row3.first) + 
                    (row3.fourth * matrix.row4.first)
                ), 
                (
                    (row3.first * matrix.row1.second) + 
                    (row3.second * matrix.row2.second) + 
                    (row3.third * matrix.row3.second) + 
                    (row3.fourth * matrix.row4.second)
                ), 
                (
                    (row3.first * matrix.row1.third) + 
                    (row3.second * matrix.row2.third) + 
                    (row3.third * matrix.row3.third) + 
                    (row3.fourth * matrix.row4.third)
                ), 
                (
                    (row3.first * matrix.row1.fourth) + 
                    (row3.second * matrix.row2.fourth) + 
                    (row3.third * matrix.row3.fourth) + 
                    (row3.fourth * matrix.row4.fourth)
                )
            };

            out_mat.row4 = vec4<T>{
                (
                    (row4.first * matrix.row1.first) + 
                    (row4.second * matrix.row2.first) + 
                    (row4.third * matrix.row3.first) + 
                    (row4.fourth * matrix.row4.first)
                ), 
                (
                    (row4.first * matrix.row1.second) + 
                    (row4.second * matrix.row2.second) + 
                    (row4.third * matrix.row3.second) + 
                    (row4.fourth * matrix.row4.second)
                ), 
                (
                    (row4.first * matrix.row1.third) + 
                    (row4.second * matrix.row2.third) + 
                    (row4.third * matrix.row3.third) + 
                    (row4.fourth * matrix.row4.third)
                ), 
                (
                    (row4.first * matrix.row1.fourth) + 
                    (row4.second * matrix.row2.fourth) + 
                    (row4.third * matrix.row3.fourth) + 
                    (row4.fourth * matrix.row4.fourth)
                )
            };

            return out_mat;
        }

        else return mat4<T>{};
    }


    /// Multiply with column vector
    template<typename T>
    vec4<T> mat4<T>::operator*(const vec4<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out_vec;
            out_vec.first = {
                vec.first * row1.first + 
                vec.second * row1.second + 
                vec.third * row1.third +
                vec.fourth * row1.fourth
            };

            out_vec.second = {
                vec.first * row2.first +
                vec.second * row2.second +
                vec.third * row2.third +
                vec.fourth * row2.fourth
            };

            out_vec.third = {
                vec.first * row3.first +
                vec.second * row3.second +
                vec.third * row3.third +
                vec.fourth * row3.fourth
            };

            out_vec.fourth = {
                vec.first * row4.first +
                vec.second * row4.second +
                vec.third * row4.third +
                vec.fourth * row4.fourth
            };

            return out_vec;
        }

        else return vec4<T>{};
    }


    /// Divide all matrix elements with a constant
    template<typename T>
    mat4<T> mat4<T>::operator/(const T &c) { 
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> out;
            out.row1 = vec4<T>{row1.first / c, row1.second / c, row1.third / c, row1.fourth / c};
            out.row2 = vec4<T>{row2.first / c, row2.second / c, row2.third / c, row2.fourth / c};
            out.row3 = vec4<T>{row3.first / c, row3.second / c, row3.third / c, row3.fourth / c};
            out.row4 = vec4<T>{row4.first / c, row4.second / c, row4.third / c, row4.fourth / c};

            return out;
        }

        else return mat4<T>{};
    }


    /// Multiply all matrix members with a constant and set the product as the value of the current matrix instance
	template<typename T>
    void mat4<T>::operator*=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first *= c;
            row1.second *= c;
            row1.third *= c;
            row1.fourth *= c;

            row2.first *= c;
            row2.second *= c;
            row2.third *= c;
            row2.fourth *= c;

            row3.first *= c;
            row3.second *= c;
            row3.third *= c;
            row3.fourth *= c;

            row4.first *= c;
            row4.second *= c;
            row4.third *= c;
            row4.fourth *= c;
        }
    }


    /// Find the cross product of two matrices and set the current matrix instance value to it
    template<typename T>
    void mat4<T>::operator*=(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> new_mat{};
            new_mat.row1 = vec4<T>{
                (
                    (row1.first * mat.row1.first) + 
                    (row1.second * mat.row2.first) + 
                    (row1.third * mat.row3.first) + 
                    (row1.fourth * mat.row4.first)
                ), 
                (
                    (row1.first * mat.row1.second) + 
                    (row1.second * mat.row2.second) + 
                    (row1.third * mat.row3.second) + 
                    (row1.fourth * mat.row4.second)
                ), 
                (
                    (row1.first * mat.row1.third) + 
                    (row1.second * mat.row2.third) + 
                    (row1.third * mat.row3.third) + 
                    (row1.fourth * mat.row4.third)
                ), 
                (
                    (row1.first * mat.row1.fourth) + 
                    (row1.second * mat.row2.fourth) + 
                    (row1.third * mat.row3.fourth) + 
                    (row1.fourth * mat.row4.fourth)
                )
            };

            new_mat.row2 = vec4<T>{ 
                (
                    (row2.first * mat.row1.first) + 
                    (row2.second * mat.row2.first) + 
                    (row2.third * mat.row3.first) + 
                    (row2.fourth * mat.row4.first)
                ), 
                (
                    (row2.first * mat.row1.second) + 
                    (row2.second * mat.row2.second) + 
                    (row2.third * mat.row3.second) + 
                    (row2.fourth * mat.row4.second)
                ), 
                (
                    (row2.first * mat.row1.third) + 
                    (row2.second * mat.row2.third) + 
                    (row2.third * mat.row3.third) + 
                    (row2.fourth * mat.row4.third)
                ), 
                (
                    (row2.first * mat.row1.fourth) + 
                    (row2.second * mat.row2.fourth) + 
                    (row2.third * mat.row3.fourth) + 
                    (row2.fourth * mat.row4.fourth)
                )
            };

            new_mat.row3 = vec4<T>{
                (
                    (row3.first * mat.row1.first) + 
                    (row3.second * mat.row2.first) + 
                    (row3.third * mat.row3.first) + 
                    (row3.fourth * mat.row4.first)
                ), 
                (
                    (row3.first * mat.row1.second) + 
                    (row3.second * mat.row2.second) + 
                    (row3.third * mat.row3.second) + 
                    (row3.fourth * mat.row4.second)
                ), 
                (
                    (row3.first * mat.row1.third) + 
                    (row3.second * mat.row2.third) + 
                    (row3.third * mat.row3.third) + 
                    (row3.fourth * mat.row4.third)
                ), 
                (
                    (row3.first * mat.row1.fourth) + 
                    (row3.second * mat.row2.fourth) + 
                    (row3.third * mat.row3.fourth) + 
                    (row3.fourth * mat.row4.fourth)
                )
            };

            new_mat.row4 = vec4<T>{
                (
                    (row4.first * mat.row1.first) + 
                    (row4.second * mat.row2.first) + 
                    (row4.third * mat.row3.first) + 
                    (row4.fourth * mat.row4.first)
                ), 
                (
                    (row4.first * mat.row1.second) + 
                    (row4.second * mat.row2.second) + 
                    (row4.third * mat.row3.second) + 
                    (row4.fourth * mat.row4.second)
                ), 
                (
                    (row4.first * mat.row1.third) + 
                    (row4.second * mat.row2.third) + 
                    (row4.third * mat.row3.third) + 
                    (row4.fourth * mat.row4.third)
                ), 
                (
                    (row4.first * mat.row1.fourth) + 
                    (row4.second * mat.row2.fourth) + 
                    (row4.third * mat.row3.fourth) + 
                    (row4.fourth * mat.row4.fourth)
                )
            };

            *this = new_mat;
        }
    }


    /// Add constant value to matrix and store the value in current matrix instance
    template<typename T>
    void mat4<T>::operator+=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first += c;
            row1.second += c;
            row1.third += c;
            row1.fourth += c;

            row2.first += c;
            row2.second += c;
            row2.third += c;
            row2.fourth += c;

            row3.first += c;
            row3.second += c;
            row3.third += c;
            row3.fourth += c;

            row4.first += c;
            row4.second += c;
            row4.third += c;
            row4.fourth += c;
        }
    }


    /// Add two matrices together and store the value in current matrix instance
    template<typename T>
    void mat4<T>::operator+=(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first += mat.row1.first;
            row1.second += mat.row1.second;
            row1.third += mat.row1.third;
            row1.fourth += mat.row1.fourth;

            row2.first += mat.row2.first;
            row2.second += mat.row2.second;
            row2.third += mat.row2.third;
            row2.fourth += mat.row2.fourth;

            row3.first += mat.row3.first;
            row3.second += mat.row3.second;
            row3.third += mat.row3.third;
            row3.fourth += mat.row3.fourth;

            row4.first += mat.row4.first;
            row4.second += mat.row4.second;
            row4.third += mat.row4.third;
            row4.fourth += mat.row4.fourth;
        }
    }


    /// Substract constant value from matrix and store the result in current matrix instance
    template<typename T>
    void mat4<T>::operator-=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first -= c;
            row1.second -= c;
            row1.third -= c;
            row1.fourth -= c;

            row2.first -= c;
            row2.second -= c;
            row2.third -= c;
            row2.fourth -= c;

            row3.first -= c;
            row3.second -= c;
            row3.third -= c;
            row3.fourth -= c;

            row4.first -= c;
            row4.second -= c;
            row4.third -= c;
            row4.fourth -= c;
        }
    }


    /// Substract a matrix from current matrix and store the result in current matrix instance
    template<typename T>
    void mat4<T>::operator-=(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first -= mat.row1.first;
            row1.second -= mat.row1.second;
            row1.third -= mat.row1.third;
            row1.fourth -= mat.row1.fourth;

            row2.first -= mat.row2.first;
            row2.second -= mat.row2.second;
            row2.third -= mat.row2.third;
            row2.fourth -= mat.row2.fourth;

            row3.first -= mat.row3.first;
            row3.second -= mat.row3.second;
            row3.third -= mat.row3.third;
            row3.fourth -= mat.row3.fourth;

            row4.first -= mat.row4.first;
            row4.second -= mat.row4.second;
            row4.third -= mat.row4.third;
            row4.fourth -= mat.row4.fourth;
        }
    }


    /// Divide all matrix elements with constant and store the value in current matrix instance
    template<typename T>
    void mat4<T>::operator/=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first /= c;
            row1.second /= c;
            row1.third /= c;
            row1.fourth /= c;

            row2.first /= c;
            row2.second /= c;
            row2.third /= c;
            row2.fourth /= c;

            row3.first /= c;
            row3.second /= c;
            row3.third /= c;
            row3.fourth /= c;

            row4.first /= c;
            row4.second /= c;
            row4.third /= c;
            row4.fourth /= c;
        }
    }
    
    
    /// Check if current and given matrix instances have equal values
    template<typename T>
    deng_bool_t mat4<T>::operator==(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (deng_bool_t) (
                row1 == mat.row1 &&
                row2 == mat.row2 &&
                row3 == mat.row3 &&
                row4 == mat.row4
            );       
        }

        else return false;
    }

    
    /// Find the determinant of current matrix instance
    template<typename T>
    template<typename DT>
    DT mat4<T>::det(const mat4<DT> &mat) {
        mat3<DT> adj_mat[4];
        
        adj_mat[0].row1 = vec3<deng_vec_t>{mat.row2.second, mat.row2.third, mat.row2.fourth};
        adj_mat[0].row2 = vec3<deng_vec_t>{mat.row3.second, mat.row3.third, mat.row3.fourth};
        adj_mat[0].row3 = vec3<deng_vec_t>{mat.row4.second, mat.row4.third, mat.row4.fourth};

        adj_mat[1].row1 = vec3<deng_vec_t>{mat.row2.first, mat.row2.third, mat.row2.fourth};
        adj_mat[1].row2 = vec3<deng_vec_t>{mat.row3.first, mat.row3.third, mat.row3.fourth};
        adj_mat[1].row3 = vec3<deng_vec_t>{mat.row4.first, mat.row4.third, mat.row4.fourth};

        adj_mat[2].row1 = vec3<deng_vec_t>{mat.row2.first, mat.row2.second, mat.row2.fourth};
        adj_mat[2].row2 = vec3<deng_vec_t>{mat.row3.first, mat.row3.second, mat.row3.fourth};
        adj_mat[2].row3 = vec3<deng_vec_t>{mat.row4.first, mat.row4.second, mat.row4.fourth};

        adj_mat[3].row1 = vec3<deng_vec_t>{mat.row2.first, mat.row2.second, mat.row2.third};
        adj_mat[3].row2 = vec3<deng_vec_t>{mat.row3.first, mat.row3.second, mat.row3.third};
        adj_mat[3].row3 = vec3<deng_vec_t>{mat.row4.first, mat.row4.second, mat.row4.third};

        vec4<DT> out;
        out.first = mat.row1.first * mat3<DT>::det(adj_mat[0]);
        out.second = mat.row1.second * mat3<DT>::det(adj_mat[1]);
        out.third = mat.row1.third * mat3<DT>::det(adj_mat[2]);
        out.fourth = mat.row1.fourth * mat3<DT>::det(adj_mat[3]); 
        
        return (DT) (
            out.first -
            out.second +
            out.third -
            out.fourth
        );
    };


    /// Find the inverse of the current matrix 
    template<typename T>
    mat4<T> mat4<T>::inv() {
        deng_vec_t inv_det = 1 / mat4<T>::det(*this);
        mat4<T> out_mat;
        mat3<deng_vec_t> adj_mat;
        
        // Row 1
        adj_mat = {
            {row2.second, row2.third, row2.fourth},
            {row3.second, row3.third, row3.fourth},
            {row4.second, row4.third, row4.fourth}
        };
        out_mat.row1.first = (T) (inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.second, row1.third, row1.fourth},
            {row3.second, row3.third, row3.fourth},
            {row4.second, row4.third, row4.fourth}
        };
        out_mat.row1.second = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.second, row1.third, row1.fourth},
            {row2.second, row2.third, row2.fourth},
            {row4.second, row4.third, row4.fourth}
        };
        out_mat.row1.third = (T) (inv_det * mat3<T>::det(adj_mat));
        
        adj_mat = {
            {row1.second, row1.third, row1.fourth},
            {row2.second, row2.third, row2.fourth},
            {row3.second, row3.third, row3.fourth}
        };
        out_mat.row1.fourth = (T) (-inv_det * mat3<T>::det(adj_mat)); 

        // Row 2
        adj_mat = {
            {row2.first, row2.third, row2.fourth},
            {row3.first, row3.third, row3.fourth},
            {row4.first, row4.third, row4.fourth}
        };
        out_mat.row2.first = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.third, row1.fourth},
            {row3.first, row3.third, row3.fourth},
            {row4.first, row4.third, row4.fourth}
        };
        out_mat.row2.second = (T) (inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.third, row1.fourth},
            {row2.first, row2.third, row2.fourth},
            {row4.first, row4.third, row4.fourth}
        };
        out_mat.row2.third = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.third, row1.fourth},
            {row2.first, row2.third, row2.fourth},
            {row3.first, row3.third, row3.fourth}
        };
        out_mat.row2.fourth = (T) (inv_det * mat3<T>::det(adj_mat)); 

        // Row 3
        adj_mat = {
            {row2.first, row2.second, row2.fourth},
            {row3.first, row3.second, row3.fourth},
            {row4.first, row4.second, row4.fourth}
        };
        out_mat.row3.first = (T) (inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.fourth},
            {row3.first, row3.second, row3.fourth},
            {row4.first, row4.second, row4.fourth}
        };
        out_mat.row3.second = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.fourth},
            {row2.first, row2.second, row2.fourth},
            {row4.first, row4.second, row4.fourth}
        };
        out_mat.row3.third = (T) (inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.fourth},
            {row2.first, row2.second, row2.fourth},
            {row3.first, row3.second, row3.fourth}
        };
        out_mat.row3.fourth = (T) (-inv_det * mat3<T>::det(adj_mat)); 

        // Row 4
        adj_mat = {
            {row2.first, row2.second, row2.third},
            {row3.first, row3.second, row3.third},
            {row4.first, row4.second, row4.third}
        };
        out_mat.row4.first = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.third},
            {row3.first, row3.second, row3.third},
            {row4.first, row4.second, row4.third}
        };
        out_mat.row4.second = (T) (inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.third},
            {row2.first, row2.second, row2.third},
            {row4.first, row4.second, row4.third}
        };
        out_mat.row4.third = (T) (-inv_det * mat3<T>::det(adj_mat));

        adj_mat = {
            {row1.first, row1.second, row1.third},
            {row2.first, row2.second, row2.third},
            {row3.first, row3.second, row3.third}
        };
        out_mat.row4.fourth = (T) (inv_det * mat3<T>::det(adj_mat)); 

        return out_mat;
    }


    /// Transpose the current matrix
    template<typename T>
    mat4<T> mat4<T>::transpose() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat4<T> new_mat;
            new_mat.row1 = vec4<T>{row1.first, row2.first, row3.first, row4.first};
            new_mat.row2 = vec4<T>{row1.second, row2.second, row3.second, row4.second};
            new_mat.row3 = vec4<T>{row1.third, row2.third, row3.third, row4.third};
            new_mat.row4 = vec4<T>{row1.fourth, row2.fourth, row3.fourth, row4.fourth};

            return new_mat;
        }

        return mat4<T>{};
    }


#ifdef __DEBUG
    template<typename T>
    void mat4<T>::log(const std::string &desc) {
        std::cout << "MAT_LOG: " <<  desc << std::endl;
        std::cout << row1.first << " | " << row1.second << " | " << row1.third << " | " << row1.fourth << std::endl;
        std::cout << row2.first << " | " << row2.second << " | " << row2.third << " | " << row2.fourth << std::endl;
        std::cout << row3.first << " | " << row3.second << " | " << row3.third << " | " << row3.fourth << std::endl;
        std::cout << row4.first << " | " << row4.second << " | " << row4.third << " | " << row4.fourth << "\n" << std::endl;
    }
#endif
}

#endif
