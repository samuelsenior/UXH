//
//  laser_pulse.cpp
//
//  Modified by Patrick Anderson on 07/05/2015.
//  "laser_pulse" contains a time varying electric field. The intital conditions are passed to the
//  constructor and field can be updated as it propagates.
//

#ifndef __LASER_PULSE_HPP_INCLUDED__
#define __LASER_PULSE_HPP_INCLUDED__

#include <mkl.h>

#include "../../Eigen/Dense"

#include "../capillary/capillary_fibre.hpp"
#include "../DHT/DHT.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../physics/physics_textbook.hpp"
#include "../maths/maths_textbook.hpp"

#include "../UPPE/config_settings.hpp"

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

    maths_textbook maths;
    physics_textbook physics;
    
    // Functions
    laser_pulse(double P_av_, double RR_, double FWHM_, double l_0_, double CEO_,
                double spot_radius_, double ROC_, grid_rkr rkr_, grid_tw tw_,
                std::string path_A_w_R, std::string path_A_w_I, std::string path_w_active, int read_in_laser_pulse);

    laser_pulse(grid_rkr rkr_, grid_tw tw_, ArrayXXcd A_w_active, ArrayXd w_active, int w_active_min_index_UPPE,
                maths_textbook& maths_, physics_textbook& physics_);

};

} // XNLO namespace

namespace UPPE {
//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"laser_pulse" contains the active spectral amplitudes and governs their propagation over
longitudinal step dz.
*/
class laser_pulse {

    double p_av;
    double rep;
    double fwhm;
    double l_0;
    double ceo;
    double waist;

    int read_in_laser_pulse;
    double initial_position;

    Config_Settings config;

    // Data
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    grid_tw tw;                /*!< Temperal grid */
    grid_rkr rkr;              /*!< Radial grid */
    DFTI_DESCRIPTOR_HANDLE ft; /*!< Fourier transform */
    DHT ht;                    /*!< Hankel transform */
    ArrayXXcd Y_4;             /*!< RKF 4 */
    ArrayXXcd Y_5;             /*!< RKF 5 */
    double e;                  /*!< Error in RKF45 solve */

    double z_position;

    // Functions
    void RK_F_45(double dz_, capillary_fibre& capillary_, keldysh_gas& gas_);
    ArrayXXcd RHS_UPPE(double dz_, ArrayXXcd A_w_active, capillary_fibre& capillary_,
                       keldysh_gas& gas_);

public:
    // Data
    ArrayXXcd A_w_active; /*!< Spectral amplitudes */
    ArrayXXcd P_NL_r_t;
    ArrayXXcd P_NL_m_t;
    ArrayXXcd P_NL_w;

    ArrayXXd electron_density;
    double atom_density_max;

    double p_pk;
    double E_pk;

    double rel_tol;

    // Functions
    laser_pulse(double p_av_, double rep_, double fwhm_, double l_0_, double ceo_, double waist_,
                grid_tw& tw_, grid_rkr& rkr_, DFTI_DESCRIPTOR_HANDLE& ft_, DHT& ht_, maths_textbook& maths_,
                Config_Settings config,
                int read_in_laser_pulse, double initial_position,
                double rel_tol_);
    void propagate(double dz_, capillary_fibre& capillary_, keldysh_gas& gas_);
};
} // UPPE namespace

#endif
