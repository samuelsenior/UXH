#ifndef __PROPAGATION_HPP_INCLUDED__
#define __PROPAGATION_HPP_INCLUDED__

#include "../../src/keldysh_gas.hpp"
#include "../../src/physics_textbook.hpp"
#include "../../src/maths_textbook.hpp"
#include "../../src/grid_rkr.hpp"
#include "../../src/grid_tw.hpp"
#include "../../src/DHT.hpp"
#include "../../Eigen/Dense"

//using namespace Eigen;

//namespace HHGP {

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class propagation{

    // Data
    // E_min should really come from config or a data_config
    double E_min;
    double E_max;
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    keldysh_gas gas;
    grid_rkr rkr;              /*!< Radial grid */
    DHT ht;                    /*!< Hankel transform */

    Eigen::ArrayXd w_active_tmp;
    int k_excluded;
//
    Eigen::ArrayXcd k;
    Eigen::ArrayXcd refractiveIndex;
//
    Eigen::ArrayXcd lamda;
//
//
    Eigen::ArrayXcd A_w_kr;


    // Bug testing...
    std::string E_f1_f2_data_path;
    Eigen::ArrayXXd E_f1_f2_data;
    Eigen::ArrayXd E;
    Eigen::ArrayXd f2;
    Eigen::ArrayXd f1;

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
    propagation(double E_min_, double E_max_, Eigen::ArrayXd w_active_,
                keldysh_gas& gas_, grid_rkr& rkr_,
                physics_textbook& physics_, maths_textbook& maths_,
                DHT& ht_);

    Eigen::ArrayXd segment(Eigen::ArrayXd k);
    Eigen::ArrayXXcd block(Eigen::ArrayXXcd A_w_e_);

    std::complex<double> n(int i);

    void nearFieldPropagationStep(double dz, Eigen::ArrayXXcd A_w_r_);
    void farFieldPropagation();
};

//} // HHGP namespace

#endif
