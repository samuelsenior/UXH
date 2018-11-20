#include "../Eigen/Dense"

#include "../src/config_settings.hpp"

using namespace Eigen;

namespace HHGP {

	ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
							ArrayXd w_active,
							double z, double dz);

} // HHGP namespace
