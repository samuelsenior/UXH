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

	int n_r;

	std::string config_file_path;
	Config_Settings config;

//	int N_cols;
//	int N_rows;
//	int N_cols_w;
//    int N_rows_w;

//    maths_textbook maths(std::string);

//    DHT ht(int, maths_textbook);
//    int n_active;

//    physics_textbook physics;

    // Grids
//    grid_rkr rkr;
    
//    MKL_LONG dimensions;
//    MKL_LONG length;
//    double scale;
//    DFTI_DESCRIPTOR_HANDLE ft;

//    grid_tw tw;//(int, double, double, double, maths_textbook);
//    keldysh_gas gas;//(double press_, grid_tw& tw_, DFTI_DESCRIPTOR_HANDLE& ft_, maths_textbook& maths_);

    double E_min;

    propagation prop;

//    ArrayXXcd A_w_r;
//    ArrayXXcd A_w_r_tmp;


	public:
		HHGP();
		HHGP(propagation& prop, Config_Settings& config);

		ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
								ArrayXd w_active,
								double z, double dz);
		//propagation prop(double E_min, Eigen::ArrayXd w_active, keldysh_gas gas, grid_rkr rkr, DHT ht);
};

//} // HHGP namespace

#endif