/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat2.cpp - DENG 2x2 matrix class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE INTO SEPERATE UNIT!!!
#ifndef __MAT2_CPP
#define __MAT2_CPP


namespace dengMath {

    template<typename T>
    mat2<T>::mat2() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1 = vec2<T>(1, 0);
            row2 = vec2<T>(0, 1);
        }
    }


    template<typename T>
    mat2<T>::mat2(const vec2<T> &r1, const vec2<T> &r2) {
        row1 = r1;
        row2 = r2;
    }


    template<typename T>
    mat2<T>::mat2(vec2<T> &&r1, vec2<T> &&r2) {
        row1 = std::move(r1);
        row2 = std::move(r2);
    }


    template<typename T>
    mat2<T>::mat2(const mat2<T> &val) {
        row1 = val.row1;
        row2 = val.row2;
    }


    template<typename T>
    mat2<T>::mat2(mat2<T> &&val) {
        row1 = std::move(val.row1);
        row2 = std::move(val.row2);
    }


    template<typename T>
    void mat2<T>::operator=(const mat2<T> &val) {
        row1 = val.row1;
        row2 = val.row2;
    }


    /// Add two matrices together
    template<typename T>
    mat2<T> mat2<T>::operator+(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out{}; 
            out.row1 = vec2<T>{row1.first + mat.row1.first, row1.second + mat.row1.second};
            out.row2 = vec2<T>{row2.first + mat.row2.first, row1.second + mat.row2.second};

            return out;
        }

        else return mat2<T>{};
    }


    /// Add constant to the current matrix
    template<typename T>
    mat2<T> mat2<T>::operator+(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out{};
            out.row1 = vec2<T>{row1.first + c, row1.second + c};
            out.row1 = vec2<T>{row2.first + c, row2.second + c};

            return out;
        }

        else return mat2<T>{};
    }


    /// Substract current matrix with given matrix
    template<typename T>
    mat2<T> mat2<T>::operator-(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out{};
            out.row1 = vec2<T>{row1.first - mat.row1.first, row1.second - mat.row1.second};
            out.row2 = vec2<T>{row2.first - mat.row2.first, row2.second - mat.row2.second};

            return out;
        }

        else return mat2<T>{};
    }


    /// Substract a constant number from the current matrix
    template<typename T>
    mat2<T> mat2<T>::operator-(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out{};
            out.row1 = vec2<T>{row1.first - c, row1.second - c};
            out.row2 = vec2<T>{row2.first - c, row2.second - c};

            return out;
        }

        else return mat2<T>{};
    }

    
    /// Multiply all matrix members with a constant
    template<typename T>
    mat2<T> mat2<T>::operator*(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out;
            out.row1 = {row1.first * c, row1.second * c};
            out.row2 = {row2.first * c, row2.second * c};

            return out; 
        }

        else return mat2<T>{};
    }


    /// Find the dot product of two matrices
    template<typename T>
    mat2<T> mat2<T>::operator*(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out_mat;
            out_mat.row1 = vec2<deng_vec_t>{
                (
                    (row1.first * mat.row1.first) +
                    (row1.second * mat.row2.first)
                ),
                (
                    (row1.first * mat.row1.second) +
                    (row1.second * mat.row2.second)
                )
            }; 

            out_mat.row2 = vec2<deng_vec_t>{
                (
                    (row2.first * mat.row1.first) +
                    (row2.second * mat.row2.first)
                ),
                (
                    (row2.first * mat.row1.second) +
                    (row2.second * mat.row2.second)
                )
            };

            return out_mat;
        }

        else return mat2<T>{};
    }


    /// Find the dot product of current matrix and a vector
    template<typename T>
    vec2<T> mat2<T>::operator*(const vec2<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = vec2<T>{
                (row1.first * vec.first + row1.second * vec.second),
                (row2.first * vec.first + row2.second * vec.second)
            };

            return out; 
        }

        else return vec2<T>{};
    }


    /// Divide all matrix elements with constant
    template<typename T>
    mat2<T> mat2<T>::operator/(const T &c) { 
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> out;
            out.row1 = vec2<T>{row1.first / c, row1.second / c};
            out.row2 = vec2<T>{row2.first / c, row2.second / c};

            return out;
        }

        else return mat2<T>{};
    }


    /// Multiply all matrix members with a constant and
    /// set the product as the value of the current matrix instance
	template<typename T>
    void mat2<T>::operator*=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first *= c;
            row1.second *= c;
            row2.first *= c;
            row2.second *= c;
        }
    }


    /// Find the cross product of two matrices and set the current matrix
    /// instance value to it
	template<typename T>
    void mat2<T>::operator*=(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> new_mat{};
            new_mat.row1 = vec2<deng_vec_t>{
                (
                    (row1.first * mat.row1.first) +
                    (row1.second * mat.row2.first)
                ),
                (
                    (row1.first * mat.row1.second) +
                    (row1.second * mat.row2.second)
                )
            }; 

            new_mat.row2 = vec2<deng_vec_t>{
                (
                    (row2.first * mat.row1.first) +
                    (row2.second * mat.row2.first)
                ),
                (
                    (row2.first * mat.row1.second) +
                    (row2.second * mat.row2.second)
                )
            };

            *this = new_mat;
        }
    }


    /// Add constant value to matrix and store the value in current matrix instance
	template<typename T>
    void mat2<T>::operator+=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first += c;
            row1.second += c;
            row2.first += c;
            row2.second += c;
        }
    }


    /// Add two matrices together and store the value in current matrix instance
	template<typename T>
    void mat2<T>::operator+=(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first += mat.row1.first;
            row1.second += mat.row1.second;
            row2.first += mat.row2.first;
            row2.second += mat.row2.second;
        }
    }


    /// Substract constant value from matrix and store the result in current matrix instance
	template<typename T>
    void mat2<T>::operator-=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first -= c;
            row1.second -= c;
            row2.first -= c;
            row2.second -= c;
        }
    }


    /// Substract a matrix from current matrix and store the result in current matrix instance
	template<typename T>
    void mat2<T>::operator-=(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first -= mat.row1.first;
            row1.second -= mat.row1.second;
            row2.first -= mat.row2.first;
            row2.second -= mat.row2.second;
        }
    }


    /// Divide all matrix elements with constant and store the value in current matrix instance
	template<typename T>
    void mat2<T>::operator/=(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            row1.first /= c;
            row1.second /= c;
            row2.first /= c;
            row2.second /= c;
        }
    }
    
    
    /// Check if current and given matrix instances have equal values
	template<typename T>
    deng_bool_t mat2<T>::operator==(const mat2<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (deng_bool_t) (
                row1 == mat.row1 &&
                row2 == mat.row2
            );       
        }

        else return false;
    }


    /// Find the determinant of current matrix instance
    template<typename T>
    template<typename DT>
    DT mat2<T>::det(const mat2<DT> &mat) { 
        return mat.row1.first * mat.row2.second - mat.row1.second * mat.row2.first; 
    }


    /// Find the inverse of the current matrix 
    template<typename T>
    mat2<T> mat2<T>::inv() {
        mat2<deng_vec_t> fl_mat;
        deng_vec_t inv_det = 1 / mat2<T>::det(*this);
        fl_mat.row1.first = row2.second * inv_det;
        fl_mat.row1.second = -row2.first * inv_det;
        fl_mat.row2.first = -row1.second * inv_det;
        fl_mat.row2.second = row1.first * inv_det;

        mat2<T> out_mat;
        out_mat.row1 = {(T) fl_mat.row1.first, (T) fl_mat.row1.second};
        out_mat.row2 = {(T) fl_mat.row2.first, (T) fl_mat.row2.second};

        return out_mat;
    }


    /// Transpose the current matrix
    template<typename T>
    mat2<T> mat2<T>::transpose() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat2<T> new_mat{};
            new_mat.row1 = vec2<T>{row1.first, row2.first};
            new_mat.row2 = vec2<T>{row1.second, row2.second};

            return new_mat;
        }

        else return mat2<T>{};
    }


#ifdef __DEBUG
    template<typename T>
    void mat2<T>::log(const std::string &desc) {
        std::cout << "MAT_LOG: " <<  desc << std::endl;
        std::cout << row1.first << " | " << row1.second << std::endl;
        std::cout << row2.first << " | " << row2.second << "\n" << std::endl;
    }
#endif
}

#endif
