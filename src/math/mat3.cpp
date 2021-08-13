/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat3.cpp - DENG 3x3 matrix class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE!!!
#ifndef __MAT3_CPP
#define __MAT3_CPP


namespace dengMath {

    /// Add two matrices together
    template<typename T>
    mat3<T> mat3<T>::operator+(const mat3<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out;
            out.row1 = vec3<T>{row1.first + mat.row1.first, row1.second + mat.row1.second, row1.third + mat.row1.third};
            out.row2 = vec3<T>{row2.first + mat.row2.first, row2.second + mat.row2.second, row2.third + mat.row2.third};
            out.row3 = vec3<T>{row3.first + mat.row3.first, row3.second + mat.row3.second, row3.third + mat.row3.third};
            return out;
        }

        else return mat3<T>{};
    }


    /// Add constant to the current matrix
    template<typename T>
    mat3<T> mat3<T>::operator+(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out{};
            out.row1 = vec3<T>{row1.first + c, row1.second + c, row1.third + c};
            out.row2 = vec3<T>{row2.first + c, row2.second + c, row2.third + c};
            out.row3 = vec3<T>{row3.first + c, row3.second + c, row3.third + c};

            return out;
        }

        else return mat3<T>{};
    }


    /// Substract current matrix with given matrix
    template<typename T>
    mat3<T> mat3<T>::operator-(const mat3<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out{};
            out.row1 = vec3<T>{row1.first - mat.row1.first, row1.second - mat.row1.second, row1.third - mat.row1.third};
            out.row2 = vec3<T>{row2.first - mat.row2.first, row2.second - mat.row2.second, row2.second - mat.row2.third};
            out.row3 = vec3<T>{row3.first - mat.row3.first, row3.second - mat.row3.second, row3.second - mat.row3.third};

            return out;
        }

        else return mat3<T>{};
    }


    /// Substract a constant number from the current matrix
    template<typename T>
    mat3<T> mat3<T>::operator-(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out{};
            out.row1 = vec3<T>{row1.first - c, row1.second - c, row1.third - c};
            out.row2 = vec3<T>{row2.first - c, row2.second - c, row2.third - c};
            out.row3 = vec3<T>{row3.first - c, row3.second - c, row3.third - c};

            return out;
        }

        else return mat3<T>{};
    }


    /// Multiply all matrix members with a constant
    template<typename T>
    mat3<T> mat3<T>::operator*(const T &c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out;
            out.row1 = {row1.first * c, row1.second * c, row1.third * c};
            out.row2 = {row2.first * c, row2.second * c, row2.third * c};
            out.row3 = {row3.first * c, row3.second * c, row3.third * c};
            return out;
        }

        else return mat3<T>{};
    }


    /// Find the dot product of two matrices
    template<typename T>
    mat3<T> mat3<T>::operator*(const mat3<T> &matrix) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out_mat;
            out_mat.row1 = vec3<deng_vec_t>{
                (
                    (row1.first * matrix.row1.first) + 
                    (row1.second * matrix.row2.first) + 
                    (row1.third * matrix.row3.first)
                ), 
                (
                    (row1.first * matrix.row1.second) + 
                    (row1.second * matrix.row2.second) + 
                    (row1.third * matrix.row3.second)
                ), 
                (
                    (row1.first * matrix.row1.third) + 
                    (row1.second * matrix.row2.third) + 
                    (row1.third * matrix.row3.third)
                )
            };

            out_mat.row2 = vec3<deng_vec_t>{
                (
                    (row2.first * matrix.row1.first) + 
                    (row2.second * matrix.row2.first) + 
                    (row2.third * matrix.row3.first)
                ), 
                (
                    (row2.first * matrix.row1.second) + 
                    (row2.second * matrix.row2.second) + 
                    (row2.third * matrix.row3.second)
                ), 
                (
                    (row2.first * matrix.row1.third) + 
                    (row2.second * matrix.row2.third) + 
                    (row2.third * matrix.row3.third)
                )
            };

            out_mat.row3 = vec3<deng_vec_t>{
                (
                    (row3.first * matrix.row1.first) + 
                    (row3.second * matrix.row2.first) + 
                    (row3.third * matrix.row3.first)
                ), 
                (
                    (row3.first * matrix.row1.second) + 
                    (row3.second * matrix.row2.second) + 
                    (row3.third * matrix.row3.second)
                ), 
                (
                    (row3.first * matrix.row1.third) + 
                    (row3.second * matrix.row2.third) + 
                    (row3.third * matrix.row3.third)
                )
            };

            return out_mat;
        }
        
        else return mat3<T>{};
    }


    /// Find the dot product of current matrix and a vector
    template<typename T>
    vec3<T> mat3<T>::operator*(const vec3<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                (row1.first * vec.first + row1.second * vec.second + row1.third * vec.third),
                (row2.first * vec.first + row2.second * vec.second + row2.third * vec.third),
                (row3.first * vec.first + row3.second * vec.second + row3.third * vec.third)
            }; 

            return out;
        }

        else return vec3<T>{};
    }


    /// Divide all matrix elements with a constant
    template<typename T>
    mat3<T> mat3<T>::operator/(const T &c) { 
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> out;
            out.row1 = vec3<T>{row1.first / c, row1.second / c, row1.third / c};
            out.row2 = vec3<T>{row2.first / c, row2.second / c, row2.third / c};
            out.row3 = vec3<T>{row3.first / c, row3.second / c, row3.third / c};

            return out;
        }

        else return mat3<T>{};
    }


    /// Multiply all matrix members with a constant and set the product as the value of the current matrix instance
	template<typename T>
        void mat3<T>::operator*=(const T &c) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first *= c;
                row1.second *= c;
                row1.third *= c;

                row2.first *= c;
                row2.second *= c;
                row2.third *= c;

                row3.first *= c;
                row3.second *= c;
                row3.third *= c;
            }
        }


        /// Find the cross product of two matrices and set the current matrix instance value to it
        template<typename T>
        void mat3<T>::operator*=(const mat3<T> &mat) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                mat3<T> new_mat{};
                new_mat.row1 = {
                    (
                        (row1.first * mat.row1.first) + 
                        (row1.second * mat.row2.first) + 
                        (row1.third * mat.row3.first)
                    ), 
                    (
                        (row1.first * mat.row1.second) + 
                        (row1.second * mat.row2.second) + 
                        (row1.third * mat.row3.second)
                    ), 
                    (
                        (row1.first * mat.row1.third) + 
                        (row1.second * mat.row2.third) + 
                        (row1.third * mat.row3.third)
                    )
                };

                new_mat.row2 = {
                    (
                        (row2.first * mat.row1.first) + 
                        (row2.second * mat.row2.first) + 
                        (row2.third * mat.row3.first)
                    ), 
                    (
                        (row2.first * mat.row1.second) + 
                        (row2.second * mat.row2.second) + 
                        (row2.third * mat.row3.second)
                    ), 
                    (
                        (row2.first * mat.row1.third) + 
                        (row2.second * mat.row2.third) + 
                        (row2.third * mat.row3.third)
                    )
                };

                new_mat.row3 = {
                    (
                        (row3.first * mat.row1.first) + 
                        (row3.second * mat.row2.first) + 
                        (row3.third * mat.row3.first)
                    ), 
                    (
                        (row3.first * mat.row1.second) + 
                        (row3.second * mat.row2.second) + 
                        (row3.third * mat.row3.second)
                    ), 
                    (
                        (row3.first * mat.row1.third) + 
                        (row3.second * mat.row2.third) + 
                        (row3.third * mat.row3.third)
                    )
                };

                *this = new_mat;
            }
        }


        /// Add constant value to matrix and store the value in current matrix instance
        template<typename T>
        void mat3<T>::operator+=(const T &c) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first += c;
                row1.second += c;
                row1.third += c;

                row2.first += c;
                row2.second += c;
                row2.third += c;

                row3.first += c;
                row3.second += c;
                row3.third += c;
            }
        }


        /// Add two matrices together and store the value in current matrix instance
        template<typename T>
        void mat3<T>::operator+=(const mat3<T> &mat) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first += mat.row1.first;
                row1.second += mat.row1.second;
                row1.third += mat.row1.third;

                row2.first += mat.row2.first;
                row2.second += mat.row2.second;

                row3.first += mat.row3.first;
                row3.second += mat.row3.second;
                row3.third += mat.row3.third;
            }
        }


        /// Substract constant value from matrix and store the result in current matrix instance
        template<typename T>
        void mat3<T>::operator-=(const T &c) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first -= c;
                row1.second -= c;
                row1.third -= c;

                row2.first -= c;
                row2.second -= c;
                row2.third -= c;

                row3.first -= c;
                row3.second -= c;
                row3.third -= c;
            }
        }


        /// Substract a matrix from current matrix and store the result in current matrix instance
        template<typename T>
        void mat3<T>::operator-=(const mat3<T> &mat) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first -= mat.row1.first;
                row1.second -= mat.row1.second;
                row1.third -= mat.row1.third;

                row2.first -= mat.row2.first;
                row2.second -= mat.row2.second;
                row2.third -= mat.row2.third;

                row3.first -= mat.row3.first;
                row3.second -= mat.row3.second;
                row3.third -= mat.row3.third;
            }
        }


        /// Divide all matrix elements with constant and store the value in current matrix instance
        template<typename T>
        void mat3<T>::operator/=(const T &c) {
            if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
                row1.first /= c;
                row1.second /= c;
                row1.third /= c;

                row2.first /= c;
                row2.second /= c;
                row2.third /= c;

                row3.first /= c;
                row3.second /= c;
                row3.third /= c;
            }
        }
        
        
        /// Check if current and given matrix instances have equal values
        template<typename T>
        deng_bool_t mat3<T>::operator==(const mat3<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (deng_bool_t) (
                row1 == mat.row1 &&
                row2 == mat.row2 &&
                row3 == mat.row3
            );       
        }

        else return false;
    }


    /// Find the determinant of current matrix instance
    template<typename T>
    template<typename DT>
    DT mat3<T>::det(const mat3<DT> &mat) {
        return (DT) (
            (mat.row1.first * mat.row2.second * mat.row3.third) +
            (mat.row1.second * mat.row2.third * mat.row3.first) + 
            (mat.row1.third * mat.row2.first * mat.row3.second) -

            (mat.row1.third * mat.row2.second * mat.row3.first) -
            (mat.row1.first * mat.row2.third * mat.row3.second) -
            (mat.row1.second * mat.row2.first * mat.row3.third) 
        );
    }


    /// Find the inverse of the current matrix 
    template<typename T>
    mat3<T> mat3<T>::inv() {
        mat3<deng_vec_t> fl_mat;
        deng_vec_t inv_det = 1 / mat3<T>::det(*this);

        fl_mat.row1 = {
            inv_det * (row2.second * row3.third - row2.third * row3.second),
            inv_det * -(row1.second * row3.third - row1.third * row3.second),
            inv_det * (row1.second * row2.third - row1.third * row2.second) 
        };

        fl_mat.row2 = {
            inv_det * -(row2.first * row3.third - row2.third * row3.first),
            inv_det * (row1.first * row3.third - row1.third * row3.first),
            inv_det * -(row1.first * row2.third - row1.third * row2.first)
        };
        
        fl_mat.row3 = {
            inv_det * (row2.first * row3.second - row2.second * row3.first),
            inv_det * -(row1.first * row3.second - row1.second * row3.first),
            inv_det * (row1.first * row2.second - row1.second * row2.first)
        };

        mat3<T> out_mat;
        out_mat.row1 = {(T) fl_mat.row1.first, (T) fl_mat.row1.second, (T) fl_mat.row1.third}; 
        out_mat.row2 = {(T) fl_mat.row2.first, (T) fl_mat.row2.second, (T) fl_mat.row2.third};
        out_mat.row3 = {(T) fl_mat.row3.first, (T) fl_mat.row3.second, (T) fl_mat.row3.third};
        
        return out_mat;
    }


    /// Transpose the current matrix
    template<typename T>
    mat3<T> mat3<T>::transpose() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            mat3<T> new_mat{};
            new_mat.row1 = vec3<T>{row1.first, row2.first, row3.first};
            new_mat.row2 = vec3<T>{row1.second, row2.second, row3.second};
            new_mat.row3 = vec3<T>{row1.third, row2.third, row3.third};

            return new_mat;
        }

        else return mat3<T>{};
    }


#ifdef __DEBUG
    template<typename T>
    void mat3<T>::log(const std::string &desc) {
        std::cout << "MAT_LOG: " <<  desc << std::endl;
        std::cout << row1.first << " | " << row1.second << " | " << row1.third << std::endl;
        std::cout << row2.first << " | " << row2.second << " | " << row2.third << std::endl;
        std::cout << row3.first << " | " << row3.second << " | " << row3.third << "\n" << std::endl;
    }
#endif
}

#endif
