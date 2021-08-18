/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec2.h - DENG 2 element vector class header
/// author: Karl-Mihkel Ott


/* 
 * Vector structure requirements are following:
 * mat<T>();
 * mat<T>(T &&r1, ...);
 * mat<T>(mat<T> &val);
 * mat<T>(mat<T> &&val);
 * OPERATORS: 
 *  vec<T> + vec<T>
 *  vec<T> + T
 *  vec<T> - vec<T>
 *  vec<T> - T
 *  vec<T> * T
 *  T * vec<T> (calculate vector dot product)
 *  vec<T> / T 
 *  void *= T
 *  void *= matn<T>
 *  void +=  vec<T>
 *  void += T
 *  void -=  vec<T>
 *  void -= T
 *  void /= T
 *  bool ==  vec<T>
 *
 *  OPTIONAL: 
 *    void = <VERT_DATA>
 *    bool == <VERT_DATA>
 * 
 * MEMBER METHODS:
 *  T length()
 *  void norm()
 *  static void cross(vec<T>, vec<T>) (find the cross product of two vectors)
 */


#ifndef __VEC2_H
#define __VEC2_H


namespace dengMath {

    template<typename T> struct mat2;

    template<typename T>
    struct vec2 {
        vec2();
        vec2(const T &r1, const T &r2);
        vec2(T &&r1, T &&r2);
        vec2(const vec2<T> &val);
        vec2(vec2<T>&& val);
        void operator=(const vec2<T> &val);

        T first, second;

        /******************************/
        /***** Operator overloads *****/
        /******************************/

        const vec2<T> operator+(const vec2<T> &vec) const;
        const vec2<T> operator+(const T c) const;
        const vec2<T> operator-(const vec2<T> &vec) const;
        const vec2<T> operator-(const T c) const;
        const vec2<T> operator*(const T c) const;
        const T operator*(const vec2<T> &vec) const;
        const vec2<T> operator/(const T c) const;
        void operator*=(const T c);
        void operator*=(const mat2<T> &m);
        void operator+=(const vec2<T> &vec);
        void operator+=(const T c);
        void operator-=(const vec2<T> &vec);
        void operator-=(const T c);
        void operator/=(const T c);
        const deng_bool_t operator==(const vec2<T> &vec) const;


        /// C vertices structure assignment operators
        void operator=(const das_ObjTextureData tex);
        void operator=(const das_ObjPosData2D pos);

        
        /// C vertices structure comparison operators
        const deng_bool_t operator==(const das_ObjTextureData tex) const;
        const deng_bool_t operator==(const das_ObjPosData2D pos) const;

        const T length() const;
        void norm();
    };
}


#include <../src/math/vec2.cpp>
#endif
