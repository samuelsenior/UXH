#ifndef __HH_SOURCE_HPP_INCLUDED__
#define __HH_SOURCE_HPP_INCLUDED__

#include <iostream>
#include <string>

#include "../../Eigen/Dense"

#include "../IO/IO.hpp"
#include "config_settings.hpp"
#include "../maths/maths_textbook.hpp"
#include "../DHT/DHT.hpp"

using namespace Eigen;

//namespace HHGP {

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class HH_source{
    int fileNumber;
    HH::Config_Settings config;
    maths_textbook maths;

public:

    HH_source();
    ArrayXXcd GetSource(int fileNumber, HH::Config_Settings config, maths_textbook maths);

};

//} // HHGP namespace

#endif
