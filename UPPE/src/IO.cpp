//
//  IO.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "IO" objects enable reading/writing of binary files to/from eigen arrays.
//

#include "IO.hpp"
#include "Eigen/Dense"
#include <fstream>

#include <iostream>

#include <string>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
IO::IO() {  }

//------------------------------------------------------------------------------------------------//
Array<unsigned short, Dynamic, Dynamic> IO::read_uint16(const char* path, int N_row_, int N_col_) {
/*!
Read from binary file to N_col_ by N_row_ Eigen array
uint16
*/

    Array<unsigned short, Dynamic, Dynamic> output(N_row_, N_col_);
    std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);
    if (fs.is_open()) {
        std::cout << "IO::read_uint16: File " << path << " successfully opened\n";
        fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(unsigned short));
    } else {
        std::cout << "IO::read_uint16: Error, file " << path << " not found!\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
    }
    fs.close();

    return(output);
}

// int
ArrayXXi IO::read_int(const char* path, int N_row_, int N_col_) {
/*!
int
*/

    ArrayXXi output(N_row_, N_col_);
    std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);
    if (fs.is_open()) {
        std::cout << "IO::read_int: File " << path << " successfully opened\n";
        fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(int));
    } else {
        std::cout << "IO::read_int: Error, file " << path << " not found!\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
    }
    fs.close();

    return(output);
}

// double
ArrayXXd IO::read_double(const std::string path, int N_row_, int N_col_) {
/*!
Read double to Eigen array from binary file
*/

    ArrayXXd output(N_row_, N_col_);
    std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);
    if (fs.is_open()) {
        std::cout << "IO::read_double: File " << path << " successfully opened\n";
        fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(double));
    } else {
        std::cout << "IO::read_double: Error, file " << path << " not found!\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
    }
    fs.close();

    return(output);
}

// double
ArrayXXd IO::read_ascii_double(const std::string path, int N_row_, int N_col_) {
/*!
Read double to Eigen array from ascii file
*/

    ArrayXXd output(N_row_, N_col_);
    std::ifstream fs(path, std::ios_base::in);
    if (fs.is_open()) {
        //std::cout << "IO::read_double: File " << path << " successfully opened\n";  // Put this back in at some point?
        fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(double));
    } else {
        std::cout << "IO::read_double: Error, file " << path << " not found!\n";
        std::cout << "Press enter to continue...";
        std::cin.get();
    }
    fs.close();

    return(output);
}

//------------------------------------------------------------------------------------------------//
// Write to binary file from N_col_ by N_row_ Eigen array
// double
void IO::write_double(const std::string path, ArrayXXd input, int N_row_, int N_col_, bool print) {
/*!
Write to binary file from N_col_ by N_row_ Eigen array
double
*/

    std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
    if (fs.is_open()) {
        fs.write((char*) input.data(), N_row_ * N_col_ * sizeof(double));
        if (print) std::cout << "IO::write_double: Successfully wrote data to file " << path << std::endl;
    } else {

        std::cout << "IO::write_double: Error, file " << path << " could not be opened to write data to!\n";
    }
    fs.close();
}


//------------------------------------------------------------------------------------------------//
void IO::write_header(const std::string path, int N_row_, int N_col_, bool print) {
/*!
Write UPPE binary header to given binary file.
Header takes the form:
    Offset  Size (Bytes)    Type/Contents   Description
         0             4           'UPPE'   Binary format name
         4             4              int   Version Number
         8             4              int   Subversion number
        12             4              int   Size of header
        16             4              int   N_row
        20             4              int   N_col
        24             4              int   Total size of data
        28             4              int   Size of each double in the data
*/
    std::string UPPE_binary_format = "UPPE";
    int UPPE_binary_format_version = 1;
    int UPPE_binary_format_subversion = 0;
    int UPPE_binary_format_len = UPPE_binary_format.length();
    int UPPE_data_size = N_row_ * N_col_;
    int UPPE_double_size = sizeof(double);

    int UPPE_header_size = sizeof(int) +  // Size of header size
                           UPPE_binary_format_len +
                           sizeof(UPPE_binary_format_version) +
                           sizeof(UPPE_binary_format_subversion) +
                           sizeof(N_row_) +
                           sizeof(N_col_) +
                           sizeof(UPPE_data_size) +
                           sizeof(UPPE_double_size);

    std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
    if (fs.is_open()) {
        fs.write(UPPE_binary_format.c_str(), UPPE_binary_format_len);
        fs.write(reinterpret_cast<const char *>(&UPPE_binary_format_version), sizeof(UPPE_binary_format_version));
        fs.write(reinterpret_cast<const char *>(&UPPE_binary_format_subversion), sizeof(UPPE_binary_format_subversion));
        fs.write(reinterpret_cast<const char *>(&UPPE_header_size), sizeof(UPPE_header_size));
        fs.write(reinterpret_cast<const char *>(&N_row_), sizeof(N_row_));
        fs.write(reinterpret_cast<const char *>(&N_col_), sizeof(N_col_));
        fs.write(reinterpret_cast<const char *>(&UPPE_data_size), sizeof(UPPE_data_size));
        fs.write(reinterpret_cast<const char *>(&UPPE_double_size), sizeof(UPPE_double_size));
        if (print) std::cout << "IO::write_header: Successfully wrote data to file " << path << std::endl;
    } else {

        std::cout << "IO::write_header: Error, file " << path << " could not be opened to write data to!\n";
    }
    fs.close();
}

//------------------------------------------------------------------------------------------------//
void IO::overwrite(const std::string path, bool print) {
/*!
Overwrites given binary file.
*/
    if (print) std::cout << "IO::overwrite_file: Overwriting file " << path << " ... ";
    std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    if (fs.is_open()) {
        if (print) std::cout << "Sucessfull!" << path << std::endl;
    } else {

        std::cout << "Error, file " << path << " could not be opened to overwrite!\n";
    }
    fs.close();
}

void IO::write_ascii_double(ArrayXd data, std::string path, bool print) {

    std::ofstream file;
    file.open(path);

    if (file.is_open()) {

        file << data;
        file.close();

    } else {

        std::cout << "Error writing file!" << std::endl;

    }

}
