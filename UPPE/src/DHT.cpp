//
//  DHT.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "DHT" evaluates the forward and backward discrete Hankel transform. Based on Fisk, Computer
//  Physics Communications, 43 (1987). Complex datatype used here, should really template/overload.
//

#include "DHT.hpp"
#include "Eigen/Dense"
#include "maths_textbook.hpp"

//using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Default constructor */
DHT::DHT() {  }

//------------------------------------------------------------------------------------------------//
/*! Parameterized constructor */
DHT::DHT(int n_r_, maths_textbook& maths_) {

    //j0 and j1 and from cmath and are Bessel functions of the first kind

    // Transformation matrix
    Eigen::MatrixXd temp_1 = Eigen::MatrixXd::Zero(n_r_, n_r_);
    for (int ii = 0; ii < n_r_; ii++) {
        for (int jj = 0; jj < n_r_; jj++) {
            temp_1(ii, jj) = (2.0 / maths_.J0_zeros(n_r_)) *
                             (j0(maths_.J0_zeros(jj) * maths_.J0_zeros(ii) /
                                 maths_.J0_zeros(n_r_)) /
                              std::pow(j1(maths_.J0_zeros(jj)), 2.0));
        }
    }
    H = temp_1.cast<std::complex<double> >();
}

//------------------------------------------------------------------------------------------------//
/*! Forward transform */
Eigen::ArrayXcd DHT::forward(Eigen::ArrayXcd f_r_) {

    Eigen::ArrayXcd output = H * f_r_.matrix();

    return(output);
}

//------------------------------------------------------------------------------------------------//
/*! Backward transform */
Eigen::ArrayXcd DHT::backward(Eigen::ArrayXcd f_kr_) {

    Eigen::ArrayXcd output = H * f_kr_.matrix();

    return(output);
}
