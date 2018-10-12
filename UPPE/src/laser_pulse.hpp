//
//  laser_pulse.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "laser_pulse" contains the active spectral amplitudes and governs their propagation over
//  longitudinal step dz.
//

#ifndef __LASER_PULSE_HPP_INCLUDED__
#define __LASER_PULSE_HPP_INCLUDED__

#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "grid_tw.hpp"
#include "grid_rkr.hpp"
#include <mkl.h>
#include "DHT.hpp"
#include "Eigen/Dense"
#include "capillary_fibre.hpp"
#include "keldysh_gas.hpp"

using namespace Eigen;

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

    // Functions
    laser_pulse(double p_av_, double rep_, double fwhm_, double l_0_, double ceo_, double waist_,
                grid_tw& tw_, grid_rkr& rkr_, DFTI_DESCRIPTOR_HANDLE& ft_, DHT& ht_, maths_textbook& maths_);
    void propagate(double dz_, capillary_fibre& capillary_, keldysh_gas& gas_);
};

#endif
