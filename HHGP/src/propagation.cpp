#include "propagation.hpp"
#include "../../src/keldysh_gas.hpp"
#include "../../src/grid_rkr.hpp"
#include "../../src/grid_tw.hpp"
#include "../../src/DHT.hpp"
#include "../../src/physics_textbook.hpp"
#include "../../src/maths_textbook.hpp"
#include "../../Eigen/Dense"

#include <iostream>

// Remove this if IO for E, f1, f2 goes into another class etc
#include "../../src/IO.hpp"

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
                         Eigen::ArrayXd w_active_,
                         keldysh_gas& gas_,
                         grid_rkr& rkr_,
                         physics_textbook& physics_,
                         maths_textbook& maths_,
                         DHT& ht_)
                        :
                         E_min(E_min_),
                         E_max(E_max_),
                         w_active_tmp(w_active_),
                         gas(gas_),
                         rkr(rkr_),
                         physics(physics_),
                         maths(maths_),
                         ht(ht_) {

      z = 0.0;
      k_r = rkr.kr;
      k_excluded = 0;
      // Put the divides on the other side to become multiplies to save
      // computational time
      // Do it like this in the future so less calculations:
      //    w_min = C * E_min^B
      //    while () {...}
      while ((physics.h / (2.0*maths.pi) * w_active_tmp(k_excluded) * physics.E_eV) < (E_min)) {
            k_excluded++;
std::cout << "k_excluded: " << k_excluded << ", w_active_tmp(k_excluded): " << w_active_tmp(k_excluded) << ", E: " << (physics.h / (2.0*maths.pi) * w_active_tmp(k_excluded) * physics.E_eV) << std::endl;
      }
      int count = 0;
      while ((physics.h / (2.0*maths.pi) * w_active_tmp(count) * physics.E_eV) < (E_max)) {
            count++;
      }
std::cout << "count: " << count << std::endl;

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
      E_f1_f2_data_path = "../../AtomicScatteringFactors/ar.nff";
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
      
      refractiveIndex = (physics.r_0 * lamda.pow(2.0))/(2.0 * maths.pi) * (f1 + std::complex<double>(0.0, 1.0) * f2);

      //config.step_path(i);
      IO RI_output;
      RI_output.overwrite("../output/f1.bin", false);
      RI_output.write_header("../output/f1.bin", n_k, 1, false);
      RI_output.write_double("../output/f1.bin", refractiveIndex.real(), n_k, 1, false);

      RI_output.overwrite("../output/f2.bin", false);
      RI_output.write_header("../output/f2.bin", n_k, 1, false);
      RI_output.write_double("../output/f2.bin", refractiveIndex.imag(), n_k, 1, false);

      A_w_kr = Eigen::ArrayXcd::Zero(rkr.n_r);

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
      return (1.0 - gas.atom_density(z) * refractiveIndex(i));
}

/*!
      Angular Spectrum Method (ASM) for propagating in the near-field, in the gas-filled capillary.
      [Need to add citation!]
      For circularly-symmetric geometry:
      \f[
        E(z, r) = \mathcal{H}^{-1}\left(\mathcal{H}(E(0, r)) \times \exp(iz\sqrt{k^2 - k_r^2})\right).
      \f]
*/
void propagation::nearFieldPropagationStep(double dz, Eigen::ArrayXXcd A_w_r_) {
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
std::cout << "dz: " << dz << ", z: " << z << std::endl;

      for(int i = 0; i < n_k; i++) {
            // Transform from radial representation to frequency representation
            A_w_kr = ht.forward(A_w_r_.row(i));
            // For each radial point (/radial frequency), apply the propagator to it
            for(int j = 0; j < rkr.n_r; j++) {
//std::cout << "k.rows(): " << k.rows() << ", k.cols(): " << k.cols() << std::endl;
                  //
                  //A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * dz * std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5));
                  A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * dz * std::pow(n(i)*n(i)*k(i)*k(i) - k_r(j)*k_r(j), 0.5));
//                  A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * dz * std::pow(k(i)*k(i) - k_r(j)*k_r(j), 0.5));
                  //A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * dz * std::pow(std::pow(k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5));

//std::cout << "std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5)): " << std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5) << std::endl;
            }
            // Backtransform to put back into radial representation
            A_w_r.row(i) = ht.backward(A_w_kr);
      }
}

/*!
      Fraunhofer diffraction for propagating into the far-field.
*/
void propagation::farFieldPropagation() {

}

//} // HHGP namespace
