#include <iostream>
#include <string>

//#include <mpi.h>

#include "../../Eigen/Dense"

#include "HHGP.hpp"

#include "config_settings.hpp"
#include "../maths/maths_textbook.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../DHT/DHT.hpp"
#include "../grid/grid_rkr.hpp"
#include "propagation.hpp"
#include "../IO/IO.hpp"

using namespace Eigen;

//namespace HHGP {
	HHGP::HHGP() {}

	HHGP::HHGP(propagation& prop_,
			   HH::Config_Settings& config_,
			   grid_rkr& rkr_, keldysh_gas& gas_,
			   maths_textbook& maths_, DHT& ht_)
		       : prop(prop_),
		       config(config_),
		       rkr(rkr_), gas(gas_),
		       maths(maths_), ht(ht_) {

		N_cols = -1;
	    n_active = -1;

		E_min = 10.0;

	}

	ArrayXXcd HHGP::nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
                                  double z, double dz) {

		// Am I expecting spectral amplitudes in terms of radial position or mode?
		N_cols = source.cols();
	    n_active = source.rows();

	    A_w_r = ArrayXXcd::Zero(n_active, N_cols);
	    A_w_r_tmp = ArrayXXcd::Zero(prop.n_k, N_cols);
	    // Want to propagate to the end fo the capillary and include the very final
	    // source terms but not propagate them outside of the capillary
	    // Don't need a step 1 for this when being called from UPPE
	    
        //prop.z += dz;
	    prop.z = z;// + dz;

        prop.nearFieldPropagationStep(dz, previous);
	    A_w_r_tmp = prop.A_w_r;
	    //
	    A_w_r = source;
		// The block is now taken in the main loop, so isn't needed here anymore
	    //A_w_r_tmp = prop.block(A_w_r);
	    A_w_r_tmp += source;

		return A_w_r_tmp;
	}
//} // HHGP namespace
