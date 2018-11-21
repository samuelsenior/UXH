#include "../Eigen/Dense"

#include "../src/config_settings.hpp"
#include "../src/maths_textbook.hpp"
#include "../src/physics_textbook.hpp"
#include "../src/keldysh_gas.hpp"
#include "../src/DHT.hpp"
#include "../src/grid_rkr.hpp"
#include "../src/propagation.hpp"
#include "../src/IO.hpp"

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

	    maths_textbook maths;

	    DHT ht;
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
		HHGP(ArrayXd w_active);

		ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
								double z, double dz);
		propagation prop;
	};

} // HHGP namespace
