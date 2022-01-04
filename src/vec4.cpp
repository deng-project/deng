/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec4.cpp - DENG 4 element vector class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE SEPARATELY!!!
#ifndef __VEC4_CPP
#define __VEC4_CPP


namespace dengMath {

    template<typename T>
    vec4<T>::vec4() {
        memset(&first, 0, sizeof(T));
        memset(&second, 0, sizeof(T));
        memset(&third, 0, sizeof(T));
        memset(&fourth, 0, sizeof(T));
    }


    template<typename T>
    vec4<T>::vec4(const T &r1, const T &r2, const T &r3, const T &r4) {
        first = r1;
        second = r2;
        third = r3;
        fourth = r4;
    }


    template<typename T>
    vec4<T>::vec4(T &&r1, T &&r2, T &&r3, T &&r4) {
        first = std::move(r1);
        second = std::move(r2);
        third = std::move(r3);
        fourth = std::move(r4);
    }


    template<typename T>
    vec4<T>::vec4(const vec4<T> &val) {
        first = val.first;
        second = val.second;
        third = val.third;
        fourth = val.fourth;
    }


    template<typename T>
    vec4<T>::vec4(vec4<T> &&val) {
        first = std::move(val.first);
        second = std::move(val.second);
        third = std::move(val.third);
        fourth = std::move(val.fourth);
    }


    template<typename T>
    void vec4<T>::operator=(const vec4<T> &val) {
        first = val.first;
        second = val.second;
        third = val.third;
        fourth = val.fourth;
    }


