//
//  main.cpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  Test enviroment for UPPE codes.
//

#include <mpi.h>
#include "../../src/maths_textbook.hpp"
#include "../../src/physics_textbook.hpp"
#include <mkl.h>
#include "../../src/DHT.hpp"
#include "../../src/grid_rkr.hpp"
#include "../../src/grid_tw.hpp"
#include "laser_pulse.hpp"
#include "capillary_fibre.hpp"
#include "../../src/keldysh_gas.hpp"
#include "Eigen/Dense"
#include "../../src/IO.hpp"

#include "config_settings.hpp"

#include <iostream>
#include <string>

#include "../../XNLO/lib/XNLO.hpp"
#include "../../HHGP/lib/HHGP.hpp"

#include "../../HHGP/src/propagation.hpp"
//#include "../../HHGP/src/config_settings.hpp"

using namespace Eigen;

/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
Test enviroment for UPPE codes.
*/
int main(int argc, char** argv){

    // Get config file path passed in from command line with "-cf" flag
    std::string args[argc];
    std::string config_file_path;
    std::string config_XNLO_file_path = "../configFiles/config_XNLO.txt";
    std::string config_HHGP_file_path = "./config_HHGP_test.txt";
    for (int i = 0; i < argc; i++) {
      args[i] = argv[i];
    }
    for (int i = 0; i < argc; i++) {
      if (args[i] == "-cf") {
        config_file_path = argv[i+1];
      }
    }

    // MPI
    int this_process;
    int total_processes;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &this_process);

    if (this_process == 0) {
        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "                                  UPPE\n";
        std::cout << "-------------------------------------------------------------------------------\n";
    }

    //if (this_process == 0) {

    //--------------------------------------------------------------------------------------------//
    // 1. Program input
    //--------------------------------------------------------------------------------------------//

    // Input Settings and Parameters
    Config_Settings config;
    if(this_process == 0 && config_file_path.empty()) {
      std::cout << "Using default config file path " << config.path_config_file() << std::endl;
    } else {
        config.path_config_file_set(config_file_path);
        config.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
        if (this_process == 0) {
            std::cout << "Using config file path " << config.path_config_file() << std::endl;
        }
    }
    config.read_in(config.path_config_file(), false);
    config.check_paths(false);
    if (total_processes > 1) {
        config.n_m_set(total_processes-1);
        config.n_r_set(total_processes-1);
    }
    if (this_process == 0) {
        config.print();
    }

    Config_Settings config_XNLO;
    if (total_processes > 1) {
        if(this_process == 0 && config_XNLO_file_path.empty()) {
          std::cout << "Using default config file path " << config_XNLO.path_config_file() << std::endl;
        } else {
            config_XNLO.path_config_file_set(config_XNLO_file_path);
            config_XNLO.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) {
                std::cout << "Using config file path " << config_XNLO.path_config_file() << std::endl;
            }
        }
        config_XNLO.read_in(config_XNLO.path_config_file(), false);
        config_XNLO.check_paths(false);
        if (this_process == 0) {
            config_XNLO.print();
        }
    }

    Config_Settings config_HHGP;
    if (total_processes > 1) {
        if(this_process == 0 && config_HHGP_file_path.empty()) {
          std::cout << "Using default config file path " << config_HHGP.path_config_file() << std::endl;
        } else {
            config_HHGP.path_config_file_set(config_HHGP_file_path);
            config_HHGP.path_config_file_description_set("(std::string) Passed in by '-cf' argument");
            if (this_process == 0) {
                std::cout << "Using config file path " << config_HHGP.path_config_file() << std::endl;
            }
        }
    }
    config_HHGP.read_in(config_HHGP.path_config_file(), false);
    config_HHGP.check_paths(false);
    if (total_processes > 1) {
        config_HHGP.n_m_set(total_processes-1);
        config_HHGP.n_r_set(total_processes-1);
    }
    if (this_process == 0) {
        config_HHGP.print();
    }

    //--------------------------------------------------------------------------------------------//
    // 2. Constructors
    //--------------------------------------------------------------------------------------------//

    // General
    maths_textbook maths(config.path_input_j0());
    physics_textbook physics;

    MKL_LONG dimensions = 1;
    MKL_LONG length = config.n_t();
    double scale = 1.0 / config.n_t();
    DFTI_DESCRIPTOR_HANDLE ft;
    DftiCreateDescriptor(&ft, DFTI_DOUBLE, DFTI_COMPLEX, dimensions, length);
    DftiSetValue(ft, DFTI_BACKWARD_SCALE, scale);
    DftiCommitDescriptor(ft);

    DHT ht(config.n_r(), maths);

    // Grids
    grid_rkr rkr(config.n_r(), config.R(), config.n_m(), maths);
    grid_tw tw_driving(config.n_t(), config.T(), config.w_active_min(), config.w_active_max(), maths);
    //grid_tw tw_hhg(80000, config.T(), 1.88e16, 1.88e17, maths);

    // Physical
    laser_pulse laser_driving(config.p_av(), config.rep(), config.fwhm(), config.l_0(), config.ceo(), config.waist(), tw_driving, rkr, ft, ht, maths);
    //laser_pulse laser_hhg(0.0, config.rep(), config.fwhm(), config.l_0(), config.ceo(), config.waist(), tw_hhg, rkr, ft, ht, maths); //Make it a zero field - possibly with contructor overloading
    
    capillary_fibre capillary_driving(config.Z(), rkr, tw_driving, physics, maths);
    //capillary_fibre capillary_hhg(config.Z(), rkr, tw_hhg, physics, maths);
    
    //Dipole_moment dipole;

    keldysh_gas gas(config.press(), tw_driving, ft, maths);
    //keldysh_gas gas_hhg(config.press(), tw_hhg, ft, maths, dipole);

    //--------------------------------------------------------------------------------------------//
    // 3. Propagation
    //--------------------------------------------------------------------------------------------//
    // Main loop
    double dz = capillary_driving.Z / double(config.n_z());
    //double dz_hhg = capillary_hhg.Z / config.n_z();

    if (this_process == 0) {
        config.print(config.path_config_log());

        std::cout << "-------------------------------------------------------------------------------\n";
        std::cout << "Main Program:\n";
        std::cout << "-------------------------------------------------------------------------------\n";

        std::cout << "Laser p_pk: " << laser_driving.p_pk << std::endl;
        std::cout << "Laser E_pk: " << laser_driving.E_pk << std::endl;
    }
    
    IO file_prop_step;

    std::string ionisation_rate_test = "ionisation_rate_test.bin";

    //Fix this at some point
    ArrayXXd dipole = ArrayXXd::Zero(config_XNLO.n_t(), config.n_r());
    ArrayXXd w = ArrayXXd::Zero(config_XNLO.n_t(), config.n_r());
    ArrayXXd E = ArrayXXd::Zero(config_XNLO.n_t(), config.n_r());
    XNLO::Result tmp;// = ArrayXXd::Zero(config_XNLO.N_t(), config.n_r());
    //ArrayXd neutral_atoms = ArrayXd::Zero(config.n_r());
    ArrayXXd neutral_atoms = ArrayXXd::Zero(config.n_t(), config.n_r());

    ArrayXXcd A_w_active;

    //HHGP hhgp(config_HHGP.n_r());
    HHGP hhgp;

    ArrayXXcd hhg;
    ArrayXXcd hhg_new;
    ArrayXXcd hhg_source;
    ArrayXXcd hhg_previous;

    propagation prop;

    MPI_Barrier(MPI_COMM_WORLD);

        for (int ii = 1; ii < config.n_z() + 1; ii++) {
            if (this_process == 0) {
                std::cout << "Propagation step: " << ii << std::endl;
                laser_driving.propagate(dz, capillary_driving, gas);

                // Driving pulse:
                config.step_path(ii);
                file_prop_step.overwrite(config.path_A_w_R(), false);
                file_prop_step.write_header(config.path_A_w_R(), tw_driving.n_active, rkr.n_m, false);
                file_prop_step.write_double(config.path_A_w_R(), laser_driving.A_w_active.real(), tw_driving.n_active, rkr.n_m, false);
                
                file_prop_step.overwrite(config.path_A_w_I(), false);
                file_prop_step.write_header(config.path_A_w_I(), tw_driving.n_active, rkr.n_m, false);
                file_prop_step.write_double(config.path_A_w_I(), laser_driving.A_w_active.imag(), tw_driving.n_active, rkr.n_m, false);
                
                file_prop_step.overwrite(config.path_w_active(), false);
                file_prop_step.write_header(config.path_w_active(), tw_driving.n_active, 1, false);
                file_prop_step.write_double(config.path_w_active(), tw_driving.w_active, tw_driving.n_active, 1, false);

                file_prop_step.overwrite(config.path_electron_density(), false);
                file_prop_step.write_header(config.path_electron_density(), tw_driving.n_t, rkr.n_m, false);
                file_prop_step.write_double(config.path_electron_density(), laser_driving.electron_density, tw_driving.n_t, rkr.n_m, false);

                A_w_active = laser_driving.A_w_active;

                if (total_processes > 1) {
                    // Send
                    for (int j = 1; j < total_processes; j++) {
                        MPI_Send(laser_driving.A_w_active.real().data(),
                                 1242 * rkr.n_r, MPI_DOUBLE, j, j, MPI_COMM_WORLD);
                    }
                }
            } else {
                // Receive
                A_w_active = ArrayXXd::Zero(1242, rkr.n_r);
                MPI_Recv(A_w_active.real().data(), 1242 * rkr.n_r, MPI_DOUBLE, 0, this_process, MPI_COMM_WORLD, &status);
            }

            // Needs an MPI Send and Recv to collect up the acceleration from all processes
            // So don't want XNLO::XNLO to do the collecting itself but rather the logic in here to instead
            //    So Rank 0 holds all the information
            //    Ranks !0 have a block sent to them
            //    They do work and return the result
            //    So basically how the main of XNLO does it but split up between here and XNLO::XNLO
            //        So here manages splitting up the work, does the MPI_Send
            //        XNLO::XNLO has the MPI_Resvs and send back the results with the MPI_Send

            // Need rank 0 to send laser_driving.A_w_active to the other ranks as they done
            // calculate it

            int response_rate = 1;//config.n_z() / 10;
            if (total_processes > 1 && ((ii % response_rate == 0) || ii == 1)) {
                tmp = XNLO::XNLO(A_w_active, tw_driving.w_active);
            }

            if (this_process == 0 && total_processes > 1) {
                HHGP hhgp(config_HHGP.n_r());
                
                // Do we just take the electron density at the last time step or at all of them?
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < config.n_t(); i++) {
                        neutral_atoms.row(i).col(j) = (gas.atom_density(double(ii)*dz) - laser_driving.electron_density.row(i).col(j));
                    }
                }
