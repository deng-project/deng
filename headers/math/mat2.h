/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat2.h - DENG 2x2 matrix class header
/// author: Karl-Mihkel Ott


/*
 * Matrix structure requirements are follwing
 * mat<T>();
 * mat<T>(const vec<T> &r1, ...);
 * mat<T>(mat<T> &val);
 * mat<T>(mat<T> &&val);
 * OPERATORS:
 *   mat<T> + mat<T>
 *   mat<T> + T
 *   mat<T> - mat<T>
 *   mat<T> - T
 *   mat<T> * T 
 *   mat<T> * mat<T> (matrix dot product)
 *   vec<T> * vec<T> (vector and matrix dot product)
 *   mat<T> / T
 *   void *= T
 *   void *= mat<T>
 *   void += T
 *   void += mat<T>
 *   void -= T
 *   void -= mat<T>
 *   void /= T
 *   bool == mat<T>
 *
 * MEMBER METHODS:
 *   static DT det(const mat<DT> &mat) (find the matrix determinant)
 *   mat<T> inv() (find the inverse of the current matrix)
 *   mat<T> transpose() (transpose the current matrix)
 *   void log() (log the matrix data into console)
 */


#ifndef __MAT2_H
#define __MAT2_H


namespace dengMath {
    template<typename T>
    struct mat2 {
        mat2();
        mat2(const vec2<T> &r1, const vec2<T> &r2);
        mat2(vec2<T> &&r1, vec2<T> &&r2);
        mat2(const mat2<T> &val);
        mat2(mat2<T> &&val);
        void operator=(const mat2<T> &val);

        vec2<T> row1, row2;
    
        /******************************/
        /***** Operator overloads *****/
        /******************************/

        mat2<T> operator+(const mat2<T> &mat); 
        mat2<T> operator+(const T &c);
        mat2<T> operator-(const mat2<T> &mat);
        mat2<T> operator-(const T &c);
        mat2<T> operator*(const T &c); 
        mat2<T> operator*(const mat2<T> &mat); 
        vec2<T> operator*(const vec2<T> &vec); 
        mat2<T> operator/(const T &c);
        void operator*=(const T &c);
        void operator*=(const mat2<T> &mat);
        void operator+=(const T &c);
        void operator+=(const mat2<T> &mat);
        void operator-=(const T &c);
        void operator-=(const mat2<T> &mat);
        void operator/=(const T &c);
        deng_bool_t operator==(const mat2<T> &mat);


        /// Find the determinant of current matrix instance
        template<typename DT>
        static DT det(const mat2<DT> &mat);

        
        /// Find the inverse of the current matrix 
        mat2<T> inv(); 


        /// Transpose the current matrix
        mat2<T> transpose();


#ifdef __DEBUG
        /// Log matrix into console output and add description to it
        void log(const std::string &desc);
#endif
    };
}

#include <../src/math/mat2.cpp>
#endif
