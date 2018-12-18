//
//  IO.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "IO" objects enable reading/writing of binary files to/from Eigen arrays.
//

#ifndef __IO_HPP_INCLUDED__
#define __IO_HPP_INCLUDED__

#include <iostream>

#include "../Eigen/Dense"

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
    std::string binary_format;    /*!< The binary format name. */
    int binary_format_version;    /*!< The binary format version number. */
    int binary_format_subversion; /*!< The binary format subversion number. */
    int binary_format_len;        /*!< The length of the binary format name. */
    int data_size;                /*!< The total number of elements of the data in the two dimensional array. */
    int double_size;              /*!< The size of a double in bytes. */
    int N_row_;                   /*!< The number of rows to the two dimensional Eigen array. */
    int N_col_;                   /*!< The number of columns to the two dimensional Eigen array. */
    int header_size;              /*!< The size of the header in bytes. */

    // Functions
    IO();
    void read_header(const std::string path, bool print=true);
    Array<unsigned short, Dynamic, Dynamic> read_uint16(const char* path_, int N_row_, int N_col_);
    ArrayXXi read_int(const char* path_, int N_row_, int N_col_);
    ArrayXXd read_double(const std::string path, int N_row_, int N_col_, bool print=true);
    ArrayXXd read_double(const std::string path, bool skip_header=true, bool print=true);
    ArrayXXd read_ascii_double(const std::string path, int N_row_, int N_col_, bool header=true);
    void write_double(const std::string path_, ArrayXXd input_, int N_row_, int N_col_, bool print=true);
    void write_header(const std::string path_, int N_row_, int N_col_, bool print=true);
   	void overwrite(const std::string path, bool print=true);
   	void write_ascii_double(ArrayXd data, std::string path, bool print=true);
    template<typename T> void write(T output, std::string path, bool print=true);
};

// See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf.
template <typename...>
using void_t = void;
// Primary template handles all types not supporting the operation.
template <typename, template <typename> class, typename = void_t<>>
struct detect : std::false_type {};
// Specialisation recognises/validates only types supporting the archetype.
template <typename T, template <typename> class Op>
struct detect<T, Op, void_t<Op<T>>> : std::true_type {};
// Variable part, testing for specific functions that we're looking for
// 1. rows() - to give the number of rows in the array
template <typename T>
using rows_t = decltype(std::declval<T>().rows());
template <typename T>
using has_rows = detect<T, rows_t>;
// 2. cols() - to give the number of cols in the array
template <typename T>
using cols_t = decltype(std::declval<T>().cols());
template <typename T>
using has_cols = detect<T, cols_t>;

template<typename T>
void IO::write(T output, std::string path, bool print) {
    int rows = -1;
    int cols = -1;

    // Check if tyoe T has the functions rows() and cols(), which are used to get
    // the number of rows and cols of the array that is about to be outputted.
    if (has_rows<T>() == true) {
        rows = output.rows();
    }
    if(has_cols<T>() == true) {
        cols = output.cols();
    }

    IO::overwrite(path, print);
    IO::write_header(path, rows, cols, print);
    IO::write_double(path, output, rows, cols, print);
}

#endif
