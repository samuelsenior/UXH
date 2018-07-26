//
//  maths_textbook.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "maths_textbook" is a container for mathematical constants and functions.
//

#ifndef __MATHS_TEXTBOOK_HPP_INCLUDED__
#define __MATHS_TEXTBOOK_HPP_INCLUDED__

#include "Eigen/Dense"

#include <string>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"maths_textbook" is a container for mathematical constants and functions.
*/
class maths_textbook {

std::string path_input_j0;

public:
    // Data
    double pi;
    ArrayXd J0_zeros;

    // Functions
    maths_textbook(std::string path_input_j0_);
    double trapz(ArrayXd x_, ArrayXd y_);
    ArrayXd cumtrapz(ArrayXd x_, ArrayXd y_);
};

#endif
