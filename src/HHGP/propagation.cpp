#include "propagation.hpp"
//#include "config_settings.hpp"
#include "../gas/keldysh_gas.hpp"
#include "../grid/grid_rkr.hpp"
#include "../grid/grid_tw.hpp"
#include "../DHT/DHT.hpp"
#include "../physics/physics_textbook.hpp"
#include "../maths/maths_textbook.hpp"
#include "../../Eigen/Dense"

#include <iostream>

// Remove this if IO for E, f1, f2 goes into another class etc
#include "../IO/IO.hpp"

#include <complex>

//namespace HHGP {

//using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*!
    Constructor
*/
propagation::propagation() {}
propagation::propagation(double E_min_,
                         double E_max_,
                         double Z_max_,
                         Eigen::ArrayXd w_active_,
                         keldysh_gas& gas_,
                         grid_rkr& rkr_,
                         physics_textbook& physics_,
                         maths_textbook& maths_,
                         DHT& ht_,
                         bool print_)//,
                         //HH::Config_Settings config_)
                        :
                         E_min(E_min_),
                         E_max(E_max_),
                         Z_max(Z_max_),
                         w_active_tmp(w_active_),
                         gas(gas_),
                         rkr(rkr_),
                         physics(physics_),
                         maths(maths_),
                         ht(ht_),
                         print(print_) {//,
                         //config(config_) {

      z = 0.0;
      k_r = rkr.kr;
      k_excluded = 0;

      to_end_only = true;
      // Put the divides on the other side to become multiplies to save
      // computational time
      // Do it like this in the future so less calculations:
      //    w_min = C * E_min^B
      //    while () {...}
      while ((physics.h / (2.0*maths.pi) * w_active_tmp(k_excluded) * physics.E_eV) < (E_min)) {
            k_excluded++;
if (print) std::cout << "k_excluded: " << k_excluded << ", w_active_tmp(k_excluded): " << w_active_tmp(k_excluded) << ", E: " << (physics.h / (2.0*maths.pi) * w_active_tmp(k_excluded) * physics.E_eV) << std::endl;
      }
      int count = 0;
      while ((physics.h / (2.0*maths.pi) * w_active_tmp(count) * physics.E_eV) < (E_max)) {
            count++;
      }
if (print) std::cout << "count: " << count << std::endl;

      n_k = count-k_excluded;//w_active_tmp.rows() - k_excluded;
      w_active = w_active_tmp.segment(k_excluded, n_k);
      k = std::complex<double>(1, 0) * w_active / physics.c;
      A_w_r = Eigen::ArrayXXcd::Zero(n_k, rkr.n_r);

      refractiveIndex = Eigen::ArrayXcd::Zero(n_k);
      Eigen::ArrayXd E_grid = w_active * physics.h / (2.0*maths.pi) * 6.241509e18; // In units of eV!!

      // Put these in their own class or in the physics class?
      // Also, how to know how much to read in?
      int E_length = 506;  // Length of Ar.nff file
      int ASF_file_cols = 3;  // Number of cols in ASF file
      E_f1_f2_data_path = "../../input/AtomicScatteringFactors/ar.nff";
      IO ASF_file;
      E_f1_f2_data = ASF_file.read_ascii_double(E_f1_f2_data_path, E_length, ASF_file_cols);
      E = E_f1_f2_data.col(0);
      f1 = E_f1_f2_data.col(1);
      f2 = E_f1_f2_data.col(2);

      // Read in E, f_1, and f_2 from the data file for Ar
      // Interpolate each onto a new grid with spacing from E_grid
      // Calculate refractive index
      int splineOrder = 4;
      Eigen::ArrayXd test_f1 = Eigen::ArrayXd::Zero(n_k);
      f1 = maths.interp1D(E, E_length, f1, E_grid, n_k, splineOrder);
      Eigen::ArrayXd test_f2 = Eigen::ArrayXd::Zero(n_k);
      f2 = maths.interp1D(E, E_length, f2, E_grid, n_k, splineOrder);

      ArrayXd rho = physics.r_0 * (2.0*maths.pi*physics.c / w_active).pow(2.0) / (2.0*maths.pi);

      lamda = 2.0*maths.pi / k;

      // Split up the calculation as we don't need to do the full calculation at every
      // propagation step, only the position dependent atom density bit
      //refractiveIndex = 1 - (rho_0 * physics.r_0 * lamda.pow(2.0))/(2.0 * maths.pi) * (test_f1 + std::complex<double>(0.0, 1.0) * test_f2);//1 - rho * (test_f1 + std::complex<double>(0.0, 1.0) * test_f2);
      
      refractiveIndex = (physics.r_0 * (lamda.real()).pow(2.0))/(2.0 * maths.pi) * (f1 + std::complex<double>(0.0, 1.0) * f2);

      //config.step_path(i);
      IO RI_output;
      RI_output.overwrite("../output/f1.bin", false);
      RI_output.write_header("../output/f1.bin", n_k, 1, false);
      RI_output.write_double("../output/f1.bin", refractiveIndex.real(), n_k, 1, false);

      RI_output.overwrite("../output/f2.bin", false);
      RI_output.write_header("../output/f2.bin", n_k, 1, false);
      RI_output.write_double("../output/f2.bin", refractiveIndex.imag(), n_k, 1, false);

      A_w_kr = Eigen::ArrayXcd::Zero(rkr.n_r);

      n_k_squared_tmp = Eigen::ArrayXcd::Ones(rkr.n_r);
      ones = Eigen::ArrayXcd::Ones(rkr.n_r);
}

