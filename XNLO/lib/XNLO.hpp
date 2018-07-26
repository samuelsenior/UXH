#include "../Eigen/Dense"

using namespace Eigen;

namespace XNLO {

struct Result {
	ArrayXXd acceleration;
	ArrayXXd w;
	ArrayXXd E;
	ArrayXXcd wavefunction;
};

Result XNLO(ArrayXXcd A_w_active, ArrayXd w_active);

}