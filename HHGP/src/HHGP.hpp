#include "../Eigen/Dense"

#include "config_settings.hpp"

using namespace Eigen;

namespace HHGP {

	ArrayXXcd nearFieldStep(ArrayXXcd source, ArrayXXcd previous,
							ArrayyXd w_active,
							double z, double dz, int step,
							Config_Settings config);

} // HHGP namespace