std::cout << " main.foo 0.0" << std::endl;
                //ArrayXXcd hhg;
                //ArrayXXcd hhg_new;
                //ArrayXXcd hhg_source;
                //ArrayXXcd hhg_previous;
                double w_active_min_HHG = 1.2566371e+16;
                double w_active_max_HHG = 3.1415927e+17;
                int n_active_HHG = 0;
                ArrayXd w_active_HHG;
                w = tmp.w;
                int w_active_min_index_HHG = 0;
                while (w(w_active_min_index_HHG) < w_active_min_HHG)
                    w_active_min_index_HHG++;
                int count = 0;
                while (w(count) < w_active_max_HHG) {
                    count++;
                }
std::cout << " main.foo 0.1" << std::endl;
                n_active_HHG = count - w_active_min_index_HHG;
                w_active_HHG = w.col(0).segment(w_active_min_index_HHG, n_active_HHG);
                E = tmp.E;
                //hhgp.set_w_active(w_active_HHG);
std::cout << " main.foo 0.2" << std::endl;
                //std::cout << "dipole.rows(): " << dipole.rows() << ", dipole.cols(): " << dipole.cols() << std::endl;
                //std::cout << "tmp.acceleration.rows(): " << tmp.acceleration.rows() << ", tmp.acceleration.cols(): " << tmp.acceleration.cols() << std::endl;
                //std::cout << "neutral_atoms.rows(): " << neutral_atoms.rows() << ", neutral_atoms.cols(): " << neutral_atoms.cols() << std::endl;
                //std::cout << "w.rows(): " << w.rows() << ", w.cols(): " << w.cols() << std::endl;
                //std::cout << "w.row(0): " << w.row(0) << ", w.row(1000): " << w.row(1000) << std::endl;
                //std::cout << neutral_atoms.row(0).col(0) << std::endl;
