//
//  config_settings.cpp
//
//  Modified by Samuel Senior on 05/02/2017.
//  Reads in input parameters and settings from a config file.
//

#include "config_settings.hpp"
#include <fstream>

#include <iostream>
#include <string>

namespace XNLO{
//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
//  Constructor
Config_Settings::Config_Settings() {  }

//------------------------------------------------------------------------------------------------//

const char * Config_Settings::setting_name[] = {
  "atoms_per_worker",
  "x_min", "x_max",
  "N_t", "t_min", "t_max",
  "P_av", "RR", "FWHM", "l_0", "CEO", "spot_radius",
  "alpha",
  "read_in_laser_pulse",
  "output_wavefunction",
  "pend_path",
  "path_input_j0",
  "path_laser_A_w_R",
  "path_laser_A_w_I",
  "path_laser_w_active",
  "path_dipole", "path_w",
  "path_E",
  "path_config_file", "path_config_log",
};

void Config_Settings::read_in(const std::string path, bool print_to_screen) {
  std::string input_name_char;
  std::string input_value_char;
  std::string input_description_char;
  int lines_found = 0;
  int lines_read = 0;
  std::string str;
  std::ifstream config_file (path);
	if (config_file.is_open()) {
    while (std::getline(config_file, str) && !config_file.eof()) {
      int i = 0;
      int one_start = -1;
      int one_end = -1;
      int two_start = -1;
      int two_end = -1;
      int three_start = -1;
      int three_end = -1;

      lines_found++;
      for (std::string::iterator it=str.begin(); it!=str.end(); it++, i++) {
          if (str[i] == '{') {
            if (one_start == -1) {
              one_start = i + 1;
            } else if (two_start == -1) {
              two_start = i + 1;
            } else if (three_start == -1) {
              three_start = i + 1;
            } else {
              std::cout << "Config_Settings::read_in: Error in reading in '{' from config file!\n";
            }
          } else if (str[i] == '}') {
            if (one_end == -1) {
              one_end = i;
            } else if (two_end == -1) {
              two_end = i;
            } else if (three_end == -1) {
              three_end = i;
            } else {
              std::cout << "Config_Settings::read_in: Error in reading in '}' from config file!\n";
            }
          }
        }
        bool name_valid = false;
        bool value_valid = false;
        bool description_valid = false;
        if (one_start == -1 || one_end == -1) {
        } else {
          input_name_char = str.substr(one_start, one_end - one_start);
          name_valid = true;
        }
        if (two_start == -1 || two_end == -1) {
        } else {
          input_value_char = str.substr(two_start, two_end - two_start);
          value_valid = true;
        }
        if (three_start == -1 || three_end == -1) {
        } else {
          input_description_char = str.substr(three_start, three_end - three_start);
          description_valid = true;
        }
        if (name_valid == true &&
            value_valid == true &&
            description_valid == true) {
              lines_read++;
              set_variable(input_name_char, input_value_char, input_description_char);
            }
    } //while
    if (config_file.eof() && print_to_screen == true) {
      std::cout << "Config_Settings::read_in: Info, " << lines_found << " lines found and " << lines_read << " lines read in from config file!\n";
    }
  } else if (print_to_screen == true) {
    std::cout << "Config_Settings::read_in: Error, could not open config file, using default values instead...\n";
  }
  config_file.close();
}

void Config_Settings::set_variable(std::string& variable_name, std::string& variable_value_str, std::string& input_description_char) {
  int n_settings = sizeof(setting_name)/sizeof(*setting_name);
  bool found = false;
  int i = 0;
  while (!found && i < n_settings) {
    if(variable_name == setting_name[i]) {
      found = true;
    } else {
      i++;
    }
  }
  SN SettingName;
  if (found == true) {
    SettingName = static_cast<SN>(i);
    switch(SettingName) {

      case SN::atoms_per_worker :
        atoms_per_worker_set(std::stoi(variable_value_str));
        atoms_per_worker_description_set(input_description_char);
        break;

      case SN::x_min :
        x_min_set(std::stod(variable_value_str));
        x_min_description_set(input_description_char);
        break;
      case SN::x_max :
        x_max_set(std::stod(variable_value_str));
        x_max_description_set(input_description_char);
        break;

      case SN::N_t :
        N_t_set(std::stoi(variable_value_str));
        N_t_description_set(input_description_char);
        break;
      case SN::t_min :
        t_min_set(std::stod(variable_value_str));
        t_min_description_set(input_description_char);
        break;
      case SN::t_max :
        t_max_set(std::stod(variable_value_str));
        t_max_description_set(input_description_char);
        break;

      case SN::P_av :
        P_av_set(std::stod(variable_value_str));
        P_av_description_set(input_description_char);
        break;
      case SN::RR :
        RR_set(std::stod(variable_value_str));
        RR_description_set(input_description_char);
        break;
      case SN::FWHM :
        FWHM_set(std::stod(variable_value_str));
        FWHM_description_set(input_description_char);
        break;
      case SN::l_0 :
        l_0_set(std::stod(variable_value_str));
        l_0_description_set(input_description_char);
        break;
      case SN::CEO :
        CEO_set(std::stod(variable_value_str));
        CEO_description_set(input_description_char);
        break;
      case SN::spot_radius :
        spot_radius_set(std::stod(variable_value_str));
        spot_radius_description_set(input_description_char);
        break;

      case SN::alpha :
        alpha_set(std::stod(variable_value_str));
        alpha_description_set(input_description_char);
        break;

      case SN::read_in_laser_pulse :
        read_in_laser_pulse_set(std::stoi(variable_value_str));
        read_in_laser_pulse_description_set(input_description_char);
        break;

      case SN::output_wavefunction :
        output_wavefunction_set(std::stoi(variable_value_str));
        output_wavefunction_description_set(input_description_char);
        break;

      case SN::pend_path :
        pend_path_set(variable_value_str);
        pend_path_description_set(input_description_char);
        break;

      case SN::path_input_j0 :
        path_input_j0_set(variable_value_str);
        path_input_j0_description_set(input_description_char);
        break;

      case SN::path_laser_A_w_R :
        path_laser_A_w_R_set(variable_value_str);
        path_laser_A_w_R_description_set(input_description_char);
        break;
      case SN::path_laser_A_w_I :
        path_laser_A_w_I_set(variable_value_str);
        path_laser_A_w_I_description_set(input_description_char);
        break;
      case SN::path_laser_w_active :
        path_laser_w_active_set(variable_value_str);
        path_laser_w_active_description_set(input_description_char);
        break;

      case SN::path_dipole :
        path_dipole_set(variable_value_str);
        path_dipole_description_set(input_description_char);
        break;
      case SN::path_w :
        path_w_set(variable_value_str);
        path_w_description_set(input_description_char);
        break;

      case SN::path_E :
        path_E_set(variable_value_str);
        path_E_description_set(input_description_char);
        break;

      case SN::path_config_file :
        path_config_file_set(variable_value_str);
        path_config_file_description_set(input_description_char);
        break;
      case SN::path_config_log :
        path_config_log_set(variable_value_str);
        path_config_log_description_set(input_description_char);
        break;

      default: std::cout << "Config_Settings::set_variable: Error, SettingName (number value) " << static_cast<std::underlying_type<SN>::type>(SettingName) << " does not correspond to set values!" << std::endl;
    };
  } else {
    std::cout << "Config_Settings::set_variable: Error, unknown variable name '" << variable_name << "'\n";
  }
}

void Config_Settings::check_paths(bool print_to_screen) {
  int i = 0, j = 0, k = 0;
  std::string pending_string = std::to_string(static_cast<unsigned long long>(i)) + std::to_string(static_cast<unsigned long long>(j)) + std::to_string(static_cast<unsigned long long>(k));
  std::string path = set_path(path_dipole(), pending_string);

  bool unique_path = false;
  if (pend_path() != "false") {
    while (!unique_path) {
      if (std::ifstream(path)) {
        //std::cout << "Config_Settings::check_paths: " << path << " already exists, trying incremented path\n";
        k++;
          if (k == 10) {
            k = 0;
            j++;
            if (j == 10) {
              j = 0;
              i++;
              if (i >= 10) {
                std::cout << "Config_Settings::check_paths: Warning, path incrememnt greater than 999\n";
              }
            }
        }
        pending_string =  std::to_string(static_cast<unsigned long long>(i)) + std::to_string(static_cast<unsigned long long>(j)) + std::to_string(static_cast<unsigned long long>(k));
        path = set_path(path_dipole(), pending_string);
      } else {
        if (print_to_screen == true) {
          std::cout << "Config_Settings::check_paths: Info, unique path " << path << " found!\n";
        }
        unique_path = true;
      }
    }
  }

  path_dipole_set(path);

  path = set_path(path_w(), pending_string);
  path_w_set(path);

  path = set_path(path_E(), pending_string);
  path_E_set(path);

  path = set_path(path_config_log(), pending_string);
  path_config_log_set(path);
}

std::string Config_Settings::set_path(std::string path, std::string pending_string) {
  if (pend_path() == "prepend") {
    return set_pre_path(pending_string, path);
  } else if (pend_path() == "postpend") {
    return set_post_path(path, pending_string);
  } else if (pend_path() == "false") {
    return path;
  } else {
    std::cout << "Config_Settings::check_paths: Warning, unknown pend switch, using default\n";
    return set_pre_path(pending_string, path);
  }
}

std::string Config_Settings::set_pre_path(std::string pre_path, std::string path) {
  std::size_t found = path.find_last_of("/");
  if (found == -1) {
    path = pre_path + "_" + path;
  } else {
    path = path.substr(0, found+1) + pre_path + "_" + path.substr(found+1);
  }
  return path;
}

std::string Config_Settings::set_post_path(std::string path, std::string post_path) {
    std::size_t found = path.find_last_of(".");
    if (found == -1) {
      path = path + "_" + post_path;
    } else {
      path = path.substr(0, found) + "_" + post_path + path.substr(found);
    }
    return path;
}

void Config_Settings::print() {
    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "Input Parameters and Settings:\n";
    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "   atoms_per_worker:    " << atoms_per_worker() << "                    " << atoms_per_worker_description() << std::endl;
    std::cout << "   x_min:               " << x_min() << "           " << x_min_description() << std::endl;
    std::cout << "   x_max:               " << x_max() << "              " << x_max_description() << std::endl;
    std::cout << "   N_t:                 " << N_t() << "                " << N_t_description() << std::endl;
    std::cout << "   t_min:               " << t_min() << "               " << t_min_description() << std::endl;
    std::cout << "   t_max:               " << t_max() << "              " << t_max_description() << std::endl;
    std::cout << "   P_av:                " << P_av() << "             " << P_av_description() << std::endl;
    std::cout << "   RR:                  " << RR() << "                 " << RR_description() << std::endl;
    std::cout << "   FWHM:                " << FWHM() << "                " << FWHM_description() << std::endl;
    std::cout << "   l_0:                 " << l_0() << "              " << l_0_description() << std::endl;
    std::cout << "   CEO:                 " << CEO() << "                    " << CEO_description() << std::endl;
    std::cout << "   spot_radius:         " << spot_radius() << "              " << spot_radius_description() << std::endl;
    std::cout << "   alpha:               " << alpha() << "                 " << alpha_description() << std::endl;
    std::cout << "   read_in_laser_pulse: " << read_in_laser_pulse() << "                    " << read_in_laser_pulse_description() << std::endl;
    std::cout << "   output_wavefunction: " << output_wavefunction() << "                    " << output_wavefunction_description() << std::endl;
    std::cout << "   pend_path:           " << pend_path() << "                  " << pend_path_description() << std::endl;
    std::cout << "   path_input_j0:       " << path_input_j0() << "    " << path_input_j0_description() << std::endl;
    std::cout << "   path_laser_A_w_R:    " << path_laser_A_w_R() << "        " << path_laser_A_w_R_description() << std::endl;
    std::cout << "   path_laser_A_w_I:    " << path_laser_A_w_I() << "        " << path_laser_A_w_I_description() << std::endl;
    std::cout << "   path_laser_w_active: " << path_laser_w_active() << "        " << path_laser_w_active_description() << std::endl;
    std::cout << "   path_dipole:         " << path_dipole() << "     " << path_dipole_description() << std::endl;
    std::cout << "   path_w:              " << path_w() << "          " << path_w_description() << std::endl;
    std::cout << "   path_E:              " << path_E() << "          " << path_E_description() << std::endl;
    std::cout << "   path_config_file:    " << path_config_file() << "                 " << path_config_file_description() << std::endl;
    std::cout << "   path_config_log:     " << path_config_log() << " " << path_config_log_description() << std::endl;
    std::cout << "-------------------------------------------------------------------------------\n";
}

void Config_Settings::print(std::string path_) {
    std::ofstream config_log;
    config_log.open(path_);
    if (config_log.is_open()) {
      config_log << "{atoms_per_worker} {" << atoms_per_worker() << "} {" << atoms_per_worker_description() << "}\n";

      config_log << "{x_min} {" << x_min() << "} {" << x_min_description() << "}\n";
      config_log << "{x_max} {" << x_max() << "} {" << x_max_description() << "}\n";

      config_log << "{N_t} {" << N_t() << "} {" << N_t_description() << "}\n";
      config_log << "{t_min} {" << t_min() << "} {" << t_min_description() << "}\n";
      config_log << "{t_max} {" << t_max() << "} {" << t_max_description() << "}\n";

      config_log << "{P_av} {" << P_av() << "} {" << P_av_description() << "}\n";
      config_log << "{RR} {" << RR() << "} {" << RR_description() << "}\n";
      config_log << "{FWHM} {" << FWHM() << "} {" << FWHM_description() << "}\n";
      config_log << "{l_0} {" << l_0() << "} {" << l_0_description() << "}\n";
      config_log << "{CEO} {" << CEO() << "} {" << CEO_description() << "}\n";
      config_log << "{spot_radius} {" << spot_radius() << "} {" << spot_radius_description() << "}\n";

      config_log << "{alpha} {" << alpha() << "} {" << alpha_description() << "}\n";

      config_log << "{read_in_laser_pulse} {" << read_in_laser_pulse() << "} {" << read_in_laser_pulse_description() << "}\n";

      config_log << "{output_wavefunction} {" << output_wavefunction() << "} {" << output_wavefunction_description() << "}\n";

      config_log << "{pend_path} {" << pend_path() << "} {" << pend_path_description() << "}\n";

      config_log << "{path_input_j0} {" << path_input_j0() << "} {" << path_input_j0_description() << "}\n";

      config_log << "{path_laser_A_w_R} {" << path_laser_A_w_R() << "} {" << path_laser_A_w_R_description() << "}\n";
      config_log << "{path_laser_A_w_I} {" << path_laser_A_w_I() << "} {" << path_laser_A_w_I_description() << "}\n";
      config_log << "{path_laser_w_active} {" << path_laser_w_active() << "} {" << path_laser_w_active_description() << "}\n";

      config_log << "{path_dipole} {" << path_dipole() << "} {" << path_dipole_description() << "}\n";
      config_log << "{path_w} {" << path_w() << "} {" << path_w_description() << "}\n";

      config_log << "{path_E} {" << path_E() << "} {" << path_E_description() << "}\n";

      config_log << "{path_config_file} {" << path_config_file() << "} {" << path_config_file_description() << "}\n";
      config_log << "{path_config_log} {" << path_config_log() << "} {" << path_config_log_description() << "}\n";

      config_log.close();

      std::cout << "Config_Settings::print: Successfully wrote config log to " << path_ << std::endl;

    } else {
      std::cout << "Config_Settings::print: Error, could not print to config log at " << path_ << std::endl;
    }
}

int Config_Settings::atoms_per_worker() {
    return atoms_per_worker_;
}
void Config_Settings::atoms_per_worker_set(int val) {
    atoms_per_worker_ = val;
}
std::string Config_Settings::atoms_per_worker_description() {
  return atoms_per_worker_description_;
}
void Config_Settings::atoms_per_worker_description_set(std::string description) {
  atoms_per_worker_description_ = description;
}

double Config_Settings::x_min() {
    return x_min_;
}
void Config_Settings::x_min_set(double val) {
    x_min_ = val;
}
std::string Config_Settings::x_min_description() {
  return x_min_description_;
}
void Config_Settings::x_min_description_set(std::string description) {
  x_min_description_ = description;
}

double Config_Settings::x_max() {
    return x_max_;
}
void Config_Settings::x_max_set(double val) {
    x_max_ = val;
}
std::string Config_Settings::x_max_description() {
  return x_max_description_;
}
void Config_Settings::x_max_description_set(std::string description) {
  x_max_description_ = description;
}

int Config_Settings::N_t() {
    return N_t_;
}
void Config_Settings::N_t_set(int val) {
    N_t_ = val;
}
std::string Config_Settings::N_t_description() {
  return N_t_description_;
}
void Config_Settings::N_t_description_set(std::string description) {
  N_t_description_ = description;
}

double Config_Settings::t_min() {
    return t_min_;
}
void Config_Settings::t_min_set(double val) {
    t_min_ = val;
}
std::string Config_Settings::t_min_description() {
  return t_min_description_;
}
void Config_Settings::t_min_description_set(std::string description) {
  t_min_description_ = description;
}

double Config_Settings::t_max() {
    return t_max_;
}
void Config_Settings::t_max_set(double val) {
    t_max_ = val;
}
std::string Config_Settings::t_max_description() {
  return t_max_description_;
}
void Config_Settings::t_max_description_set(std::string description) {
  t_max_description_ = description;
}

double Config_Settings::P_av() {
    return P_av_;
}
void Config_Settings::P_av_set(double val) {
    P_av_ = val;
}
std::string Config_Settings::P_av_description() {
  return P_av_description_;
}
void Config_Settings::P_av_description_set(std::string description) {
  P_av_description_ = description;
}

double Config_Settings::RR() {
    return RR_;
}
void Config_Settings::RR_set(double val) {
    RR_ = val;
}
std::string Config_Settings::RR_description() {
  return RR_description_;
}
void Config_Settings::RR_description_set(std::string description) {
  RR_description_ = description;
}

double Config_Settings::FWHM() {
    return FWHM_;
}
void Config_Settings::FWHM_set(double val) {
    FWHM_ = val;
}
std::string Config_Settings::FWHM_description() {
  return FWHM_description_;
}
void Config_Settings::FWHM_description_set(std::string description) {
  FWHM_description_ = description;
}

double Config_Settings::l_0() {
    return l_0_;
}
void Config_Settings::l_0_set(double val) {
    l_0_ = val;
}
std::string Config_Settings::l_0_description() {
  return l_0_description_;
}
void Config_Settings::l_0_description_set(std::string description) {
  l_0_description_ = description;
}

double Config_Settings::CEO() {
    return CEO_;
}
void Config_Settings::CEO_set(double val) {
    CEO_ = val;
}
std::string Config_Settings::CEO_description() {
  return CEO_description_;
}
void Config_Settings::CEO_description_set(std::string description) {
  CEO_description_ = description;
}

double Config_Settings::spot_radius() {
    return spot_radius_;
}
void Config_Settings::spot_radius_set(double val) {
    spot_radius_ = val;
}
std::string Config_Settings::spot_radius_description() {
  return spot_radius_description_;
}
void Config_Settings::spot_radius_description_set(std::string description) {
  spot_radius_description_ = description;
}

double Config_Settings::alpha() {
    return alpha_;
}
void Config_Settings::alpha_set(double val) {
    alpha_ = val;
}
std::string Config_Settings::alpha_description() {
  return alpha_description_;
}
void Config_Settings::alpha_description_set(std::string description) {
  alpha_description_ = description;
}

int Config_Settings::read_in_laser_pulse() {
    return read_in_laser_pulse_;
}
void Config_Settings::read_in_laser_pulse_set(int val) {
    read_in_laser_pulse_ = val;
}
std::string Config_Settings::read_in_laser_pulse_description() {
  return read_in_laser_pulse_description_;
}
void Config_Settings::read_in_laser_pulse_description_set(std::string description) {
  read_in_laser_pulse_description_ = description;
}

int Config_Settings::output_wavefunction() {
    return output_wavefunction_;
}
void Config_Settings::output_wavefunction_set(int val) {
    output_wavefunction_ = val;
}
std::string Config_Settings::output_wavefunction_description() {
  return output_wavefunction_description_;
}
void Config_Settings::output_wavefunction_description_set(std::string description) {
  output_wavefunction_description_ = description;
}

std::string Config_Settings::pend_path() {
    return pend_path_;
}
void Config_Settings::pend_path_set(std::string pend_path_val) {
    pend_path_ = pend_path_val;
}
std::string Config_Settings::pend_path_description() {
  return pend_path_description_;
}
void Config_Settings::pend_path_description_set(std::string description_) {
  pend_path_description_ = description_;
}

std::string Config_Settings::path_input_j0() {
    return path_input_j0_;
}
void Config_Settings::path_input_j0_set(std::string value) {
    path_input_j0_ = value;
}
std::string Config_Settings::path_input_j0_description() {
  return path_input_j0_description_;
}
void Config_Settings::path_input_j0_description_set(std::string description) {
  path_input_j0_description_ = description;
}

std::string Config_Settings::path_laser_A_w_R() {
    return path_laser_A_w_R_;
}
void Config_Settings::path_laser_A_w_R_set(std::string value) {
    path_laser_A_w_R_ = value;
}
std::string Config_Settings::path_laser_A_w_R_description() {
  return path_laser_A_w_R_description_;
}
void Config_Settings::path_laser_A_w_R_description_set(std::string description) {
  path_laser_A_w_R_description_ = description;
}
std::string Config_Settings::path_laser_A_w_I() {
    return path_laser_A_w_I_;
}
void Config_Settings::path_laser_A_w_I_set(std::string value) {
    path_laser_A_w_I_ = value;
}
std::string Config_Settings::path_laser_A_w_I_description() {
  return path_laser_A_w_I_description_;
}
void Config_Settings::path_laser_A_w_I_description_set(std::string description) {
  path_laser_A_w_I_description_ = description;
}
std::string Config_Settings::path_laser_w_active() {
    return path_laser_w_active_;
}
void Config_Settings::path_laser_w_active_set(std::string value) {
    path_laser_w_active_ = value;
}
std::string Config_Settings::path_laser_w_active_description() {
  return path_laser_w_active_description_;
}
void Config_Settings::path_laser_w_active_description_set(std::string description) {
  path_laser_w_active_description_ = description;
}

std::string Config_Settings::path_dipole() {
    return path_dipole_;
}
void Config_Settings::path_dipole_set(std::string val) {
    path_dipole_ = val;
}
std::string Config_Settings::path_dipole_description() {
  return path_dipole_description_;
}
void Config_Settings::path_dipole_description_set(std::string description) {
    path_dipole_description_ = description;
}

std::string Config_Settings::path_w() {
    return path_w_;
}
void Config_Settings::path_w_set(std::string val) {
    path_w_ = val;
}
std::string Config_Settings::path_w_description() {
  return path_w_description_;
}
void Config_Settings::path_w_description_set(std::string description) {
  path_w_description_ = description;
}

std::string Config_Settings::path_E() {
    return path_E_;
}
void Config_Settings::path_E_set(std::string val) {
    path_E_ = val;
}
std::string Config_Settings::path_E_description() {
  return path_E_description_;
}
void Config_Settings::path_E_description_set(std::string description) {
  path_E_description_ = description;
}

std::string Config_Settings::path_config_file() {
    return path_config_file_;
}
void Config_Settings::path_config_file_set(std::string val) {
    path_config_file_ = val;
}
std::string Config_Settings::path_config_file_description() {
  return path_config_file_description_;
}
void Config_Settings::path_config_file_description_set(std::string description) {
  path_config_file_description_ = description;
}

std::string Config_Settings::path_config_log() {
    return path_config_log_;
}
void Config_Settings::path_config_log_set(std::string val) {
    path_config_log_ = val;
}
std::string Config_Settings::path_config_log_description() {
  return path_config_log_description_;
}
void Config_Settings::path_config_log_description_set(std::string description) {
  path_config_log_description_ = description;
}

}
