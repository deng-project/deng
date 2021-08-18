/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec3.cpp - DENG 3 element vector class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE!!!
#ifndef __VEC3_CPP
#define __VEC3_CPP


namespace dengMath {

    /*********************************************/
    /********* vec3 operators and methods ********/
    /*********************************************/

    template<typename T>
    vec3<T>::vec3() {
        memset(&first, 0 , sizeof(T));
        memset(&second, 0 , sizeof(T));
        memset(&third, 0 , sizeof(T));
    }


    template<typename T>
    vec3<T>::vec3(const T &r1, const T &r2, const T &r3) {
        first = r1;
        second = r2;
        third = r3;
    }


    template<typename T>
    vec3<T>::vec3(T &&r1, T &&r2, T &&r3) {
        first = std::move(r1);
        second = std::move(r2);
        third = std::move(r3);
    }


    template<typename T>
    vec3<T>::vec3(const vec3<T> &val) {
        first = val.first;
        second = val.second;
        third = val.third;
    }


    template<typename T>
    vec3<T>::vec3(vec3<T> &&val) {
        first = std::move(val.first);
        second = std::move(val.second);
        third = std::move(val.third);
    }


    template<typename T>
    void vec3<T>::operator=(const vec3<T> &val) {
        first = val.first;
        second = val.second;
        third = val.third;
    }


    template<typename T>
    const vec3<T> vec3<T>::operator+(const vec3<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                first + vec.first,
                second + vec.second,
                third + vec.third
            };
            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    const vec3<T> vec3<T>::operator+(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                first + c,
                second + c,
                third + c 
            };

            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    const vec3<T> vec3<T>::operator-(const vec3<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                first - vec.first,
                second - vec.second,
                third - vec.third
            };
            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    const vec3<T> vec3<T>::operator-(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                first - c,
                second - c,
                third - c 
            };
            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    const T vec3<T>::operator*(const vec3<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T out = static_cast<T>(
                first * vec.first +
                second * vec.second +
                third * vec.third
            );

            return out; 
        }

        return T{};
    }


    template<typename T>
    const vec3<T> vec3<T>::operator*(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                c * first,
                c * second,
                c * third,
            };
            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    const vec3<T> vec3<T>::operator/(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = {
                first / c,
                second / c,
                third / c,
            };
            return out; 
        }

        return vec3<T>{};
    }


    template<typename T>
    void vec3<T>::operator*=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first *= c;
            second *= c;
            third *= c;
        }
    }


    template<typename T>
    void vec3<T>::operator*=(const mat3<T> &mat) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value)
            *this = mat * (*this);
    }


    template<typename T>
    void vec3<T>::operator+=(const vec3<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += vec.first;
            second += vec.second;
            third += vec.third;
        }
    }


    template<typename T>
    void vec3<T>::operator+=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += c;
            second += c;
            third += c;
        }
    }

    
    template<typename T>
    void vec3<T>::operator-=(const vec3<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= vec.first;
            second -= vec.second;
            third -= vec.third;
        }
    }


    template<typename T>
    void vec3<T>::operator-=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= c;
            second -= c;
            third -= c;
        }
    }


    template<typename T>
    void vec3<T>::operator/=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first /= c;
            second /= c;
            third /= c;
        }
    }


    template<typename T>
    const deng_bool_t vec3<T>::operator==(const vec3<T> &vec) const {
        return static_cast<deng_bool_t>(
            first == vec.first && 
            second == vec.second && 
            third == vec.third
        ); 
    }


    template<typename T>
    void vec3<T>::operator=(const das_ObjPosData &pos) {
        if(std::is_floating_point<T>::value::value) {
            first = pos.vert_x;
            second = pos.vert_y;
            third = pos.vert_z;
        }
    }


    template<typename T>
    void vec3<T>::operator=(const das_ObjPosData2D pos) {
        if(std::is_floating_point<T>::value) {
            first = pos.vert_x;
            second = pos.vert_y;
            third = 1.0f;
        }
    }


    template<typename T>
    void vec3<T>::operator=(const das_ObjTextureData tex) {
        if(std::is_floating_point<T>::value) {
            first = tex.tex_x;
            second = tex.tex_y;
            third = 1.0f;
        }
    }


    template<typename T>
    void vec3<T>::operator=(const das_ObjNormalData &nor) {
        if(std::is_floating_point<T>::value::value) {
            first = nor.nor_x;
            second = nor.nor_y;
            third = nor.nor_z;
        }
    }


    template<typename T>
    void vec3<T>::operator=(const vec2<T> &vec) {
        first = vec.first;
        second = vec.second;
    }


    template<typename T>
    const deng_bool_t vec3<T>::operator==(const das_ObjPosData &vert) const {
        if(std::is_floating_point<T>::value) {
            return static_cast<deng_bool_t>(
                first == vert.vert_x &&
                second == vert.vert_y &&
                third == vert.vert_z
            );
        }

        return false;
    }


    template<typename T>
    const deng_bool_t vec3<T>::operator==(const das_ObjPosData2D vert) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return static_cast<deng_bool_t>(
                first == vert.vert_x &&
                second == vert.vert_y
            );
        }

        return false;
    }


    template<typename T>
    const deng_bool_t vec3<T>::operator==(const das_ObjTextureData tex) const {
        if(std::is_floating_point<T>::value) {
            return static_cast<deng_bool_t>(
                first == tex.tex_x &&
                second == tex.tex_y
            );
        }

        return false;
    }
    

    template<typename T>
    const deng_bool_t vec3<T>::operator==(const das_ObjNormalData &norm) const {
        if(std::is_floating_point<T>::value) {
            return static_cast<deng_bool_t>(
                first == norm.nor_x &&
                second == norm.nor_y &&
                third == norm.nor_z
            );
        }

        return false;
    }


    template<typename T>
    const deng_bool_t vec3<T>::operator==(const vec2<T> &vec) const {
        return static_cast<deng_bool_t>(first == vec.first && second == vec.second);
    }
    

    template<typename T>
    const T vec3<T>::length() const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (T) sqrt (
                first * first +
                second * second +
                third * third
            );
        }

        return T{};
    }


    template<typename T>
    void vec3<T>::norm() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T len = length();
            first /= len;
            second /= len;
            third /= len;
        }
    };


    template<typename T>
    const vec3<T> vec3<T>::cross(const vec3<T> &vec1, const vec3<T> &vec2) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec3<T> out = vec3<T>{
                vec1.second * vec2.third -
                vec1.third * vec2.second,
                vec1.third * vec2.first -
                vec1.first * vec2.third,
                vec1.first * vec2.second -
                vec1.second * vec2.first
            };

            return out;
        }

        return vec3<T>{};
    }
}


#endif
