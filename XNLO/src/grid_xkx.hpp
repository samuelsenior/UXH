//
//  grid_xkx.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "grid_xkx" is a linear 1D spatial grid. The spectral counterpart of this grid is evaluated and
//  accessible.
//

#ifndef __GRID_XKX_HPP_INCLUDED__
#define __GRID_XKX_HPP_INCLUDED__

#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
    Modified by Patrick Anderson on 07/05/2015.
    "grid_xkx" is a linear 1D spatial grid. The spectral counterpart of this grid is evaluated and
    accessible.
*/
class grid_xkx {
    
public:
    
    // Data
    ArrayXd x;
    ArrayXd kx;
    int N_x;
    double x_min;
    double x_max;
    double dx;
    
    // Functions
    grid_xkx();
    grid_xkx(int N_x_, double x_min_, double x_max_);
    
};

}

#endif

