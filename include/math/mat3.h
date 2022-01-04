/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: mat3.h - DENG 3x3 matrix class header
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

#ifndef __MAT3_H
#define __MAT3_H


namespace dengMath {
    template<typename T>
    struct mat3 {
        mat3();
        mat3(const vec3<T> &r1, const vec3<T> &r2, const vec3<T> &r3);
        mat3(vec3<T> &&r1, vec3<T> &&r2, vec3<T> &&r3);
        mat3(const mat3<T> &val);
        mat3(mat3<T>&& val);
        void operator=(const mat3<T> &val);


        vec3<T> row1, row2, row3;
        
        
        /******************************/
        /***** Operator overloads *****/
        /******************************/

        mat3<T> operator+(const mat3<T> &mat); 
        mat3<T> operator+(const T &c);
        mat3<T> operator-(const mat3<T> &mat);
        mat3<T> operator-(const T &c);
        mat3<T> operator*(const T &c); 
        mat3<T> operator*(const mat3<T> &mat); 
        vec3<T> operator*(const vec3<T> &vec); 
        mat3<T> operator/(const T &c);
        void operator*=(const T &c);
        void operator*=(const mat3<T> &mat);
        void operator+=(const T &c);
        void operator+=(const mat3<T> &mat);
        void operator-=(const T &c);
        void operator-=(const mat3<T> &mat);
        void operator/=(const T &c);
        deng_bool_t operator==(const mat3<T> &mat);


        /// Find the determinant of current matrix instance
        template<typename DT>
        static DT det(const mat3<DT> &mat);

        
        /// Find the inverse of the current matrix 
        mat3<T> inv(); 


        /// Transpose the current matrix
        mat3<T> transpose();


#ifdef __DEBUG
        void log(const std::string &desc);
#endif
    };
}


#ifdef __DENG_LIB_EXPORT
    #include <../src/math/mat3.cpp>
#else 
    #include <math/mat3.cpp>
#endif
#endif