std::cout << " main.foo 0.3" << std::endl;
                MKL_LONG dimensions_HHG = 1;
                MKL_LONG length_HHG = config_XNLO.n_t();
                double scale_HHG = 1.0 / config_XNLO.n_t();
                DFTI_DESCRIPTOR_HANDLE ft_HHG;
                DftiCreateDescriptor(&ft_HHG, DFTI_DOUBLE, DFTI_COMPLEX, dimensions_HHG, length_HHG);
                DftiSetValue(ft_HHG, DFTI_BACKWARD_SCALE, scale_HHG);
                DftiCommitDescriptor(ft_HHG);
std::cout << " main.foo 0.4" << std::endl;
                ArrayXd temp_linSpace = ArrayXd::LinSpaced(config_XNLO.n_t(), -500.0e-15, 500.0e-15);
                ArrayXd window = (1 - ((0.5 * maths.pi * temp_linSpace / 500e-15).sin()).pow(50));
                // Delete tmp after use to save ram
                dipole = tmp.acceleration;
std::cout << " main.foo 0.5" << std::endl;
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < config_XNLO.n_t(); i++) {
                        // Or is it (0, 0)?
                        // So, physically, what is going on?
                        // The laser pulse at each propagation step interacts with the gas.
                        // Some atoms become ionised by the laser, these atoms do not add to the
                        // HHG response
                        // The neutral atoms do add to the response
                        // The acceleration at this point is as a function of time, as is the neutral atoms
                        // these are both on different, independent time grid though, as...?
                        // So, we only want the number of neutral atoms at one point in time, but which point?
                        // At the start time there are a maximum number of neutral atoms
                        // At the end time there are a minimum
                        // So, laser comes through, ionises gas, causes HHG response
                        // if gas is ionised it can't add to HHG response
                        // as the atom has no electron to it
                        // So it needs to be the final number of neutral atoms?
                        // If so then can save a load of ram by only storing this value and not for all time steps
                        dipole.row(i).col(j) *= neutral_atoms.row(neutral_atoms.rows() - 1).col(j) * dz;

                        dipole.row(i).col(j) *= window.row(i);// / (w.row(i)).pow(2);

                        // How to do volume normalisation?
                        // Step increases to integrate over, or trapezoidal rule
                        // First is easier to implement, second is more accurate
                    }
                }
