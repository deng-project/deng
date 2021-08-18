/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat4.h - DENG 4x4 matrix class header
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
 */


#ifndef __MAT4_H
#define __MAT4_H


namespace dengMath {

    template<typename T>
    struct mat4 {
        mat4();
        mat4(const vec4<T> &r1, const vec4<T> &r2, const vec4<T> &r3, const vec4<T> &r4);
        mat4(vec4<T> &&r1, vec4<T> &&r2, vec4<T> &&r3, vec4<T> &&r4);
        mat4(const mat4<T> &val);
        mat4(mat4<T>&& val);
        void operator=(const mat4<T> &val);

        vec4<T> row1, row2, row3, row4; 


        /******************************/
        /***** Operator overloads *****/
        /******************************/

        mat4<T> operator+(const mat4<T> &mat); 
        mat4<T> operator+(const T &c);
        mat4<T> operator-(const mat4<T> &mat);
        mat4<T> operator-(const T &c);
        mat4<T> operator*(const T &c); 
        mat4<T> operator*(const mat4<T> &mat); 
        vec4<T> operator*(const vec4<T> &vec); 
        mat4<T> operator/(const T &c);
        void operator*=(const T &c);
        void operator*=(const mat4<T> &mat);
        void operator+=(const T &c);
        void operator+=(const mat4<T> &mat);
        void operator-=(const T &c);
        void operator-=(const mat4<T> &mat);
        void operator/=(const T &c);
        deng_bool_t operator==(const mat4<T> &mat);


        /// Find the determinant of current matrix instance
        template<typename DT>
        static DT det(const mat4<DT> &mat);

        
        /// Find the inverse of the current matrix 
        mat4<T> inv(); 


        /// Transpose the current matrix
        mat4<T> transpose();


#ifdef __DEBUG
        /// Log matrix into console output and add description to it
        void log(const std::string &desc);
#endif
    };
}


#include <../src/math/mat4.cpp>
#endif

