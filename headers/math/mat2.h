/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 



/*
 * Matrix structure requirements are follwing
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


#ifndef __MAT2_H
#define __MAT2_H


namespace dengMath {
    template<typename T>
    struct mat2 {
        vec2<T> row1;
        vec2<T> row2;
    
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
    };
}

#include <../src/math/mat2.cpp>
#endif