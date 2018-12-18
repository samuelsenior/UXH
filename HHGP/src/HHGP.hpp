#ifndef __HHGP_HPP_INCLUDED__
#define __HHGP_HPP_INCLUDED__


#include "../../Eigen/Dense"

#include "config_settings.hpp"

//#include "config_settings.hpp"
#include "../../src/maths_textbook.hpp"
#include "../../src/physics_textbook.hpp"
#include "../../src/keldysh_gas.hpp"
#include "../../src/DHT.hpp"
#include "../../src/grid_rkr.hpp"
#include "propagation.hpp"
#include "../../src/IO.hpp"

using namespace Eigen;

//namespace HHGP {

class HHGP {
	//ArrayXd w_active;

	//int n_r;

	//std::string config_file_path;
	HH::Config_Settings config;
    grid_rkr rkr;
    keldysh_gas gas;
    maths_textbook maths;
    DHT ht;

	int N_cols;
    int n_active;

    double E_min;

    propagation prop;

    ArrayXXcd A_w_r;
    ArrayXXcd A_w_r_tmp;

	public:
		HHGP();
		HHGP(propagation& prop_,
             HH::Config_Settings& config_,
             grid_rkr& rkr_, keldysh_gas& gas_,
             maths_textbook& maths_, DHT& ht_);

		ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
								double z, double dz);
		//propagation prop(double E_min, Eigen::ArrayXd w_active, keldysh_gas gas, grid_rkr rkr, DHT ht);
};

//} // HHGP namespace

#endif