//
//  IO.hpp
//
//  Originally created by Patrick Anderson.
//  Modified by Samuel Senior on 10/03/2017.
//  "IO" objects enable reading/writing of binary files to/from eigen arrays.
//

#include "IO.hpp"
#include "../Eigen/Dense"
#include <fstream>

#include <iostream>

#include <string>

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
IO::IO() {
    std::string binary_format = "XNLO";
    binary_format_version = -1;
    binary_format_subversion = -1;
    binary_format_len = binary_format.length();
    data_size = -1;
    double_size = -1;//sizeof(double);
    N_row_ = -1;
    N_col_ = -1;
    header_size = -1;
}

//------------------------------------------------------------------------------------------------//
    void IO::read_header(const std::string path, bool print) {
    /*!
    Read the %XNLO/UPPE binary header of a given binary file.

    The header takes the form:
    |Offset | Size (Bytes) | Type/Contents | Description                   |
    |:-----:|:------------:|:-------------:|:------------------------------|
    |0      |4             |'%XNLO'/'UPPE' |Binary format name             |
    |4      |4             |int            |Version number                 |
    |8      |4             |int            |Subversion number              |
    |12     |4             |int            |Size of header                 |
    |16     |4             |int            |N_row                          |
    |20     |4             |int            |N_col                          |
    |24     |4             |int            |Total size of data             |
    |28     |4             |int            |Size of each double in the data|
    That is to say, the first four bytes of an %XNLO or UPPE binary header are the either the four
    characters '%XNLO' or 'UPPE', used to specify which file type it is. The next four bytes give
    an integer, which is the version number. The next four give the subversion number as integer,
    and so on.


    */
        if (print == true) {
            std::cout << "binary_format_len+1: " << binary_format_len+1 << std::endl;
        }
        char* temp = new char[binary_format_len+1];
        temp[binary_format_len] = '\0';
        std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);// | std::ios_base::app);
        if (fs.is_open()) {
            fs.read(temp, binary_format_len);
            if (!fs) {
                std::cout << "Error in reading binary format, " << fs.gcount() << " could be read" << std::endl;
                std::cout << " good()=" << fs.good() << std::endl;
                std::cout << " eof()=" << fs.eof() << std::endl;
                std::cout << " fail()=" << fs.fail() << std::endl;
                std::cout << " bad()=" << fs.bad() << std::endl;
            }
            binary_format = temp;
            delete [] temp;
            fs.read(reinterpret_cast< char *>(&binary_format_version), sizeof(binary_format_version));
            fs.read(reinterpret_cast< char *>(&binary_format_subversion), sizeof(binary_format_subversion));
            fs.read(reinterpret_cast< char *>(&header_size), sizeof(header_size));
            fs.read(reinterpret_cast< char *>(&N_row_), sizeof(N_row_));
            fs.read(reinterpret_cast< char *>(&N_col_), sizeof(N_col_));
            fs.read(reinterpret_cast< char *>(&data_size), sizeof(data_size));
            fs.read(reinterpret_cast< char *>(&double_size), sizeof(double_size));
            if (print == true) {
                std::cout << "IO::read_header: Successfully read data from file " << path << std::endl;
            }
        } else {

            std::cout << "IO::read_header: Error, file " << path << " could not be opened to read data from!\n";
        }
        fs.close();

        if (print == true) {
            std::cout << "Binary format: " << binary_format << std::endl;
            std::cout << "Version number: " << binary_format_version << std::endl;
            std::cout << "Subversion number: " << binary_format_subversion << std::endl;
            std::cout << "Size of header: " << header_size << std::endl;
            std::cout << "N_row: " << N_row_ << std::endl;
            std::cout << "N_col: " << N_col_ << std::endl;
            std::cout << "Total size of data: " << data_size << std::endl;
            std::cout << "Size of each double in the data: " << double_size << std::endl;
        }

    }

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

ArrayXXd IO::read_double(const std::string path, int N_row_, int N_col_, bool print) {
    /*!
    Read a two-dimensional array of doubles into an to Eigen array from a binary file.
    The number of rows and columns of the array are given by N_row_ and N_col_, as passed in
    from the function arguments.
    */

        ArrayXXd output(N_row_, N_col_);
        std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);
        if (fs.is_open()) {
            if (print == true) {
                std::cout << "IO::read_double: File " << path << " successfully opened\n";
            }
            fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(double));
        } else {
            std::cout << "IO::read_double: Error, file " << path << " not found!\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
        }
        fs.close();

        return(output);
    }

ArrayXXd IO::read_double(const std::string path, bool skip_header, bool print) {
    /*!
    Read a two-dimensional array of doubles into an to Eigen array from a binary file.
    The number of rows and columns of the array are taken as the class variables N_row_
    and N_col_.
    */

        ArrayXXd output(N_row_, N_col_);
        std::ifstream fs(path, std::ios_base::in | std::ios_base::binary);
        if (fs.is_open()) {
            if (print == true) {
                std::cout << "IO::read_double: File " << path << " successfully opened\n";
            }
            if (skip_header == true) {
                char* temp = new char[header_size+1];
                fs.read(temp, header_size);
                delete [] temp;
            }
            fs.read((char*) output.data(), N_row_ * N_col_ * double_size);
        } else {
            std::cout << "IO::read_double: Error, file " << path << " not found!\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
        }
        fs.close();

        return(output);
    }

// double
ArrayXXd IO::read_ascii_double(const std::string path, int N_row_, int N_col_, bool header) {
/*!
Read double to Eigen array from ascii file
*/

    //char test[256];
    std::string test;
    std::string tmp;

    ArrayXXd output(N_row_, N_col_);
    std::ifstream fs(path, std::ios_base::in);
    if (fs.is_open()) {
        //std::cout << "IO::read_double: File " << path << " successfully opened\n";  // Put this back in at some point?
        if (header == true) {
            getline(fs, test);
//std::cout << test << std::endl;
        }
//std::cout << "-----" << std::endl;
        int i = 0;
        while(getline(fs, test)) {
            std::stringstream ss(test);
            int j = 0;
//std::cout << "ss: ";
            while(getline(ss, tmp, '\t')) {
                //ss >> tmp;
//std::cout << "'" << tmp << "' ";
                output.row(i).col(j) = std::stod(tmp);
                j++;
            }
//std::cout << std::endl;
//std::cout << "Output(" << i << "): " << output.row(i) << std::endl;
            i++;
        }
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
Write HHGP binary header to given binary file.
Header takes the form:
    Offset  Size (Bytes)    Type/Contents   Description
         0             4           'HHGP'   Binary format name
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

