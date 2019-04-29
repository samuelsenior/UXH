#ifndef __XNLO_HPP_INCLUDED__
#define __XNLO_HPP_INCLUDED__

#include "../../Eigen/Dense"

#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"
#include "../atom/Schrodinger_atom_1D.hpp"

#include "config_settings.hpp"

using namespace Eigen;

//namespace XNLO {
class XNLO_AtomResponse {
	// MPI
    int this_node;
    int total_nodes;
    MPI_Status status;
    int world_rank, world_size;

    grid_rkr& rkr;
    XNLO::grid_tw& tw;
    maths_textbook& maths;
    physics_textbook& physics;
    std::string print;

    int total_atoms;
    int atoms_per_worker;

    XNLO::Config_Settings config;

    Schrodinger_atom_1D atom;

    ArrayXXd E_field;
    ArrayXXd dipole;

public:
	ArrayXXd acceleration;
	ArrayXXd w;
	ArrayXXd E;
	ArrayXXcd wavefunction;

	XNLO_AtomResponse(grid_rkr& rkr_, XNLO::grid_tw& tw_,
		              maths_textbook& maths_, physics_textbook& physics_,
                      int this_node, int total_nodes,
                      XNLO::Config_Settings config_,
                      std::string print_="minimum");

	void run(ArrayXXcd A_w_active, ArrayXd w_active, int w_active_min_index_UPPE);

};
//} // XNLO namespace

#endif