//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 13/03/2017.
//  Test environment for single atom codes.
//


#include <mpi.h>
#include "XNLO.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../laser_pulse/laser_pulse.hpp"
#include <limits>
#include "../atom/Schrodinger_atom_1D.hpp"

#include "../maths/maths_textbook.hpp"
#include "../physics/physics_textbook.hpp"

#include "config_settings.hpp"
#include <string>

#include <iostream>

#include "../../Eigen/Dense"

/*!
    Originally created by Patrick Anderson.
    Modified by Samuel Senior on 13/03/2017.
    Test environment for single atom codes.
*/

using namespace Eigen;

/*!
    XNLO namespace - A container for everything XNLO, so that all classes etc that are a part
    of it are self contained and so that it is harder to confuse with other namespaces etc.
*/
XNLO_AtomResponse::XNLO_AtomResponse() {}
XNLO_AtomResponse::XNLO_AtomResponse(grid_rkr* rkr_, XNLO::grid_tw* tw_,
           maths_textbook* maths_, physics_textbook* physics_,
           int this_node_, int total_nodes_,
           XNLO::Config_Settings config_,
           std::string print_)
           :
           rkr(rkr_),
           tw(tw_),
           maths(maths_),
           physics(physics_),
           this_node(this_node_),
           total_nodes(total_nodes_),
           config(config_),
           print(print_) {

    if (this_node == 0) { std::cout << "Initialising XNLO..." << std::endl; }
    if (total_nodes <= 1) {
        std::cout << "Error: total number of processes needs to be greater than 1, instead total processes = " << total_nodes << std::endl;
    }

    // Input Settings and Parameters
    if (print == "minimum" || print == "false") { config.print_to_screen = false;}
    else { config.print_to_screen = true; }

    config.read_in(config.path_config_file(), config.print_to_screen);
    config.check_paths(config.print_to_screen);

    total_atoms = config.atoms_per_worker() * (total_nodes - 1);
    atoms_per_worker = config.atoms_per_worker();

    if (this_node == 0) { std::cout << "Initialising Schrodinger_atom_1D..." << std::endl; }
    if (print == "minimum") {
        if (this_node == 1) {
            atom = Schrodinger_atom_1D(*tw, config.alpha(), config.SAR_N_x(), config.SAR_x_min(), config.SAR_x_max(), config.output_wavefunction(), true);
        } else {
            atom = Schrodinger_atom_1D(*tw, config.alpha(), config.SAR_N_x(), config.SAR_x_min(), config.SAR_x_max(), config.output_wavefunction(), false);
        }
    }
    else if (print == "false") {
        atom = Schrodinger_atom_1D(*tw, config.alpha(), config.SAR_N_x(), config.SAR_x_min(), config.SAR_x_max(), config.output_wavefunction(), false);
    } else {
        atom = Schrodinger_atom_1D(*tw, config.alpha(), config.SAR_N_x(), config.SAR_x_min(), config.SAR_x_max(), config.output_wavefunction(), true);
    }
    if (this_node == 0) { std::cout << "Schrodinger_atom_1D initialised!" << std::endl; }

    if (this_node == 0) {
        dipole = ArrayXXd::Zero(tw->N_t, total_atoms);
    } else {
        E_field = ArrayXXd::Zero(tw->N_t, atoms_per_worker);
        dipole = ArrayXXd::Zero(tw->N_t, atoms_per_worker);
    }


    if (this_node == 0) { std::cout << "XNLO initialised." << std::endl; }

}

void XNLO_AtomResponse::run(ArrayXXcd A_w_active, ArrayXd w_active, int N_t_UPPE, int w_active_min_index_UPPE) {
    // Control
    if (this_node == 0) {
        std::cout << "Single atom response calculations started..." << std::endl;
        // Field
        double ROC = std::numeric_limits<double>::max();

        XNLO::laser_pulse pulse(*rkr, *tw, A_w_active, w_active, N_t_UPPE, w_active_min_index_UPPE, *maths, *physics);

        // Send
        for (int ii = 1; ii < total_nodes; ii++) {

            MPI_Send(pulse.E.block(0, atoms_per_worker * (ii - 1), tw->N_t, atoms_per_worker).data(),
                     tw->N_t * atoms_per_worker, MPI_DOUBLE, ii, 1, MPI_COMM_WORLD);
        }

        // Receive
        if (config.output_wavefunction() == 1) { wavefunction = ArrayXXcd::Zero(tw->N_t, 4096); }
        else { wavefunction = ArrayXXcd::Zero(0, 0); }
        for (int jj = 1; jj < total_nodes; jj++) {
            // Request
            MPI_Recv(dipole.block(0, atoms_per_worker * (jj - 1), tw->N_t, atoms_per_worker).data(),
                     tw->N_t * atoms_per_worker, MPI_DOUBLE, jj, 1, MPI_COMM_WORLD, &status);
        }
        if (config.output_wavefunction() == 1) {
            MPI_Recv(wavefunction.block(0, 0, tw->N_t, 4096).data(),
                     4096 * tw->N_t, MPI_DOUBLE_COMPLEX, 1, 1, MPI_COMM_WORLD, &status);
        }

        acceleration = dipole;
        w = tw->w;
        if (config.output_electric_field() == 1) { E = pulse.E; }
        else { E = ArrayXXd::Zero(0, 0); }
        wavefunction = wavefunction;
        std::cout << "Single atom response calculations finished!" << std::endl;
    } else if (this_node != 0) {
    // Worker
        // Receive
        MPI_Recv(E_field.data(), tw->N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        // Single atom calculations

        if (config.output_wavefunction() == 1) { wavefunction = ArrayXXcd::Zero(tw->N_t, 4096); }
        else { wavefunction = ArrayXXcd::Zero(0, 0); }

        for (int ii = 0; ii < atoms_per_worker; ii++) { dipole.col(ii) = atom.get_acceleration(tw->N_t, tw->dt, E_field.col(ii)); }

        if (config.output_wavefunction() == 1) { wavefunction = atom.wfn_output; }

        // Send
        MPI_Send(dipole.data(), tw->N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        if (config.output_wavefunction() == 1 && this_node == 1) {
            MPI_Send(wavefunction.data(), 4096 * tw->N_t, MPI_DOUBLE_COMPLEX, 0, 1, MPI_COMM_WORLD);
        }
    }
}

