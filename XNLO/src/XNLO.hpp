#include "../../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

	struct Result {
		ArrayXXd acceleration;
		ArrayXXd w;
		ArrayXXd E;
		ArrayXXcd wavefunction;
	};

	Result XNLO(ArrayXXcd A_w_active, ArrayXd w_active, int w_active_min_index_UPPE, std::string print="minimum");

} // XNLO namespace
