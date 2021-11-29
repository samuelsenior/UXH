//
//  DHT.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "DHT" evaluates the forward and backward discrete Hankel transform. Based on Fisk, Computer
//  Physics Communications, 43 (1987). Complex datatype used here, should really template/overload.
//

#ifndef __DHT_HPP_INCLUDED__
#define __DHT_HPP_INCLUDED__

#include "../../Eigen/Dense"
#include "../maths/maths_textbook.hpp"


//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"DHT" evaluates the forward and backward discrete Hankel transform. Based on Fisk, Computer
Physics Communications, 43 (1987). Complex datatype used here, should really template/overload.
*/
class DHT {
    // Data
    //MatrixXcd H; // Change this back to being private once bug testing done

    // Functions

public:
    // Data
   	MatrixXcd H; // Change this back to being private once bug testing done

    // Functions
    DHT();
    DHT(int n_r_, maths_textbook& maths_);
    Eigen::ArrayXcd forward(Eigen::ArrayXcd f_r_);
    Eigen::ArrayXcd backward(Eigen::ArrayXcd f_kr_);
};

#endif
