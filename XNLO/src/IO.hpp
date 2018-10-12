//
//  IO.hpp
//
//  Modified by Patrick Anderson on 09/05/2015.
//  
//

#ifndef __IO_HPP_INCLUDED__
#define __IO_HPP_INCLUDED__

#include "../Eigen/Dense"
#include <string>

using namespace Eigen;

namespace XNLO {

//------------------------------------------------------------------------------------------------//
//  Class definition
/*!
	Modified by Patrick Anderson on 09/05/2015.
*/
class IO {

public:

    //Data
    std::string binary_format;    /*!< The binary format name. */
    int binary_format_version;    /*!< The binary format version number. */
    int binary_format_subversion; /*!< The binary format subversion number. */
    int binary_format_len;        /*!< The length of the binary format name. */
    int data_size;                /*!< The total number of elements of the data in the two dimensional array. */
    int double_size;              /*!< The size of a double in bytes. */
    int N_row_;                   /*!< The number of rows to the two dimensional Eigen array. */
    int N_col_;                   /*!< The number of columns to the two dimensional Eigen array. */
    int header_size;              /*!< The size of the header in bytes. */

    //Functions
    IO();
    void read_header(const std::string path, bool print=true);
    ArrayXXd read_double(const std::string path, int N_row_, int N_col_, bool print=true);
    ArrayXXd read_double(const std::string path, bool skip_header=true, bool print=true);
    void write_ascii_double(std::string path, ArrayXXd data);
    void write_double(const std::string path, ArrayXXd output, int N_row_, int N_col_);
    void write_header(const std::string path_, int N_row_, int N_col_);
    void overwrite(const std::string path);

};

}

#endif
