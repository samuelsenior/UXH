#ifndef __HH_SOURCE_HPP_INCLUDED__
#define __HH_SOURCE_HPP_INCLUDED__

#include <iostream>
#include <string>

#include "Eigen/Dense"

#include "IO.hpp"
#include "config_settings.hpp"
#include "maths_textbook.hpp"
#include "DHT.hpp"

using namespace Eigen;

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//
class HH_source{
    int fileNumber;
    Config_Settings config;
    maths_textbook maths;

public:

    HH_source();
    ArrayXXcd GetSource(int fileNumber, Config_Settings config, maths_textbook maths);

};

#endif
