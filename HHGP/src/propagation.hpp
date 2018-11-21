#ifndef __PROPAGATION_HPP_INCLUDED__
#define __PROPAGATION_HPP_INCLUDED__

#include "keldysh_gas.hpp"
#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "grid_rkr.hpp"
#include "grid_tw.hpp"
#include "DHT.hpp"
#include "Eigen/Dense"

//using namespace Eigen;

namespace HHGP {

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class propagation{

    // Data
    // E_min should really come from config or a data_config
    double E_min;
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    keldysh_gas gas;
    grid_rkr rkr;              /*!< Radial grid */
    DHT ht;                    /*!< Hankel transform */

    Eigen::ArrayXd w_active_tmp;
    int k_excluded;

    Eigen::ArrayXcd k;
    Eigen::ArrayXcd refractiveIndex;

    Eigen::ArrayXcd lamda;


    Eigen::ArrayXcd A_w_kr;

public:
    // Data
    double z;
    int n_k;
    Eigen::ArrayXd w_active;
    Eigen::ArrayXcd k_r;
    Eigen::ArrayXXcd A_w_r;

    // Functions
    // E_min should really come from config or a data_config
    propagation();
    propagation(double E_min, Eigen::ArrayXd w_active, keldysh_gas gas, grid_rkr rkr, DHT ht);

    Eigen::ArrayXd segment(Eigen::ArrayXd k);
    Eigen::ArrayXXcd block(Eigen::ArrayXXcd A_w_e_);

    std::complex<double> n(int i);

    void nearFieldPropagationStep(double dz, Eigen::ArrayXXcd A_w_r_);
    void farFieldPropagation();
};

} // HHGP namespace

#endif