std::cout << " main.foo 0.6" << std::endl;
                // Apply forward spectral transform
                ArrayXXcd temp_1 = dipole.cast<std::complex<double> >();
                for (int i = 0; i < rkr.n_r; i++)
                    DftiComputeForward(ft_HHG, temp_1.col(i).data());

                // Why is there a Hankel transform here?
                // This would be putting it in terms of modes, whereas the
                // postprocessing code expects it in radial representation
                // and XNLO::acceleration is (N_t, N_r) / in radial rep.
                ArrayXXcd temp_2 = temp_1;
                //for (int ii = 0; ii < config_XNLO.n_t(); ii++)
                //    temp_2.row(ii) = ht.forward(temp_2.row(ii));

                hhg = temp_2.block(0, 0, n_active_HHG, rkr.n_r);
std::cout << " main.foo 0.7" << std::endl;
                for (int j = 0; j < rkr.n_r; j++) {
                    for (int i = 0; i < n_active_HHG; i++) {
                        hhg.row(i).col(j) /= (w_active_HHG.row(i)).pow(2);
                    }
                }
std::cout << " main.foo 0.8" << std::endl;
                // Propagate the harmonics here and loose the outputted source terms?
                // or, propagate them after this and keep the outputted source terms?
                // If I'm make HHGP a class then it'll keep variables between calss
                // otherwise I'll need to add statics in to stop so many copy/desctuctor calls
                // though would that even work in this case? (i think so, but not 100%)
                //
                // Something like this:

                double E_min = 10.0;
                //propagation prop(E_min, w_active_HHG, gas, rkr, ht);
                // May need a destructor at the end of the loop
                ArrayXd k_r = rkr.kr;
                double k_excluded = 0;
                // Put the divides on the other side to become multiplies to save
                // computational time
                // Do it like this in the future so less calculations:
                //    w_min = C * E_min^B
                //    while () {...}
                std::cout << "prop.foo 2: " << std::endl;
                while ((physics.h / (2.0*maths.pi) * w_active_HHG(k_excluded) * physics.E_eV) < (E_min)) {
                      k_excluded++;

                    //std::cout << "foobar: " << (physics.h / (2.0*maths.pi) * w_active(k_excluded-1) * physics.E_eV) << std::endl;
                }
                std::cout << "prop.foo 3: " << std::endl;
                int n_k = w_active_HHG.rows() - k_excluded;
                std::cout << "prop.foo 4: " << std::endl;
                std::cout << "k_excluded: " << k_excluded << ", n_k: " << n_k << ", w_active_HHG.rows()" << w_active_HHG.rows() << std::endl;
                Eigen::ArrayXd w_active_HHG_tmp = w_active_HHG;
                w_active_HHG = w_active_HHG_tmp.segment(k_excluded, n_k);

                //prop = propagation(E_min, w_active_HHG, gas, rkr, ht);
