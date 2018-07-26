//
//  grid_tw.hpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
//  made accessible.
//

#ifndef __GRID_TW_HPP_INCLUDED__
#define __GRID_TW_HPP_INCLUDED__

#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
	Modified by Patrick Anderson on 07/05/2015.
	"grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
	made accessible.
*/
class grid_tw {
    
public:
    
    //Data
    ArrayXd t;
    ArrayXd w;
    int N_t;
    double t_min;
    double t_max;
    double dt;
    
    //Functions
    grid_tw(int N_t_, double t_min_, double t_max_);
    
};

}

#endif

