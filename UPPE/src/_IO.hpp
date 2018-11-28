//
//  IO.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "IO" objects enable reading/writing of binary files to/from Eigen arrays.
//

#ifndef __IO_HPP_INCLUDED__
#define __IO_HPP_INCLUDED__

#include "Eigen/Dense"

#include <string>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
/*!
Originally created by Patrick Anderson.
Modified by Samuel Senior on 10/03/2017.
"IO" objects enable reading/writing of binary files to/from Eigen arrays.
*/
class IO {

public:
    // Data

    // Functions
    IO();
    Array<unsigned short, Dynamic, Dynamic> read_uint16(const char* path_, int N_row_, int N_col_);
    ArrayXXi read_int(const char* path_, int N_row_, int N_col_);
    ArrayXXd read_double(const std::string path_, int N_row_, int N_col_);
    ArrayXXd read_ascii_double(const std::string path, int N_row_, int N_col_);
    void write_double(const std::string path_, ArrayXXd input_, int N_row_, int N_col_, bool print=true);
    void write_header(const std::string path_, int N_row_, int N_col_, bool print=true);
   	void overwrite(const std::string path, bool print=true);
   	void write_ascii_double(ArrayXd data, std::string path, bool print=true);
};

#endif
