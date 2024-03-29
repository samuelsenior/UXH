//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 13/03/2017.
//  Test environment for single atom codes.
//


#include <mpi.h>
#include "../maths/maths_textbook.hpp"
#include "../grid/grid_xkx.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../laser_pulse/laser_pulse.hpp"
#include <limits>
#include "../IO/IO.hpp"
#include "../atom/Schrodinger_atom_1D.hpp"

#include "config_settings.hpp"
#include <string>

#include <iostream>


/*!
    Originally created by Patrick Anderson.
    Modified by Samuel Senior on 13/03/2017.
    Test environment for single atom codes.
*/
int main(int argc, char** argv){

  std::string args[argc];
  std::string config_file_path;
  for (int i = 0; i < argc; i++) {
    args[i] = argv[i];
  }
  for (int i = 0; i < argc; i++) {
    if (args[i] == "-cf") {
      config_file_path = argv[i+1];
    }
  }

    // MPI
    int this_node;
    int total_nodes;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_nodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &this_node);

    if (this_node == 0) {
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "                                  XNLO\n";
        std::cout << "-------------------------------------------------------------------------------\n";
    }

    if (total_nodes <= 1) {
        std::cout << "Error: total number of processes needs to be greater than 1, instead total processes = " << total_nodes << std::endl;
    }

    // Input Settings and Parameters
    XNLO::Config_Settings config;
    if(config_file_path.empty() && this_node == 0) {
        std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    } else {
        config.path_config_file_set(config_file_path);
        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        if (this_node == 0) {
            std::cout << "Using config file path " << config.path_config_file() << std::endl;
        }
    }
    config.read_in(config.path_config_file(), this_node==0);
    config.check_paths(this_node==0);
    if (this_node == 0) {
        config.print(config.path_config_log());
        config.print();
    }

    int total_atoms = config.atoms_per_worker() * (total_nodes - 1);
    int N_x = total_atoms;

    // Maths
    maths_textbook maths(config.path_input_j0());

    // Grids
    grid_xkx xkx(N_x, config.x_min(), config.x_max());
    grid_rkr rkr(N_x, config.x_max(), N_x, maths);
    XNLO::grid_tw tw(config.N_t(), config.t_min(), config.t_max());
    int atoms_per_worker = config.atoms_per_worker();

    if (this_node == 0) {
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "Main Program:\n";
        std::cout << "-------------------------------------------------------------------------------\n";
    }

    // Control
    if (this_node == 0) {

        // Field
        double ROC = std::numeric_limits<double>::max();
        XNLO::laser_pulse pulse(config.P_av(), config.RR(), config.FWHM(), config.l_0(), config.CEO(), config.spot_radius(), ROC, rkr, tw,
                          config.path_laser_A_w_R(), config.path_laser_A_w_I(), config.path_laser_w_active(), config.N_t_UPPE(),
                          config.read_in_laser_pulse());

        // Send
        for (int ii = 1; ii < total_nodes; ii++) {

            MPI_Send(pulse.E.block(0, atoms_per_worker * (ii - 1), tw.N_t, atoms_per_worker).data(),
                     tw.N_t * atoms_per_worker, MPI_DOUBLE, ii, 1, MPI_COMM_WORLD);

        }

        // Receive
        ArrayXXd acceleration = ArrayXXd::Zero(tw.N_t, total_atoms);

        ArrayXXcd wavefunction;
        if (config.output_wavefunction() == 1) {
            wavefunction = ArrayXXcd::Zero(tw.N_t, config.SAR_N_x());
        } else {
            wavefunction = ArrayXXcd::Zero(0, 0);
        }

        for (int jj = 1; jj < total_nodes; jj++) {

            // Request
            bool send = true;
            MPI_Send(&send, 1, MPI_C_BOOL, jj, 1, MPI_COMM_WORLD);

            MPI_Recv(acceleration.block(0, atoms_per_worker * (jj - 1), tw.N_t, atoms_per_worker).data(),
                     tw.N_t * atoms_per_worker, MPI_DOUBLE, jj, 1, MPI_COMM_WORLD, &status);

        }

        if (config.output_wavefunction() == 1) {
            MPI_Recv(wavefunction.block(0, 0, tw.N_t, config.SAR_N_x()).data(),
                         config.SAR_N_x() * tw.N_t, MPI_DOUBLE_COMPLEX, 1, 1, MPI_COMM_WORLD, &status);
        }

        // Output
        IO file;

        file.overwrite(config.path_acceleration());
        file.write_header(config.path_acceleration(), config.N_t(), total_atoms);
        file.write_double(config.path_acceleration(), acceleration, config.N_t(), total_atoms);

        file.overwrite(config.path_w());
        file.write_header(config.path_w(), config.N_t(), 1);
        file.write_double(config.path_w(), tw.w, config.N_t(), 1);

        if (config.output_wavefunction() == 1) {
            std::string wavefunction_R = "wavefunction_R.bin";
            file.overwrite(wavefunction_R);
            file.write_header(wavefunction_R, config.N_t(), config.SAR_N_x());
            file.write_double(wavefunction_R, wavefunction.real(), config.N_t(), config.SAR_N_x());

            std::string wavefunction_I = "wavefunction_I.bin";
            file.overwrite(wavefunction_I);
            file.write_header(wavefunction_I, config.N_t(), config.SAR_N_x());
            file.write_double(wavefunction_I, wavefunction.imag(), config.N_t(), config.SAR_N_x());
        }
        if (config.output_electric_field() == 1) {
            file.overwrite(config.path_E());
            file.write_header(config.path_E(), config.N_t(), total_atoms);
            file.write_double(config.path_E(), pulse.E, config.N_t(), total_atoms);
        }

    }

    // Worker
    if (this_node != 0) {

        // Receive
        ArrayXXd E = ArrayXXd::Zero(tw.N_t, atoms_per_worker);
        MPI_Recv(E.data(), tw.N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);

        // Single atom calculations
        ArrayXXd acceleration = ArrayXXd::Zero(tw.N_t, atoms_per_worker);

        ArrayXXcd wavefunction;
        if (config.output_wavefunction() == 1) {
            wavefunction = ArrayXXcd::Zero(tw.N_t, config.SAR_N_x());
        } else {
            wavefunction = ArrayXXcd::Zero(0, 0);
        }

        Schrodinger_atom_1D atom(tw, config.alpha(), config.SAR_N_x(), config.SAR_x_min(), config.SAR_x_max(), config.output_wavefunction());
        for (int ii = 0; ii < atoms_per_worker; ii++) {

            acceleration.col(ii) = atom.get_acceleration(tw.N_t, tw.dt, E.col(ii));

        }

        if (config.output_wavefunction() == 1) {
            wavefunction = atom.wfn_output;
        }

        // Send
        bool send;
        MPI_Recv(&send, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Send(acceleration.data(), tw.N_t * atoms_per_worker, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);

        if (config.output_wavefunction() == 1 && this_node == 1) {
            MPI_Send(wavefunction.data(), config.SAR_N_x() * tw.N_t, MPI_DOUBLE_COMPLEX, 0, 1, MPI_COMM_WORLD);
        }

    }

    // Clean up
    MPI_Finalize();

    if (this_node == 0) {
        std::cout << "\n-------------------------------------------------------------------------------\n";
        std::cout << "XNLO successfully ran!\n";
        std::cout << "-------------------------------------------------------------------------------\n";
    }

}
