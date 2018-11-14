//
//  IO.hpp
//
//  Modified by Patrick Anderson on 09/05/2015.
//  
//

#include "IO.hpp"
#include "../Eigen/Dense"
#include <fstream>
#include <iostream>
#include <string>

using namespace Eigen;

namespace XNLO {

    //------------------------------------------------------------------------------------------------//
    //  Class implementation
    //------------------------------------------------------------------------------------------------//
    /*! Class constructor. Sets unrealistic values for header variables so that if one isn't succesfully
        read/written then it will be caught quickly.
    */
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
                std::cout << "text::read_double: File " << path << " successfully opened\n";
            }
            fs.read((char*) output.data(), N_row_ * N_col_ * sizeof(double));
        } else {
            std::cout << "text::read_double: Error, file " << path << " not found!\n";
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
                std::cout << "text::read_double: File " << path << " successfully opened\n";
            }
            if (skip_header == true) {
                char* temp = new char[header_size+1];
                fs.read(temp, header_size);
                delete [] temp;
            }
            fs.read((char*) output.data(), N_row_ * N_col_ * double_size);
        } else {
            std::cout << "text::read_double: Error, file " << path << " not found!\n";
            std::cout << "Press enter to continue...";
            std::cin.get();
        }
        fs.close();

        return(output);
    }

    void IO::write_ascii_double(const std::string path, ArrayXXd data) {
    /*!
    Write to an ascii text file from an Eigen two odimensional array of doubles.
    */
        std::ofstream file;
        file.open(path);

        if (file.is_open()) {

            file << data;
            file.close();

        } else {

            std::cout << "Error writing file!" << std::endl;

        }

    }

    void IO::write_double(const std::string path, ArrayXXd output, int N_row_, int N_col_) {
    /*!
    Write to the binary file from a N_col_ by N_row_ Eigen array of doubles.
    */

        std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
        if (fs.is_open()) {
            fs.write((char*) output.data(), N_row_ * N_col_ * sizeof(double));
            std::cout << "text::write_double: Successfully wrote data to file " << path << std::endl;
        } else {

            std::cout << "text::write_double: Error, file " << path << " could not be opened to write data to!\n";
        }
        fs.close();
    }

    //------------------------------------------------------------------------------------------------//
    void IO::write_header(const std::string path, int N_row_, int N_col_) {
    /*!
    Writes the %XNLO binary header to a given binary file.

    The XNLO binary header takes the form:
    |Offset | Size (Bytes) | Type/Contents | Description                   |
    |:-----:|:------------:|:-------------:|:------------------------------|
    |0      |4             |'%XNLO'        |Binary format name             |
    |4      |4             |int            |Version number                 |
    |8      |4             |int            |Subversion number              |
    |12     |4             |int            |Size of header                 |
    |16     |4             |int            |N_row                          |
    |20     |4             |int            |N_col                          |
    |24     |4             |int            |Total size of data             |
    |28     |4             |int            |Size of each double in the data|
    That is to say, the first four bytes of an %XNLO binary header are the four characters '%XNLO', used
    to specify which file type it is. The next four bytes give an integer, which is the version number.
    The next four give the subversion number as integer, and so on.
    */
        std::string XNLO_binary_format = "XNLO";
        int XNLO_binary_format_version = 1;
        int XNLO_binary_format_subversion = 0;
        int XNLO_binary_format_len = XNLO_binary_format.length();
        int XNLO_data_size = N_row_ * N_col_;
        int XNLO_double_size = sizeof(double);

        int XNLO_header_size = sizeof(int) +
                               XNLO_binary_format_len +
                               sizeof(XNLO_binary_format_version) +
                               sizeof(XNLO_binary_format_subversion) +
                               sizeof(N_row_) +
                               sizeof(N_col_) +
                               sizeof(XNLO_data_size) +
                               sizeof(XNLO_double_size);

        std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
        if (fs.is_open()) {
            fs.write(XNLO_binary_format.c_str(), XNLO_binary_format_len);
            fs.write(reinterpret_cast<const char *>(&XNLO_binary_format_version), sizeof(XNLO_binary_format_version));
            fs.write(reinterpret_cast<const char *>(&XNLO_binary_format_subversion), sizeof(XNLO_binary_format_subversion));
            fs.write(reinterpret_cast<const char *>(&XNLO_header_size), sizeof(XNLO_header_size));
            fs.write(reinterpret_cast<const char *>(&N_row_), sizeof(N_row_));
            fs.write(reinterpret_cast<const char *>(&N_col_), sizeof(N_col_));
            fs.write(reinterpret_cast<const char *>(&XNLO_data_size), sizeof(XNLO_data_size));
            fs.write(reinterpret_cast<const char *>(&XNLO_double_size), sizeof(XNLO_double_size));
            std::cout << "text::write_header: Successfully wrote data to file " << path << std::endl;
        } else {

            std::cout << "text::write_header: Error, file " << path << " could not be opened to write data to!\n";
        }
        fs.close();
    }

    void IO::overwrite(const std::string path) {
    /*!
    Overwrites the given binary file.
    */
        std::cout << "text::overwrite_file: Overwriting file " << path << "... ";
        std::ofstream fs(path, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if (fs.is_open()) {
            std::cout << "Sucessfull!" << std::endl;
        } else {

            std::cout << "Error, file " << path << " could not be opened to overwrite!\n";
        }
        fs.close();
    }

} // XNLO namespace
