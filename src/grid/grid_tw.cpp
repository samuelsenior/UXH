//
//  grid_tw.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
//  made accessible.
//

#include "grid_tw.hpp"
#include "../maths/maths_textbook.hpp"
#include "../../Eigen/Dense"

#include <iostream>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
//  Default constructor
grid_tw::grid_tw() {  }

//------------------------------------------------------------------------------------------------//
/*! Parameterized Constructor */
grid_tw::grid_tw(int n_t_, double T_, double w_active_min_, double w_active_max_,
                 maths_textbook& maths_) :
                 n_t(n_t_) {

    // Temporal
    t = ArrayXd::LinSpaced(n_t, -T_ / 2.0, T_ / 2.0);
    double dt = t(1) - t(0);
    // Spectral
    ArrayXd w = 2.0 * maths_.pi * ArrayXd::LinSpaced(n_t, 0.0, n_t - 1.0) * (1.0 / (dt * n_t));
    w_active_min_index = 0;
    while (w(w_active_min_index) < w_active_min_)
        w_active_min_index++;
    int count = 0;
    while (w(count) < w_active_max_) {
        count++;
    }
    n_active = count - w_active_min_index;
    w_active = w.segment(w_active_min_index, n_active);


    std::cout << "w_active_min_index: " << w_active_min_index << std::endl;
}

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
grid_tw::grid_tw(int N_t_, double t_min_, double t_max_) {
    
    maths_textbook maths;
    
    // Temporal
    N_t = N_t_;
    t_min = t_min_;
    t_max = t_max_;
    t = ArrayXd::Zero(N_t);
    t.setLinSpaced(t_min, t_max);
    dt = t(1) - t(0);

    // Spectral
    w = ArrayXd::Zero(N_t);
    w.setLinSpaced(-N_t / 2, (N_t / 2) - 1);
    w *=  2 * maths.pi / (N_t * (t(1) - t(0)));
    w.tail(N_t / 2).swap(w.head(N_t / 2));                                                          // Equivalent to fftshift(kx)
    
}

}