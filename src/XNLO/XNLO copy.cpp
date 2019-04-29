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
namespace XNLO {

    Result XNLO(ArrayXXcd A_w_active, ArrayXd w_active, int w_active_min_index_UPPE,
                grid_rkr& rkr, grid_tw& tw,
                maths_textbook& maths, physics_textbook& physics, std::string print) {

      std::string config_file_path;
      config_file_path = "../configFiles/config_XNLO.txt";

        // MPI
        int this_node;
        int total_nodes;
        MPI_Status status;

        //MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &total_nodes);
        MPI_Comm_rank(MPI_COMM_WORLD, &this_node);

        int world_rank, world_size;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        if (this_node == 0) { std::cout << "XNLO started..." << std::endl; }

        if (total_nodes <= 1) {
            std::cout << "Error: total number of processes needs to be greater than 1, instead total processes = " << total_nodes << std::endl;
        }

        // Input Settings and Parameters
        Config_Settings config;
        if (print == "minimum" || print == "false") { config.print_to_screen = false;}
        else { config.print_to_screen = true; }
        if(config_file_path.empty() && this_node == 0) {
            std::cout << "Using default config file path " << config.path_config_file() << std::endl;
        } else {
            config.path_config_file_set(config_file_path);
            config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        }
        config.read_in(config.path_config_file(), config.print_to_screen);
        config.check_paths(config.print_to_screen);

        int total_atoms = config.atoms_per_worker() * (total_nodes - 1);
        int atoms_per_worker = config.atoms_per_worker();

        // Control
        if (this_node == 0) {
            // Field
            double ROC = std::numeric_limits<double>::max();
            laser_pulse pulse(rkr, tw, A_w_active, w_active, w_active_min_index_UPPE, maths, physics);
            // Send
            for (int ii = 1; ii < total_nodes; ii++) {
                MPI_Send(pulse.E.block(0, atoms_per_worker * (ii - 1), tw.N_t, atoms_per_worker).data(),
                         tw.N_t * atoms_per_worker, MPI_DOUBLE, ii, 1, MPI_COMM_WORLD);
            }
            // Receive
            ArrayXXd dipole = ArrayXXd::Zero(tw.N_t, total_atoms);

            ArrayXXcd wavefunction;
            if (config.output_wavefunction() == 1) { wavefunction = ArrayXXcd::Zero(tw.N_t, 4096); }
            else { wavefunction = ArrayXXcd::Zero(0, 0); }
            for (int jj = 1; jj < total_nodes; jj++) {
                // Request
                MPI_Recv(dipole.block(0, atoms_per_worker * (jj - 1), tw.N_t, atoms_per_worker).data(),
                         tw.N_t * atoms_per_worker, MPI_DOUBLE, jj, 1, MPI_COMM_WORLD, &status);
            }
            if (config.output_wavefunction() == 1) {
                MPI_Recv(wavefunction.block(0, 0, tw.N_t, 4096).data(),
                         4096 * tw.N_t, MPI_DOUBLE_COMPLEX, 1, 1, MPI_COMM_WORLD, &status);
            }

            if (this_node == 0) { std::cout << "XNLO successfully ran!\n"; }

            Result result;
            result.acceleration = dipole;
            result.w = tw.w;
            if (config.output_electric_field() == 1) { result.E = pulse.E; }
            else { result.E = ArrayXXd::Zero(0, 0); }
            result.wavefunction = wavefunction;
            return result;
        }

        // Worker
        if (this_node != 0) {
            // Receive
            ArrayXXd E = ArrayXXd::Zero(tw.N_t, atoms_per_worker);
            MPI_Recv(E.data(), tw.N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);

            // Single atom calculations
            ArrayXXd dipole = ArrayXXd::Zero(tw.N_t, atoms_per_worker);

            ArrayXXcd wavefunction;
            if (config.output_wavefunction() == 1) { wavefunction = ArrayXXcd::Zero(tw.N_t, 4096); }
            else { wavefunction = ArrayXXcd::Zero(0, 0); }

            Schrodinger_atom_1D atom;
            if (print == "minimum") {
                if (this_node == 1) { atom = Schrodinger_atom_1D(tw, config.alpha(), config.output_wavefunction(), true); }
                else { atom = Schrodinger_atom_1D(tw, config.alpha(), config.output_wavefunction(), false); } }
            else if (print == "false") { atom = Schrodinger_atom_1D(tw, config.alpha(), config.output_wavefunction(), false); }
            else { atom = Schrodinger_atom_1D(tw, config.alpha(), config.output_wavefunction(), true); }
            for (int ii = 0; ii < atoms_per_worker; ii++) { dipole.col(ii) = atom.get_acceleration(tw.N_t, tw.dt, E.col(ii)); }

            if (config.output_wavefunction() == 1) { wavefunction = atom.wfn_output; }

            // Send
            MPI_Send(dipole.data(), tw.N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
            if (config.output_wavefunction() == 1 && this_node == 1) {
                MPI_Send(wavefunction.data(), 4096 * tw.N_t, MPI_DOUBLE_COMPLEX, 0, 1, MPI_COMM_WORLD);
            }
        }
        if (this_node == 0) { std::cout << "XNLO successfully ran!\n"; }

        ArrayXXd zeros = ArrayXXd::Zero(1, 1);
        ArrayXXcd complex_zeros = ArrayXXcd::Zero(1, 1);
        Result result;
        result.acceleration = zeros;
        result.w = zeros;
        result.E = zeros;
        result.wavefunction = complex_zeros;

        return result;
    }

} // XNLO namespace
