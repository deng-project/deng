/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec4.h - DENG vector class with 4 elements capacity header
/// author: Karl-Mihkel Ott

/* 
 * Vector structure requirements are following:
 * EXPLICIT CONSTRUCTOR
 * OPERATORS: 
 *  vec<T> + vec<T>
 *  vec<T> + T
 *  vec<T> - vec<T>
 *  vec<T> - T
 *  vec<T> * T
 *  T * vec<T> (calculate vector dot product)
 *  vec<T> / T 
 *  void *= T
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

#ifndef __VEC4_H
#define __VEC4_H


namespace dengMath {
    template<typename T> struct mat4;
    template<typename T>
    struct vec4 {
        vec4();
        vec4(const T &r1, const T &r2, const T &r3, const T &r4);
        vec4(T &&r1, T &&r2, T &&r3, T &&r4);
        vec4(const vec4<T> &val);
        vec4(vec4<T>&& val);
        void operator=(const vec4<T> &val);

        T first, second, third, fourth;

        /******************************/
        /***** Operator overloads *****/
        /******************************/

        const vec4<T> operator+(const vec4<T> &vec) const; 
        const vec4<T> operator+(const T c) const; 
        const vec4<T> operator-(const vec4<T> &vec) const; 
        const vec4<T> operator-(const T c) const; 
        const T operator*(const vec4<T> &vec) const; 
        const vec4<T> operator*(const T c) const; 
        const vec4<T> operator/(const T c) const; 
        void operator*=(const T c); 
        void operator*=(const mat4<T> &m);
        void operator+=(const vec4<T> &vec); 
        void operator+=(const T c); 
        void operator-=(const vec4<T> &vec); 
        void operator-=(const T c); 
        void operator/=(const T c); 
        const deng_bool_t operator==(const vec4<T> &vec) const; 


        // Special structure assignment operators
        void operator=(const das_ObjPosData &vert); 
        void operator=(const das_ObjPosData2D vert); 
        void operator=(const das_ObjColorData &color); 
        void operator=(const das_ObjNormalData &nor); 
        void operator=(const vec2<T> &vec);
        void operator=(const vec3<T> &vec);


        /// C vertices structure comparison operators
        const deng_bool_t operator==(const das_ObjPosData &vert) const;
        const deng_bool_t operator==(const das_ObjPosData2D vert) const;
        const deng_bool_t operator==(const das_ObjColorData &color) const;
        const deng_bool_t operator==(const das_ObjNormalData &norm) const;
        const deng_bool_t operator==(const vec2<T> &vec) const;
        const deng_bool_t operator==(const vec3<T> &vec) const;


        /// Get the current length of the vector
        const T length() const;

        
        /// Normalise the vector to length 1
        void norm();

        
        /// Find the crossproduct of two vectors
        /// PS! Only first three axes are used
        static const vec4<T> cross(const vec4<T> &vec1, const vec4<T> &vec2); 
    };
}

#ifdef __DENG_LIB_EXPORT
    #include <../src/math/vec4.cpp>
#else 
    #include <math/vec4.cpp>
#endif

#endif
