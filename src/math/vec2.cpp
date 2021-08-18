/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: vec2.cpp - DENG 2 element vector class implementation
/// author: Karl-Mihkel Ott


// DO NOT COMPILE THIS FILE!!!
#ifndef __VEC2_CPP
#define __VEC2_CPP

namespace dengMath {

    template<typename T>
    vec2<T>::vec2() {
        memset(&first, 0, sizeof(T));
        memset(&second, 0, sizeof(T));
    }

    
    template<typename T>
    vec2<T>::vec2(const T &r1, const T &r2) {
        first = r1;
        second = r2;
    }


    template<typename T>
    vec2<T>::vec2(T &&r1, T &&r2) {
        first = std::move(r1);
        second = std::move(r2);
    }


    template<typename T>
    vec2<T>::vec2(const vec2<T> &val) {
        first = val.first;
        second = val.second;
    }


    template<typename T>
    vec2<T>::vec2(vec2<T> &&val) {
        first = std::move(val.first);
        second = std::move(val.second);
    }


    template<typename T>
    void vec2<T>::operator=(const vec2<T> &val) {
        first = val.first;
        second = val.second;
    }


    template<typename T>
    const vec2<T> vec2<T>::operator+(const vec2<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = { first + vec.first, second + vec.second };
            return out; 
        }
        
        else return vec2<T>{};
    }


    template<typename T>
    const vec2<T> vec2<T>::operator+(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = { first + c, second + c };
            return out; 
        }
        
        else return vec2<T>{};
    }


    template<typename T>
    const vec2<T> vec2<T>::operator-(const vec2<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = { first - vec.first, second - vec.second };
            return out;
        }
        
        else return vec2<T>{};
    }


    template<typename T>
    const vec2<T> vec2<T>::operator-(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = { first - c, second - c };
            return out; 
        }
        
        else return vec2<T>{};
    }


    template<typename T>
    const T vec2<T>::operator*(const vec2<T> &vec) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T out = (vec.first * first + vec.second * second);
            return out;
        }
        
        else return T{};
    }


    template<typename T>
    const vec2<T> vec2<T>::operator*(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = {
                c * first,
                c * second,
            };

            return out; 
        }
        
        else return vec2<T>{};
    }


    template<typename T>
    const vec2<T> vec2<T>::operator/(const T c) const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            vec2<T> out = {
                first / c,
                second / c
            };

            return c;
        } 
        
        else return vec2<T>{};
    }


    template<typename T>
    void vec2<T>::operator*=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first *= c;
            second *= c;
        }
    }


    template<typename T>
    void vec2<T>::operator*=(const mat2<T> &m) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) 
            *this = m * (*this);
    }


    template<typename T>
    void vec2<T>::operator+=(const vec2<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += vec.first;
            second += vec.second;
        }
    }


    template<typename T>
    void vec2<T>::operator+=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first += c;
            second += c;
        }
    }

    
    template<typename T>
    void vec2<T>::operator-=(const vec2<T> &vec) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= vec.first;
            second -= vec.second;
        }
    }

    
    template<typename T>
    void vec2<T>::operator-=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first -= c;
            second -= c;
        }
    }


    template<typename T>
    void vec2<T>::operator/=(const T c) {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            first /= c;
            second /= c;
        }
    }


    template<typename T>
    const deng_bool_t vec2<T>::operator==(const vec2<T> &vec) const {
        return static_cast<deng_bool_t>(first == vec.first && second == vec.second); 
    }


    template<typename T>
    void vec2<T>::operator=(const das_ObjTextureData tex) {
        if(std::is_floating_point<T>::value) {
            first = tex.tex_x;
            second = tex.tex_y;
        }
    }


    template<typename T>
    void vec2<T>::operator=(const das_ObjPosData2D pos) {
        if(std::is_floating_point<T>::value) {
            first = pos.vert_x;
            second = pos.vert_y;
        }
    }

    
    template<typename T>
    const deng_bool_t vec2<T>::operator==(const das_ObjTextureData tex) const {
        if(std::is_floating_point<T>::value) {
            return static_cast<deng_bool_t>(
                first == tex.tex_x &&
                second == tex.tex_y
            );
        }

        else return false;
    }


    template<typename T>
    const deng_bool_t vec2<T>::operator==(const das_ObjPosData2D pos) const {
        if(std::is_floating_point<T>::value) {
            return static_cast<deng_bool_t>(
                first == pos.vert_x &&
                second == pos.vert_y
            );
        }

        else return false;
    }


    template<typename T>
    const T vec2<T>::length() const {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            return (T) sqrt (
                first * first +
                second * second
            );
        }

        else return T{};
    }


    template<typename T>
    void vec2<T>::norm() {
        if(std::is_floating_point<T>::value || std::is_integral<T>::value) {
            T len = length();
            first /= len;
            second /= len;
        }
    }
}

#endif