std::cout << "Foo1 " << std::endl;
                if (ii == 1) {
std::cout << "foo 1.1" << std::endl;
                    //These would have different sizes to the HHG outputted for other steps
                    // This needs to be corrected!
                    hhg_previous = hhg.block(k_excluded, 0, n_k, 119);
                    hhg_source = hhg.block(k_excluded, 0, n_k, 119);
                    //hhg_previous = prop.block(hhg);
                    //hhg_source = prop.block(hhg);
//std::cout << "Foo2 " << std::endl;
                } else {
std::cout << "w_active_min_index_HHG: " << w_active_min_index_HHG << std::endl;
                    double z = dz * double(ii);
                    hhg_source = hhg.block(k_excluded, 0, n_k, 119);
                    //hhg_source = prop.block(hhg);
std::cout << "Foo3 " << std::endl;
std::cout << "hhg_new.cols(): " << hhg_new.cols() << ", hhg_new.rows(): " << hhg_new.rows() << std::endl;
std::cout << "hhg_source.cols(): " << hhg_source.cols() << ", hhg_source.rows(): " << hhg_source.rows() << std::endl;
std::cout << "hhg_previous.cols(): " << hhg_previous.cols() << ", hhg_previous.rows(): " << hhg_previous.rows() << std::endl;
//std::cout << "hhg_previous.col(0): " << hhg_previous.col(0) << ", hhg_previous.row(0): " << hhg_previous.row(0) << std::endl;
std::cout << "hhg.cols(): " << hhg.cols() << ", hhg.rows(): " << hhg.rows() << std::endl;
std::cout << "w_active_HHG.rows(): " << w_active_HHG.rows() << std::endl;
                    Eigen::ArrayXXcd tmp;
                    tmp = hhgp.nearFieldStep(hhg_source, hhg_previous,
                                                 w_active_HHG,
                                                 z, dz);
                    hhg_new = tmp;
std::cout << "Foo4 " << std::endl;
std::cout << "hhg_new.cols(): " << hhg_new.cols() << ", hhg_new.rows(): " << hhg_new.rows() << std::endl;
std::cout << "hhg_source.cols(): " << hhg_source.cols() << ", hhg_source.rows(): " << hhg_source.rows() << std::endl;
std::cout << "hhg_previous.cols(): " << hhg_previous.cols() << ", hhg_previous.rows(): " << hhg_previous.rows() << std::endl;
std::cout << "hhg.cols(): " << hhg.cols() << ", hhg.rows(): " << hhg.rows() << std::endl;
//                    hhg_previous = hhg_new;
//                    hhg = hhg_new;
std::cout << "Foo5 " << std::endl;
                }
