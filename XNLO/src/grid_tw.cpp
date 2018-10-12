//
//  grid_tw.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
//  made accessible.
//

#include "grid_tw.hpp"
#include "maths_textbook.hpp"
#include "../Eigen/Dense"

using namespace Eigen;

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
