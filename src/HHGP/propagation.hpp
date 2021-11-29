#ifndef __PROPAGATION_HPP_INCLUDED__
#define __PROPAGATION_HPP_INCLUDED__

#include "config_settings.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../physics/physics_textbook.hpp"
#include "../maths/maths_textbook.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../DHT/DHT.hpp"
#include "../../Eigen/Dense"

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class propagation{

    // Data
    double E_min;
    double E_max;
    double Z_max;
    physics_textbook physics;  /*!< Physical constants */
    maths_textbook maths;      /*!< Mathematical constants and functions */
    keldysh_gas gas;
    grid_rkr rkr;              /*!< Radial grid */
    DHT ht;                    /*!< Hankel transform */

    bool to_end_only;

    Eigen::ArrayXd w_active_tmp;
    int k_excluded;

    Eigen::ArrayXcd refractiveIndex;

    Eigen::ArrayXcd lamda;

    Eigen::ArrayXcd A_w_kr;

    std::string E_f1_f2_data_path;
    Eigen::ArrayXXd E_f1_f2_data;
    Eigen::ArrayXd E;
    Eigen::ArrayXd f2;
    Eigen::ArrayXd f1;

    Eigen::ArrayXcd n_k_squared_tmp;
    Eigen::ArrayXcd ones;

public:
    // Data
    double z;
    int n_k;
    Eigen::ArrayXd w_active;
    Eigen::ArrayXcd k;
    Eigen::ArrayXcd k_r;
    Eigen::ArrayXXcd A_w_r;

    bool print;

    // Functions
    propagation();
    propagation(double E_min_, double E_max_, double Z_max_, Eigen::ArrayXd w_active_,
                keldysh_gas& gas_, grid_rkr& rkr_,
                physics_textbook& physics_, maths_textbook& maths_,
                DHT& ht_, bool print_=false);

    Eigen::ArrayXd segment(Eigen::ArrayXd k);
    Eigen::ArrayXXcd block(Eigen::ArrayXXcd A_w_e_);

    double totalNumberOfAtoms();
    std::complex<double> n(int i);

    void nearFieldPropagationStep(double delta_z, Eigen::ArrayXXcd A_w_r_);
    void farFieldPropagation();
};

#endif