Eigen::ArrayXd propagation::segment(Eigen::ArrayXd K) {
      return K.segment(k_excluded, n_k);
}

Eigen::ArrayXXcd propagation::block(Eigen::ArrayXXcd A_w_r_) {
      return A_w_r_.block(k_excluded, 0, n_k, rkr.n_r);
}

std::complex<double> propagation::n(int i) {
      // Use 1DInterp to make a grid of n, for the grid of k.
      // k is not linearly spaced so will need to use a grid that is
      // linearly spaced and make sure that n is also in the same form.
      //
      // Also need to remember to include gas pressure profile.
      //
      // Rename to refractiveIndex or something similar

// Don't know if atom_density should be complex or double...?
      //bool to_end_only = true;
      double z_max_prime = Z_max - (gas.inlet_2 + gas.transitionLength);
      double z_prime = z - (gas.inlet_2 + gas.transitionLength);
      if (to_end_only == true) {
//std::cout << "z: " << z << ", " << (1.0 - (0.8*gas.atom_density_max)/(Z_max - z) * std::pow(z_max_prime - z_prime, 2.0) / (2*z_max_prime)) << std::endl;
//std::cout << "z: " << z << ", " << (0.8*gas.atom_density_max)/(Z_max - z) * std::pow(z_max_prime - z_prime, 2.0) / (2.0*z_max_prime) << std::endl;
//std::cout << "gas.atom_density_max: " << gas.atom_density_max << std::endl;
//std::cout << "gas.inlet_2: " << gas.inlet_2 << std::endl;
//std::cout << "gas.transitionLength: " << gas.transitionLength << std::endl;
//std::cout << "Z_max: " << Z_max << std::endl;
//std::cout << "z_max_prime: " << z_max_prime << std::endl;
//std::cout << "z_prime: " << z_prime << std::endl;
//std::cout << "N(z): " << gas.atom_density(z) << std::endl;
//std::cout << "-----" << std::endl;

        double N_tot = 0.0;

        // Section 1
        if (z <= gas.inlet_1 - gas.transitionLength) {
          if (z > 0.0) {
             N_tot += (0.8*gas.atom_density_max / (2.0*(gas.inlet_1 - gas.transitionLength))) * (std::pow(gas.inlet_1 - gas.transitionLength, 2.0) - std::pow(z, 2.0));
          } else {
             N_tot += (0.8*gas.atom_density_max / (2.0*(gas.inlet_1 - gas.transitionLength))) * (std::pow(gas.inlet_1 - gas.transitionLength, 2.0) - std::pow(0.0, 2.0));
          }
        }
        // Section 2
        double z_2 = 0.0;
        double z_3 = gas.inlet_1;
        if (z <= z_3) {
          if (z > gas.inlet_1 - gas.transitionLength) {
             z_2 = z;
          } else {
             z_2 = gas.inlet_1 - gas.transitionLength;
          }
          N_tot += (gas.atom_density_max / gas.transitionLength) * 0.1*(std::pow(z_3, 2.0) - (0.2*gas.inlet_1 - gas.transitionLength)*z_3 - 0.1*std::pow(z_2, 2.0) + (2.0*gas.inlet_1 - gas.transitionLength)*z_2);
        }
        // Section 3
        double z_4 = gas.inlet_2;
        if (z <= z_4) {
           if (z > gas.inlet_1) {
              z_4 = z;
           } else {
              z_4 = gas.inlet_1 - gas.transitionLength;
           }
          N_tot += gas.atom_density_max * (z_4 - z_3);
        }
        // Section 4
        double z_5 = gas.inlet_2 + gas.transitionLength;
        if (z <= z_5) {
           if (z > gas.inlet_2) {
              z_4 = z;
           } else {
              z_4 = gas.inlet_2;
           }
           N_tot += gas.atom_density_max / gas.transitionLength * (0.1*std::pow(z_4, 2.0) - z_4*(gas.transitionLength + 0.2*gas.inlet_2) - 0.1*std::pow(z_5, 2.0) + z_5*(gas.transitionLength + 0.2*gas.inlet_2));
        }
        // Section 5
        if (z <= Z_max) {
           if ( z > (gas.inlet_2 + gas.transitionLength)) {
              N_tot += 0.4*gas.atom_density_max / (Z_max - (gas.inlet_2 + gas.transitionLength)) * (2.0*Z_max*(Z_max - z) - std::pow(Z_max, 2.0) + std::pow(z, 2.0));
           } else {
              N_tot += 0.4*gas.atom_density_max / (Z_max - (gas.inlet_2 + gas.transitionLength)) * (2.0*Z_max*(Z_max - (gas.inlet_2 + gas.transitionLength)) - std::pow(Z_max, 2.0) + std::pow((gas.inlet_2 + gas.transitionLength), 2.0));
           }
        }
        // Return the average value
        // Maybe make this an if statement so can return total if needed as well???
        //return N_tot / (Z_max - z);
        return (1.0 - (N_tot / (Z_max - z)) * refractiveIndex(i));
        //return (1.0 - ((0.8*gas.atom_density_max)/(Z_max - z) * std::pow(z_max_prime - z_prime, 2.0) / (2*z_max_prime)) * refractiveIndex(i));
      } else {
        return (1.0 - gas.atom_density(z) * refractiveIndex(i));
      }
}

