//
//  grid_tw.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
//  made accessible.
//

#ifndef __GRID_TW_HPP_INCLUDED__
#define __GRID_TW_HPP_INCLUDED__

#include "../../Eigen/Dense"
#include "../maths/maths_textbook.hpp"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*! Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"grid_tw" is a linear temporal grid. The spectral counterpart of this grid is evaluated and
made accessible.
*/
class grid_tw {

public:
    // Data
    ArrayXd t;
    ArrayXd w_active;
    int n_t;
    int n_active;
    int w_active_min_index;

    // Functions
    grid_tw();
    grid_tw(int N_t_, double T_, double w_active_min_, double w_active_max_, maths_textbook& maths_);
};

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