std::cout << "Foo6 " << std::endl;
                // Explaination of the above:
                // -At the first step we just want the source term as nothing from any previous steps is
                //  propagated to the first position since nothing before
                // -Keep the hhg term here as the sources from the previous step
                // -At the next step, take the HHG as teh current source term
                //  and use previous as the sources from the previous steps
                //  propagate previous to current and add source to it
                // -This is now also the previous term for the next step
                // -Repeat
                //
                // BUT REMEMBER THAT THE SIZE OF HHG WILL CHANGE AS HHGP TAKES THE USABLE SEGMENT!
                // so the outputting below will be be wrong as different n_active
                // but outputting hhg_new using .rows() and .cols() would cause no issues

                file_prop_step.overwrite(config.path_HHG_R(), false);
                file_prop_step.write_header(config.path_HHG_R(), hhg.cols(), hhg.rows(), false);
                file_prop_step.write_double(config.path_HHG_R(), hhg.real(), hhg.cols(), hhg.rows(), false);

                file_prop_step.overwrite(config.path_HHG_I(), false);
                file_prop_step.write_header(config.path_HHG_I(), hhg.cols(), hhg.rows(), false);
                file_prop_step.write_double(config.path_HHG_I(), hhg.imag(), hhg.cols(), hhg.rows(), false);

                file_prop_step.overwrite(config.path_HHG_w(), false);
                file_prop_step.write_header(config.path_HHG_w(), w_active_HHG.rows(), w_active_HHG.cols(), false);
                file_prop_step.write_double(config.path_HHG_w(), w_active_HHG, w_active_HHG.rows(), w_active_HHG.cols(), false);

                file_prop_step.overwrite(config.path_HHG_E(), false);
                file_prop_step.write_header(config.path_HHG_E(), E.rows(), E.cols(), false);
                file_prop_step.write_double(config.path_HHG_E(), E, E.rows(), E.cols(), false);

std::cout << "Foo7 " << std::endl;
            }
        }
        std::cout << "-------------------------------------------------------------------------------\n";

        if (this_process == 0) {
            // Output
            IO file;

            file.overwrite(config.path_A_w_R());
            file.write_header(config.path_A_w_R(), tw_driving.n_active, rkr.n_m);
            file.write_double(config.path_A_w_R(), laser_driving.A_w_active.real(), tw_driving.n_active, rkr.n_m);
            
            file.overwrite(config.path_A_w_I());
            file.write_header(config.path_A_w_I(), tw_driving.n_active, rkr.n_m);
            file.write_double(config.path_A_w_I(), laser_driving.A_w_active.imag(), tw_driving.n_active, rkr.n_m);
            
            file.overwrite(config.path_w_active());
            file.write_header(config.path_w_active(), tw_driving.n_active, 1);
            file.write_double(config.path_w_active(), tw_driving.w_active, tw_driving.n_active, 1);
        }

        // Clean up
        DftiFreeDescriptor(&ft);
        MPI_Finalize();

        std::cout << "\n-------------------------------------------------------------------------------\n";
        std::cout << "UPPE successfully ran!\n";
        std::cout << "-------------------------------------------------------------------------------\n";
}
