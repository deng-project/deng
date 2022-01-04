/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec3.h - DENG 3 element vector class header
/// author: Karl-Mihkel Ott


/* 
 * Vector structure requirements are following:
 * vec<T>();
 * vec<T>(T &&r1, ...);
 * vec<T>(vec<T> &val);
 * vec<T>(vec<T> &&val);
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


#ifndef __VEC3_H
#define __VEC3_H


namespace dengMath {
    template<typename T> struct mat3;

    template<typename T>
    struct vec3 {
        vec3();
        vec3(const T &r1, const T &r2, const T &r3);
        vec3(T &&r1, T &&r2, T &&r3);
        vec3(const vec3<T> &val);
        vec3(vec3<T>&& val);
        void operator=(const vec3<T> &val);
        //void operator=(vec3<T> &&val);

        T first, second, third;
        
        /******************************/
        /***** Operator overloads *****/
        /******************************/

        const vec3<T> operator+(const vec3<T> &vec) const;
        const vec3<T> operator+(const T c) const;
        const vec3<T> operator-(const vec3<T> &vec) const;
        const vec3<T> operator-(const T c) const;
        const T operator*(const vec3<T> &vec) const;
        const vec3<T> operator*(const T c) const;
        const vec3<T> operator/(const T c) const;
        void operator*=(const T c);
        void operator*=(const mat3<T> &m);
        void operator+=(const vec3<T> &vec);
        void operator+=(const T c);
        void operator-=(const vec3<T> &vec);
        void operator-=(const T c);
        void operator/=(const T c);
        const deng_bool_t operator==(const vec3<T> &vec) const;


        /// C vertices structure assignment operators
        void operator=(const das_ObjPosData &vert);
        void operator=(const das_ObjPosData2D vert);
        void operator=(const das_ObjTextureData tex);
        void operator=(const das_ObjNormalData &nor);
        void operator=(const vec2<T> &vec);


        /// C vertices structure comparisson operators
        const deng_bool_t operator==(const das_ObjPosData &vert) const;
        const deng_bool_t operator==(const das_ObjPosData2D vert) const;
        const deng_bool_t operator==(const das_ObjTextureData tex) const;
        const deng_bool_t operator==(const das_ObjNormalData &nor) const;
        const deng_bool_t operator==(const vec2<T> &vec) const;


        /// Get the current length of the vector
        const T length() const;


        /// Normalise the vector to length 1
        void norm();


        /// Find the crossproduct of two vectors
        static const vec3<T> cross(const vec3<T> &vec1, const vec3<T> &vec2);
    };
}

#ifdef __DENG_LIB_EXPORT
    #include <../src/math/vec3.cpp>
#else
    #include <math/vec3.cpp>
#endif

#endif
