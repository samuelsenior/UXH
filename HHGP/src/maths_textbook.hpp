//
//  maths_textbook.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "maths_textbook" is a container for mathematical constants and common functions.
//

#ifndef __MATHS_TEXTBOOK_HPP_INCLUDED__
#define __MATHS_TEXTBOOK_HPP_INCLUDED__

#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
	Modified by Patrick Anderson on 07/05/2015.
	"maths_textbook" is a container for mathematical constants and common functions.
*/
class maths_textbook {

std::string path_input_j0;

public:

    // Data
    double pi;
    ArrayXd J0_zeros;
    
    // Functions
    maths_textbook();
    maths_textbook(std::string path_input_j0_);
    double trapz(ArrayXd x_, ArrayXd y_);
    ArrayXd interp1D(ArrayXd input_array, int input_length, int output_length, int spline_order);

};

}

#endif
