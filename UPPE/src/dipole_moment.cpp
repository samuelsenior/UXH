//
//  dipole_moment.hpp
//
//  Created by Samuel Senior on 10/03/2017.
//  "dipole_moment" wraps around the dipole moment calculations and output files.
//

#include "dipole_moment.hpp"
#include "Eigen/Dense"

#include "IO.hpp"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
Dipole_moment::Dipole_moment() {
    std::string path = "../../XNLO/output/008_dipole.bin";
    IO dipole_file;
    dipole_data = dipole_file.read_double(path, 2621440, 2);
}

void Dipole_moment::update_dipole_moment() {
	//not yet imlemented
}

ArrayXXd Dipole_moment::get_moment() {
	//std::string path = "../../XNLO/output/008_dipole.bin";
	//IO dipole_file;
	//dipole_data = dipole_file.read_double(path, 2621440, 2);
	return dipole_data;
}
