//
//  laser_pulse.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "laser_pulse" contains a time varying electric field. The intital conditions are passed to the
//  constructor and field can be updated as it propagates.
//

#ifndef __LASER_PULSE_HPP_INCLUDED__
#define __LASER_PULSE_HPP_INCLUDED__

#include "../Eigen/Dense"
#include "grid_rkr.hpp"
#include "grid_tw.hpp"

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
	Modified by Patrick Anderson on 07/05/2015.
	"laser_pulse" contains a time varying electric field. The intital conditions are passed to the
	constructor and field can be updated as it propagates.
*/
class laser_pulse {
    
public:
    
    // Data
    ArrayXXd E;
    
    // Functions
    laser_pulse(double P_av_, double RR_, double FWHM_, double l_0_, double CEO_,
                double spot_radius_, double ROC_, grid_rkr rkr_, grid_tw tw_,
                std::string path_A_w_R, std::string path_A_w_I, std::string path_w_active, int read_in_laser_pulse);

    laser_pulse(grid_rkr rkr_, grid_tw tw_, ArrayXXcd A_w_active, ArrayXd w_active);

};

}

#endif