/*!
      Angular Spectrum Method (ASM) for propagating in the near-field, in the gas-filled capillary.
      [Need to add citation!]
      For circularly-symmetric geometry:
      \f[
        E(z, r) = \mathcal{H}^{-1}\left(\mathcal{H}(E(0, r)) \times \exp(iz\sqrt{k^2 - k_r^2})\right).
      \f]
*/
void propagation::nearFieldPropagationStep(double delta_z, Eigen::ArrayXXcd A_w_r_) {
  // k is from w_active, in terms of t this would be linearly spaced

  // Need to discount the k's that fall below the minimum energy
  // read in from the E_ev, f1, f2, data file for Ar.
  // I.e. similar count method to that for n_active
  // and take block of k's from count to end
  //    while h*w_active[count] / (2 * pi) * E_ev < 10.0eV
  //       count++;
  //    k = w_active.block(count, end) / c;
  // Need to do this also for the field, as E[i<count] have energies
  // below the lower limit from the data file and therefore can't be
  // propagated in the gas. So (e.g.):
  //    E = E.block(count, end);
  // Also need a new n_active, not the one passed in from the prop sim
  // to account for discounted lower energy frequencies.

  // Can an Eigen array be reesized like this?
  // Or can main (etc) call a propagation class function to resize
  // it's A array to make it valid for k_r's etc?
  //    But it would have to do this at each propagation step anyway...
  // Do a sanatise or block function from the calling loop rather than this here
  // as source needs to be added to this afterwards and there would be different
  // numbers of k

  // For each active frequency, propagate that frequency a step in z
//std::cout << "dz: " << dz << ", z: " << z << std::endl;

  if (print) std::cout << "z: " << z << ", delta_z: " << delta_z << ", Z_max - z: " << Z_max - z << std::endl;
  for(int i = 0; i < n_k; i++) {
      n_k_squared_tmp = ones * std::pow(n(i)*k(i), 2.0);
      // Transform from radial representation to frequency representation
      A_w_kr = ht.forward(A_w_r_.row(i));
      // For each radial point (/radial frequency), apply the propagator to it
      //ArrayXcd testing = A_w_kr;
      A_w_kr *= (std::complex<double>(0, -1) * delta_z * (n_k_squared_tmp - k_r.pow(2.0)).pow(0.5)).exp();
      //for(int j = 0; j < rkr.n_r; j++) {
      //    A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * delta_z * std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5));
//          testing(j) *= std::exp(std::complex<double>(0, -1) * delta_z * std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5));
     // }
//std::cout << "---Testing: " << (A_w_kr - testing) << std::endl;
      // Backtransform to put back into radial representation
std::cout << "prop min propagator: " << ((std::complex<double>(0, -1) * delta_z * (n_k_squared_tmp - k_r.pow(2.0)).pow(0.5)).exp()).minCoeff;
std::cout << "prop max propagator: " << ((std::complex<double>(0, -1) * delta_z * (n_k_squared_tmp - k_r.pow(2.0)).pow(0.5)).exp()).maxCoeff;
      A_w_r.row(i) = ht.backward(A_w_kr);
  }
}

/*!
      Fraunhofer diffraction for propagating into the far-field.
*/
void propagation::farFieldPropagation() {

}

//} // HHGP namespace
