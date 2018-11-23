#include "propagation.hpp"
#include "keldysh_gas.hpp"
#include "grid_rkr.hpp"
#include "grid_tw.hpp"
#include "DHT.hpp"
#include "physics_textbook.hpp"
#include "maths_textbook.hpp"
#include "Eigen/Dense"

#include <iostream>

// Remove this if IO for E, f1, f2 goes into another class etc
#include "IO.hpp"

#include <complex>

namespace HHGP {

//using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*!
    Constructor
*/
propagation::propagation() {}
propagation::propagation(double E_min,
                         Eigen::ArrayXd w_active,
                         keldysh_gas gas,
                         grid_rkr rkr,
                         DHT ht)
                        :
                         E_min(E_min),
                         w_active_tmp(w_active),
                         gas(gas),
                         rkr(rkr),
                         ht(ht) {

      z = 0.0;
      k_r = rkr.kr;
      k_excluded = 0;
      // Put the divides on the other side to become multiplies to save
      // computational time
      // Do it like this in the future so less calculations:
      //    w_min = C * E_min^B
      //    while () {...}
      while ((physics.h / (2.0*maths.pi) * w_active(k_excluded) * physics.E_eV) < (E_min)) {
            k_excluded++;

            //std::cout << "foobar: " << (physics.h / (2.0*maths.pi) * w_active(k_excluded-1) * physics.E_eV) << std::endl;
      }
      n_k = w_active.rows() - k_excluded;
      w_active = w_active_tmp.segment(k_excluded, n_k);

      k = std::complex<double>(1, 0) * w_active / physics.c;
      A_w_r = Eigen::ArrayXXcd::Zero(n_k, rkr.n_r);

      refractiveIndex = Eigen::ArrayXcd::Zero(n_k);
      Eigen::ArrayXd E_grid = w_active * physics.h / (2.0*maths.pi) * 6.241509e18; // In units of eV!!

      // Put these in their own class or in the physics class?
      // Also, how to know how much to read in?
      std::string E_f1_f2_data_path = "../../AtomicScatteringFactors/ar.nff";
      IO file;
      Eigen::ArrayXXd E_f1_f2_data = file.read_ascii_double(E_f1_f2_data_path, 506, 3);
      Eigen::ArrayXd E = E_f1_f2_data.col(0);
      Eigen::ArrayXd f1 = E_f1_f2_data.col(1);
      Eigen::ArrayXd f2 = E_f1_f2_data.col(2);

      // Read in E, f_1, and f_2 from the data file for Ar
      // Interpolate each onto a new grid with spacing from E_grid
      // Calculate refractive index
      int splineOrder = 4;
      int E_length = 506;  // Length of Ar.nff file
      Eigen::ArrayXd test_f1 = Eigen::ArrayXd::Zero(n_k);
      f1 = maths.interp1D(E, E_length, f1, E_grid, n_k, splineOrder);
      Eigen::ArrayXd test_f2 = Eigen::ArrayXd::Zero(n_k);
      f2 = maths.interp1D(E, E_length, f2, E_grid, n_k, splineOrder);

      //double pressure = 50e-3;
      //double _atom_density_max = pressure * 1.0e5 / (physics.k_B * 300.0);
      //double rho_0 = _atom_density_max;
      //ArrayXd rho = rho_0 * physics.r_0 * (2.0*maths.pi*physics.c / w_active).pow(2.0) / (2.0*maths.pi);
      ArrayXd rho = physics.r_0 * (2.0*maths.pi*physics.c / w_active).pow(2.0) / (2.0*maths.pi);

      //ArrayXcd lamda = 2.0*maths.pi / k;
      lamda = 2.0*maths.pi / k;

      // Split up the calculation as we don't need to do the full calculation at every
      // propagation step, only the position dependent atom density bit
      //refractiveIndex = 1 - (rho_0 * physics.r_0 * lamda.pow(2.0))/(2.0 * maths.pi) * (test_f1 + std::complex<double>(0.0, 1.0) * test_f2);//1 - rho * (test_f1 + std::complex<double>(0.0, 1.0) * test_f2);
      refractiveIndex = (physics.r_0 * lamda.pow(2.0))/(2.0 * maths.pi) * (f1 + std::complex<double>(0.0, 1.0) * f2);

      //config.step_path(i);
      file.overwrite("../output/f1.bin", false);
      file.write_header("../output/f1.bin", n_k, 1, false);
      file.write_double("../output/f1.bin", refractiveIndex.real(), n_k, 1, false);

      file.overwrite("../output/f2.bin", false);
      file.write_header("../output/f2.bin", n_k, 1, false);
      file.write_double("../output/f2.bin", refractiveIndex.imag(), n_k, 1, false);



      Eigen::ArrayXcd A_w_kr = Eigen::ArrayXcd::Zero(rkr.n_r);
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

//std::cout << "n: " << (1.0 - gas.atom_density(z) * refractiveIndex(i)) << std::endl;
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
      //double k_tmp = 1.0;
      //Only need this done once and not at every propagation step
      //Eigen::ArrayXd k = w_active / c;

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

      //Only need this done once and not at every propagation step
      //Eigen::ArrayXcd n = refractiveIndex(k);


      // Can an Eigen array be reesized like this?
      // Or can main (etc) call a propagation class function to resize
      // it's A array to make it valid for k_r's etc?
      //    But it would have to do this at each propagation step anyway...
      // Do a sanatise or block function from the calling loop rather than this here
      // as source needs to be added to this afterwards and there would be different
      // numbers of k
      //A_w_r_ = A_w_r_.block(k_excluded, 0, n_k, rkr.n_r);

      // z being the position we're pospagating to (I think?)
      //z += dz;
//std::cout << "z: " << z << std::endl;
//std::cout << "atom_density(" << z << "): " << gas.atom_density(z) << std::endl;

      // For each active frequency, propagate that frequency a step in z

      //A_w_kr = Eigen::ArrayXcd::Zero(rkr.n_r);
      for(int i = 0; i < n_k; i++) {
            // Transform from radial representation to frequency representation
            A_w_kr = ht.forward(A_w_r_.row(i));
            // For each radial point (/radial frequency), apply the propagator to it
            for(int j = 0; j < rkr.n_r; j++) {
                  //
                  A_w_kr(j) *= std::exp(std::complex<double>(0, -1) * dz * std::pow(std::pow(n(i)*k(i), 2.0) - std::pow(k_r(j), 2.0), 0.5));
            }
            // Backtransform to put back into radial representation
            A_w_r.row(i) = ht.backward(A_w_kr);
      }
//std::cout << "lamda(3000): " << 1.0e9 * 2.0*maths.pi / k.row(3000) << "nm, A_w_r(3000): "<< A_w_r.row(3000).col(0) << std::endl;
}

/*!
      Fraunhofer diffraction for propagating into the far-field.
*/
void propagation::farFieldPropagation() {

}

} // HHGP namespace