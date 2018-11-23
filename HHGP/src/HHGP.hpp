#include "../Eigen/Dense"

#include "config_settings.hpp"

#include "config_settings.hpp"
#include "maths_textbook.hpp"
#include "physics_textbook.hpp"
#include "keldysh_gas.hpp"
#include "DHT.hpp"
#include "grid_rkr.hpp"
#include "propagation.hpp"
#include "IO.hpp"

using namespace Eigen;

namespace HHGP {

	class HHGP {
		ArrayXd w_active;

		std::string config_file_path;
		Config_Settings config;

		int N_cols;
		int N_rows;
		int N_cols_w;
	    int N_rows_w;

	    maths_textbook maths(std::string);

	    DHT ht(int, maths_textbook);
	    int n_active;

	    physics_textbook physics;

	    // Grids
	    grid_rkr rkr;
	    
	    MKL_LONG dimensions;
	    MKL_LONG length;
	    double scale;
	    DFTI_DESCRIPTOR_HANDLE ft;

	    grid_tw tw;
	    keldysh_gas gas;

	    double E_min;

	    //propagation prop;

	    ArrayXXcd A_w_r;
	    ArrayXXcd A_w_r_tmp;


	public:
		HHGP();

		ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
								ArrayXd w_active,
								double z, double dz);
		propagation prop;
	};

} // HHGP namespace