    template<typename T>
    const vec4<T> vec4<T>::operator+(const vec4<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first + vec.first,
                second + vec.second,
                third + vec.third
            };
            return out; 
        }

        else return vec4<T>{};
    }


    template<typename T>
    const vec4<T> vec4<T>::operator+(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first + c,
                second + c,
                third + c,
                fourth + c
            };

            return out;
        }

        else return vec4<T>{};
    }


    template<typename T>
    const vec4<T> vec4<T>::operator-(const vec4<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first - vec.first,
                second - vec.second,
                third - vec.third,
                fourth - vec.fourth
            };

            return out;
        }

        else return vec4<T>{};
    }


    template<typename T>
    const vec4<T> vec4<T>::operator-(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first - c,
                second - c,
                third - c,
                fourth - c
            };

            return out;
        }

        else return vec4<T>{};
    }


    template<typename T>
    const T vec4<T>::operator*(const vec4<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T out = (first * vec.first + second * vec.second + third * vec.third + fourth * vec.fourth);
            return out; 
        }

        else return T{};
    }


    template<typename T>
    const vec4<T> vec4<T>::operator*(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first * c,
                second * c,
                third * c,
                fourth * c
            };

            return out; 
        }

        else return vec4<T>{};
    }


    template<typename T>
    const vec4<T> vec4<T>::operator/(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = {
                first / c,
                second / c,
                third / c,
                fourth / c
            };

            return out; 
        }

        else return vec4<T>{};
    }


    template<typename T>
    void vec4<T>::operator*=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first *= c;
            second *= c;
            third *= c;
            fourth *= c;
        }
    }


    template<typename T>
    void vec4<T>::operator*=(const mat4<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value)
            *this = mat * (*this);
    }


    template<typename T>
    void vec4<T>::operator+=(const vec4<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += vec.first;
            second += vec.second;
            third += vec.third;
            fourth += vec.fourth;
        }
    }


    template<typename T>
    void vec4<T>::operator+=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += c;
            second += c;
            third += c;
            fourth += c;
        }
    }


    template<typename T>
    void vec4<T>::operator-=(const vec4<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= vec.first;
            second -= vec.second;
            third -= vec.third;
            fourth -= vec.fourth;
        }
    }


    template<typename T>
    void vec4<T>::operator-=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= c;
            second -= c;
            third -= c;
            fourth -= c;
        }
    }


    template<typename T>
    void vec4<T>::operator/=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first /= c;
            second /= c;
            third /= c;
            fourth /= c;
        }
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const vec4<T> &vec) const { 
        return (deng_bool_t) (                
            first == vec.first && 
            second == vec.second && 
            third == vec.third && 
            fourth == vec.fourth
        ); 
    }


    template<typename T>
    void vec4<T>::operator=(const das_ObjPosData &vert) {
        if(std::is_floating_point<T>::value) {
            first = vert.vert_x;
            second = vert.vert_y;
            third = vert.vert_z;
            fourth = 1.0f;
        }
    }


    template<typename T>
    void vec4<T>::operator=(const das_ObjPosData2D vert) {
        if(std::is_floating_point<T>::value) {
            first = vert.vert_x;
            second = vert.vert_y;
            third = 0.0f;
            fourth = 0.0f;
        }
    }


    template<typename T>
    void vec4<T>::operator=(const das_ObjNormalData &nor) {
        if(std::is_floating_point<T>::value) {
            first = nor.nor_x;
            second = nor.nor_y;
            third = nor.nor_z;
            fourth = 1.0f;
        }
    }


    template<typename T>
    void vec4<T>::operator=(const das_ObjColorData &color) {
        if(std::is_floating_point<T>::value) {
            first = color.col_r;
            second = color.col_g;
            third = color.col_b;
            fourth = color.col_a;
        }
    }


    template<typename T>
    void vec4<T>::operator=(const vec2<T> &vec) {
        first = vec.first;
        second = vec.second;
    }


    template<typename T>
    void vec4<T>::operator=(const vec3<T> &vec) {
        first = vec.first;
        second = vec.second;
        third = vec.third;
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const das_ObjPosData &vert) const {
        if(std::is_floating_point<T>::value)
            return static_cast<deng_bool_t>(first == vert.vert_x && second == vert.vert_y && third == vert.vert_z);
        else return false;
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const das_ObjPosData2D vert) const {
        if(std::is_floating_point<T>::value)
            return static_cast<deng_bool_t>(first = vert.vert_x && second == vert.vert_y);
        else return false;
    }



    template<typename T>
    const deng_bool_t vec4<T>::operator==(const das_ObjColorData &color) const {
        if(std::is_floating_point<T>::value)
            return static_cast<deng_bool_t>(first == color.col_r && second == color.col_g && 
                third == color.col_b && fourth == color.col_a);
        else return false;
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const das_ObjNormalData &norm) const {
        if(std::is_floating_point<T>::value) 
            return static_cast<deng_bool_t>(first == norm.nor_x && second == norm.nor_y && 
                third == norm.nor_z);
        else return false;
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const vec3<T> &vec) const {
        return static_cast<deng_bool_t>(first == vec.first && second == vec.second &&
            third == vec.third);
    }


    template<typename T>
    const deng_bool_t vec4<T>::operator==(const vec2<T> &vec) const {
        return static_cast<deng_bool_t>(first == vec.first && second == vec.second);
    }
        
    
    template<typename T>
    const T vec4<T>::length() const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (T) sqrt (
                first * first +
                second * second +
                third * third +
                fourth * fourth
            );
        }

        else return T{};
    }


    template<typename T>
    void vec4<T>::norm() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T len = length();
            first /= len;
            second /= len;
            third /= len;
            fourth /= len;
        }
    }


    template<typename T>
    const vec4<T> vec4<T>::cross(const vec4<T> &vec1, const vec4<T> &vec2) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec4<T> out = (vec4<T>) {
                vec1.second * vec2.third -
                vec1.third * vec2.second,
                vec1.third * vec2.first -
                vec1.first * vec2.third,
                vec1.first * vec2.second -
                vec1.second * vec2.first,
                0.0f
            };

            return out;
        }

        else return vec4<T>{};
    }
}

#endif
