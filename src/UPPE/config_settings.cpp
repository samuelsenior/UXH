//
//  config_settings.cpp
//
//  Modified by Samuel Senior on 05/02/2017.
//  "config_settings" reads in the input parameters and settings from a config
//   and sets the corresponding variables to these read in values.
//

#include "config_settings.hpp"
#include <fstream>

#include <iostream>
#include <string>

#include <algorithm>

//namespace UPPE {

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
Config_Settings::Config_Settings() {  }

//------------------------------------------------------------------------------------------------//

const char * Config_Settings::setting_name[] = {
  "n_z", "n_r", "n_m",
  "output_sampling_rate",
  "n_t", "T", "w_active_min", "w_active_max",
  "Z", "R",
  "press",
  "p_av", "rep", "fwhm", "l_0", "ceo", "waist",
  "laser_rel_tol",
  "read_in_laser_pulse",
  "path_A_w_R_initial", "path_A_w_I_initial", "path_w_active_initial",
  "original_n_z",
  "HHGP_starting_z",
  "ending_n_z",
  "interp_points",
  "gas_pressure_profile",
  "HHG_lambda_min",
  "HHG_lambda_max",
  "HHP_E_min",
  "HHP_E_max",
  "flush_HHP_at_output_step",
  "output_electron_density",
  "pend_path",
  "path_input_j0",
  "path_A_w_R", "path_A_w_I", "path_w_active",
  "path_electron_density",
  "path_HHG_R", "path_HHG_I", "path_HHG_w", "path_HHG_E",
  "path_HHP_R", "path_HHP_I", "path_HHP_w",
  "path_config_file", "path_config_log"
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
              set_variable(input_name_char, input_value_char, input_description_char, print_to_screen);
            }
    }
    if (config_file.eof() && print_to_screen == true) {
      std::cout << "Config_Settings::read_in: Info, " << lines_found << " lines found and " << lines_read << " lines read in from config file!\n";
    }
  } else if (print_to_screen == true) {
    std::cout << "Config_Settings::read_in: Error, could not open config file, using default values instead...\n";
  }
  config_file.close();
}

void Config_Settings::set_variable(std::string& variable_name, std::string& variable_value_str, std::string& input_description_char, bool print_to_screen) {
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

      case SN::n_z :
        n_z_set(std::stoi(variable_value_str));
        n_z_description_set(input_description_char);
        break;
      case SN::n_r :
        n_r_set(std::stoi(variable_value_str));
        n_r_description_set(input_description_char);
        break;
      case SN::n_m :
        n_m_set(std::stoi(variable_value_str));
        n_m_description_set(input_description_char);
        break;

      case SN::output_sampling_rate :
        output_sampling_rate_set(std::stoi(variable_value_str));
        output_sampling_rate_description_set(input_description_char);
        break;

      case SN::n_t :
        n_t_set(std::stoi(variable_value_str));
        n_t_description_set(input_description_char);
        break;
      case SN::T :
        T_set(std::stod(variable_value_str));
        T_description_set(input_description_char);
        break;
      case SN::w_active_min :
        w_active_min_set(std::stod(variable_value_str));
        w_active_min_description_set(input_description_char);
        break;
      case SN::w_active_max :
        w_active_max_set(std::stod(variable_value_str));
        w_active_max_description_set(input_description_char);
        break;

      case SN::Z :
        Z_set(std::stod(variable_value_str));
        Z_description_set(input_description_char);
        break;
      case SN::R :
        R_set(std::stod(variable_value_str));
        R_description_set(input_description_char);
        break;

      case SN::press :
        press_set(std::stod(variable_value_str));
        press_description_set(input_description_char);
        break;

      case SN::p_av :
        p_av_set(std::stod(variable_value_str));
        p_av_description_set(input_description_char);
        break;
      case SN::rep :
        rep_set(std::stod(variable_value_str));
        rep_description_set(input_description_char);
        break;
      case SN::fwhm :
        fwhm_set(std::stod(variable_value_str));
        fwhm_description_set(input_description_char);
        break;
      case SN::l_0 :
        l_0_set(std::stod(variable_value_str));
        l_0_description_set(input_description_char);
        break;
      case SN::ceo :
        ceo_set(std::stod(variable_value_str));
        ceo_description_set(input_description_char);
        break;
      case SN::waist :
        waist_set(std::stod(variable_value_str));
        waist_description_set(input_description_char);
        break;

      case SN::laser_rel_tol :
        laser_rel_tol_set(std::stod(variable_value_str));
        laser_rel_tol_description_set(input_description_char);
        break;

      case SN::read_in_laser_pulse :
        read_in_laser_pulse_set(std::stoi(variable_value_str));
        read_in_laser_pulse_description_set(input_description_char);
        break;
      case SN::path_A_w_R_initial :
        path_A_w_R_initial_set(variable_value_str);
        path_A_w_R_initial_description_set(input_description_char);
        break;
      case SN::path_A_w_I_initial :
        path_A_w_I_initial_set(variable_value_str);
        path_A_w_I_initial_description_set(input_description_char);
        break;
      case SN::path_w_active_initial :
        path_w_active_initial_set(variable_value_str);
        path_w_active_initial_description_set(input_description_char);
        break;
      case SN::original_n_z :
        original_n_z_set(std::stoi(variable_value_str));
        original_n_z_description_set(input_description_char);
        break;

      case SN::HHGP_starting_z :
        HHGP_starting_z_set(std::stod(variable_value_str));
        HHGP_starting_z_description_set(input_description_char);
        break;

      case SN::ending_n_z :
        ending_n_z_set(std::stoi(variable_value_str));
        ending_n_z_description_set(input_description_char);
        break;

      case SN::interp_points :
        interp_points_set(std::stoi(variable_value_str));
        interp_points_description_set(input_description_char);
        break;

      case SN::gas_pressure_profile:
        gas_pressure_profile_set(variable_value_str);
        gas_pressure_profile_description_set(input_description_char);
        break;

      case SN::HHG_lambda_min :
        HHG_lambda_min_set(std::stod(variable_value_str));
        HHG_lambda_min_description_set(input_description_char);
        break;
      case SN::HHG_lambda_max :
        HHG_lambda_max_set(std::stod(variable_value_str));
        HHG_lambda_max_description_set(input_description_char);
        break;
      case SN::HHP_E_min :
        HHP_E_min_set(std::stod(variable_value_str));
        HHP_E_min_description_set(input_description_char);
        break;
      case SN::HHP_E_max :
        HHP_E_max_set(std::stod(variable_value_str));
        HHP_E_max_description_set(input_description_char);
        break;

      case SN::flush_HHP_at_output_step :
        flush_HHP_at_output_step_set(std::stoi(variable_value_str));
        flush_HHP_at_output_step_description_set(input_description_char);

      case SN:: output_electron_density:
        output_electron_density_set(std::stoi(variable_value_str));
        output_electron_density_description_set(input_description_char);
        break;

      case SN::pend_path :
        pend_path_set(variable_value_str);
        pend_path_description_set(input_description_char);
        break;

      case SN::path_input_j0 :
        path_input_j0_set(variable_value_str);
        path_input_j0_description_set(input_description_char);
        break;

      case SN::path_A_w_R :
        path_A_w_R_set(variable_value_str);
        path_A_w_R_description_set(input_description_char);
        break;
      case SN::path_A_w_I :
        path_A_w_I_set(variable_value_str);
        path_A_w_I_description_set(input_description_char);
        break;
      case SN::path_w_active :
        path_w_active_set(variable_value_str);
        path_w_active_description_set(input_description_char);
        break;
      case SN::path_electron_density :
        path_electron_density_set(variable_value_str);
        path_electron_density_description_set(input_description_char);
        break;

      case SN::path_HHG_R :
        path_HHG_R_set(variable_value_str);
        path_HHG_R_description_set(input_description_char);
        break;
      case SN::path_HHG_I :
        path_HHG_I_set(variable_value_str);
        path_HHG_I_description_set(input_description_char);
        break;
      case SN::path_HHG_w :
        path_HHG_w_set(variable_value_str);
        path_HHG_w_description_set(input_description_char);
        break;
      case SN::path_HHG_E :
        path_HHG_E_set(variable_value_str);
        path_HHG_E_description_set(input_description_char);
        break;

      case SN::path_HHP_R :
        path_HHP_R_set(variable_value_str);
        path_HHP_R_description_set(input_description_char);
        break;
      case SN::path_HHP_I :
        path_HHP_I_set(variable_value_str);
        path_HHP_I_description_set(input_description_char);
        break;
      case SN::path_HHP_w :
        path_HHP_w_set(variable_value_str);
        path_HHP_w_description_set(input_description_char);
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
  } else if (print_to_screen == true) {
    std::cout << "Config_Settings::set_variable: Error, unknown variable name '" << variable_name << "'\n";
  }
}

void Config_Settings::check_paths(bool print_to_screen) {
  int i = 0, j = 0, k = 0;
  std::string pending_string = std::to_string(static_cast<unsigned long long>(i)) + std::to_string(static_cast<unsigned long long>(j)) + std::to_string(static_cast<unsigned long long>(k));
  std::string path = set_path(path_config_log(), pending_string);

  bool unique_path = false;
  if (pend_path() != "false") {
    while (!unique_path) {
      if (std::ifstream(path)) {
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
        path = set_path(path_config_log(), pending_string);
      } else {
        if (print_to_screen == true) {
          std::cout << "Config_Settings::check_paths: Info, unique path " << path << " found!\n";
        }
        unique_path = true;
      }
    }
  }

  path_config_log_set(path);

  path = set_path(path_A_w_R(), pending_string);
  path_A_w_R_set(path);
  path = set_path(path_A_w_I(), pending_string);
  path_A_w_I_set(path);
  path = set_path(path_w_active(), pending_string);
  path_w_active_set(path);
  path = set_path(path_electron_density(), pending_string);
  path_electron_density_set(path);

  path = set_path(path_HHG_R(), pending_string);
  path_HHG_R_set(path);
  path = set_path(path_HHG_I(), pending_string);
  path_HHG_I_set(path);
  path = set_path(path_HHG_w(), pending_string);
  path_HHG_w_set(path);
  path = set_path(path_HHG_E(), pending_string);
  path_HHG_E_set(path);

  path = set_path(path_HHP_R(), pending_string);
  path_HHP_R_set(path);
  path = set_path(path_HHP_I(), pending_string);
  path_HHP_I_set(path);
  path = set_path(path_HHP_w(), pending_string);
  path_HHP_w_set(path);

}

void Config_Settings::step_path(int step, std::string variable) {

  pending_string_stepWorkings = std::to_string(static_cast<unsigned long long>(step));
  pending_string_len_stepWorkings = std::to_string(static_cast<unsigned long long>(step - 1)).length();
  path_A_R_stepWorkings = path_A_w_R();
  path_A_I_stepWorkings = path_A_w_I();
  path_w_stepWorkings = path_w_active();

  path_e_stepWorkings = path_electron_density();

  path_hhg_r_stepWorkings = path_HHG_R();
  path_hhg_i_stepWorkings = path_HHG_I();
  path_hhg_w_stepWorkings = path_HHG_w();
  path_hhg_E_stepWorkings = path_HHG_E();

  path_hhp_r_stepWorkings = path_HHP_R();
  path_hhp_i_stepWorkings = path_HHP_I();
  path_hhp_w_stepWorkings = path_HHP_w();


  found_stepWorkings = path_A_R_stepWorkings.find_last_of("/");
  tmp_stepWorkings = path_A_R_stepWorkings.substr(found_stepWorkings+1);
  count_underscore_stepWorkings = std::count(tmp_stepWorkings.begin(), tmp_stepWorkings.end(), '_');

  if (found_stepWorkings == -1) {
    if (variable == "UPPE_A_w") {
      path_A_R_stepWorkings = path_A_R_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_A_R_stepWorkings.substr(3);
      path_A_I_stepWorkings = path_A_I_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_A_I_stepWorkings.substr(3);
      path_A_w_R_step_set(path_A_R_stepWorkings);
      path_A_w_I_step_set(path_A_I_stepWorkings);
    } else if (variable == "HHG_A_w") {
      path_hhg_r_stepWorkings = path_hhg_r_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_hhg_r_stepWorkings.substr(3);
      path_hhg_i_stepWorkings = path_hhg_i_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_hhg_i_stepWorkings.substr(3);
      path_HHG_R_step_set(path_hhg_r_stepWorkings);
      path_HHG_I_step_set(path_hhg_i_stepWorkings);
    } else if (variable == "HHP_A_w") {
      path_hhp_r_stepWorkings = path_hhp_r_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_hhp_r_stepWorkings.substr(3);
      path_hhp_i_stepWorkings = path_hhp_i_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_hhp_i_stepWorkings.substr(3);
      path_HHP_R_step_set(path_hhp_r_stepWorkings);
      path_HHP_I_step_set(path_hhp_i_stepWorkings);
    } else if (variable == "UPPE_electron_density") {
      path_e_stepWorkings = path_e_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_e_stepWorkings.substr(3);
      path_electron_density_step_set(path_e_stepWorkings);
    } else if (variable == "HHG_electric_field") {
      path_hhg_E_stepWorkings = path_hhg_E_stepWorkings.substr(0, 3) + "_" + pending_string_stepWorkings + "_" + path_hhg_E_stepWorkings.substr(3);
      path_HHG_E_step_set(path_hhg_E_stepWorkings);
    }
  } else {
    if (variable == "UPPE_A_w") {
      path_A_R_stepWorkings = path_A_R_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_A_R_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_A_I_stepWorkings = path_A_I_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_A_I_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_A_w_R_step_set(path_A_R_stepWorkings);
      path_A_w_I_step_set(path_A_I_stepWorkings);
    } else if (variable == "HHG_A_w") {
      path_hhg_r_stepWorkings = path_hhg_r_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_hhg_r_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_hhg_i_stepWorkings = path_hhg_i_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_hhg_i_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_HHG_R_step_set(path_hhg_r_stepWorkings);
      path_HHG_I_step_set(path_hhg_i_stepWorkings);
    } else if (variable == "HHP_A_w") {
      path_hhp_r_stepWorkings = path_hhp_r_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_hhp_r_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_hhp_i_stepWorkings = path_hhp_i_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_hhp_i_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_HHP_R_step_set(path_hhp_r_stepWorkings);
      path_HHP_I_step_set(path_hhp_i_stepWorkings);
    } else if (variable == "UPPE_electron_density") {
      path_e_stepWorkings = path_e_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_e_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_electron_density_step_set(path_e_stepWorkings);
    } else if (variable == "HHG_electric_field") {
      path_hhg_E_stepWorkings = path_hhg_E_stepWorkings.substr(0, found_stepWorkings+1+3) + "_" + pending_string_stepWorkings + "_" + path_hhg_E_stepWorkings.substr(found_stepWorkings+1+3+1);
      path_HHG_E_step_set(path_hhg_E_stepWorkings);
    }
  }
}

std::string Config_Settings::set_path(std::string path, std::string pending_string, std::string pend) {
  if (pend == "") pend = pend_path();
  if (pend == "prepend") {
    return set_pre_path(pending_string, path);
  } else if (pend == "postpend") {
    return set_post_path(path, pending_string);
  } else if (pend == "false") {
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
    std::cout << "   n_z:                   " << n_z() << "          " << n_z_description() << std::endl;
    std::cout << "   n_r:                   " << n_r() << "          " << n_r_description() << std::endl;
    std::cout << "   n_m:                   " << n_m() << "          " << n_m_description() << std::endl;
    std::cout << "   output_sampling_rate:  " << output_sampling_rate() << "          " << output_sampling_rate_description() << std::endl;
    std::cout << "   n_t:                   " << n_t() << "       " << n_t_description() << std::endl;
    std::cout << "   T:                     " << T() << "       " << T_description() << std::endl;
    std::cout << "   w_active_min:          " << w_active_min() << "       " << w_active_min_description() << std::endl;
    std::cout << "   w_active_max:          " << w_active_max() << "       " << w_active_max_description() << std::endl;
    std::cout << "   Z:                     " << Z() << "        " << Z_description() << std::endl;
    std::cout << "   R:                     " << R() << "     " << R_description() << std::endl;
    std::cout << "   press:                 " << press() << "        " << press_description() << std::endl;
    std::cout << "   p_av:                  " << p_av() << "         " << p_av_description() << std::endl;
    std::cout << "   rep:                   " << rep() << "        " << rep_description() << std::endl;
    std::cout << "   fwhm:                  " << fwhm() << "       " << fwhm_description() << std::endl;
    std::cout << "   l_0:                   " << l_0() << "       " << l_0_description() << std::endl;
    std::cout << "   ceo:                   " << ceo() << "           " << ceo_description() << std::endl;
    std::cout << "   waist:                 " << waist() << "     " << waist_description() << std::endl;
    std::cout << "   laser_rel_tol:         " << laser_rel_tol() << "      " << laser_rel_tol_description() << std::endl;
    std::cout << "   read_in_laser_pulse:     " << read_in_laser_pulse() << "           " << read_in_laser_pulse_description() << std::endl;
    std::cout << "   original_n_z:            " << original_n_z() << "          " << original_n_z_description() << std::endl;
    std::cout << "   HHGP_starting_z:         " << HHGP_starting_z() << "           " << HHGP_starting_z_description() << std::endl;
    std::cout << "   ending_n_z:              " << ending_n_z() << "          " << ending_n_z_description() << std::endl;
    std::cout << "   interp_points:           " << interp_points() << "           " << interp_points_description() << std::endl;
    std::cout << "   gas_pressure_profile:    " << gas_pressure_profile() << "   " << gas_pressure_profile_description() << std::endl;
    std::cout << "   HHG_lambda_min:          " << HHG_lambda_min() << "   " << HHG_lambda_min_description() << std::endl;
    std::cout << "   HHG_lambda_max:          " << HHG_lambda_max() << "   " << HHG_lambda_max_description() << std::endl;
    std::cout << "   HHP_E_min:               " << HHP_E_min() << "   " << HHP_E_min_description() << std::endl;
    std::cout << "   HHP_E_max:               " << HHP_E_max() << "   " << HHP_E_max_description() << std::endl;
    std::cout << "   flush_HHP_at_output_step " << flush_HHP_at_output_step() << "   " << flush_HHP_at_output_step_description() << std::endl;
    std::cout << "   output_electron_density: " << output_electron_density() << "   " << output_electron_density_description() << std::endl;
    std::cout << "   pend_path                " << pend_path() << "     " << pend_path_description() << std::endl;
    std::cout << "   path_input_j0:         " << path_input_j0() << "     " << path_input_j0_description() << std::endl;
    std::cout << "   path_A_w_R:            " << path_A_w_R() << "      " << path_A_w_R_description() << std::endl;
    std::cout << "   path_A_w_I:            " << path_A_w_I() << "      " << path_A_w_I_description() << std::endl;
    std::cout << "   path_w_active:         " << path_w_active() << "   " << path_w_active_description() << std::endl;
    std::cout << "   path_electron_density: " << path_electron_density() << "   " << path_electron_density_description() << std::endl;
    std::cout << "   path_HHG_R:       " << path_HHG_R() << "          " << path_HHG_R_description() << std::endl;
    std::cout << "   path_HHG_I:       " << path_HHG_I() << "          " << path_HHG_I_description() << std::endl;
    std::cout << "   path_HHG_w:       " << path_HHG_w() << "          " << path_HHG_w_description() << std::endl;
    std::cout << "   path_HHG_E:       " << path_HHG_E() << "          " << path_HHG_E_description() << std::endl;
    std::cout << "   path_HHP_R:       " << path_HHP_R() << "          " << path_HHP_R_description() << std::endl;
    std::cout << "   path_HHP_I:       " << path_HHP_I() << "          " << path_HHP_I_description() << std::endl;
    std::cout << "   path_HHP_w:       " << path_HHP_w() << "          " << path_HHP_w_description() << std::endl;
    std::cout << "   path_config_file: " << path_config_file() << "   " << path_config_file_description() << std::endl;
    std::cout << "   path_config_log:  " << path_config_log() << "     " << path_config_log_description() << std::endl;
    std::cout << "-------------------------------------------------------------------------------\n";
}

void Config_Settings::print(std::string path_) {
    std::ofstream config_log;
    config_log.open(path_);
    if (config_log.is_open()) {
      config_log << "{n_z} {" << n_z() << "} {" << n_z_description() << "}\n";
      config_log << "{n_r} {" << n_r() << "} {" << n_r_description() << "}\n";
      config_log << "{n_m} {" << n_m() << "} {" << n_m_description() << "}\n";

      config_log << "{output_sampling_rate} {" << output_sampling_rate() << "} {" << output_sampling_rate_description() << "}\n";

      config_log << "{n_t} {" << n_t() << "} {" << n_t_description() << "}\n";
      config_log << "{T} {" << T() << "} {" << T_description() << "}\n";
      config_log << "{w_active_min} {" << w_active_min() << "} {" << w_active_min_description() << "}\n";
      config_log << "{w_active_max} {" << w_active_max() << "} {" << w_active_max_description() << "}\n";

      config_log << "{Z} {" << Z() << "} {" << Z_description() << "}\n";
      config_log << "{R} {" << R() << "} {" << R_description() << "}\n";

      config_log << "{press} {" << press() << "} {" << press_description() << "}\n";

      config_log << "{p_av} {" << p_av() << "} {" << p_av_description() << "}\n";
      config_log << "{rep} {" << rep() << "} {" << rep_description() << "}\n";
      config_log << "{fwhm} {" << fwhm() << "} {" << fwhm_description() << "}\n";
      config_log << "{l_0} {" << l_0() << "} {" << l_0_description() << "}\n";
      config_log << "{ceo} {" << ceo() << "} {" << ceo_description() << "}\n";
      config_log << "{waist} {" << waist() << "} {" << waist_description() << "}\n";

      config_log << "{laser_rel_tol} {" << laser_rel_tol() << "} {" << laser_rel_tol_description() << "}\n";

      config_log << "{read_in_laser_pulse} {" << read_in_laser_pulse() << "} {" << read_in_laser_pulse_description() << "}\n";
      config_log << "{original_n_z} {" << original_n_z() << "} {" << original_n_z_description() << "}\n";
      config_log << "{HHGP_starting_z} {" << HHGP_starting_z() << "} {" << HHGP_starting_z_description() << "}\n";
      config_log << "{ending_n_z} {" << ending_n_z() << "} {" << ending_n_z_description() << "}\n";
      config_log << "{interp_points} {" << interp_points() << "} {" << interp_points_description() << "}\n";

      config_log << "{gas_pressure_profile} {" << gas_pressure_profile() << "} {" << gas_pressure_profile_description() << "}\n";

      config_log << "{HHG_lambda_min} {" << HHG_lambda_min() << "} {" << HHG_lambda_min_description() << "}\n";
      config_log << "{HHG_lambda_max} {" << HHG_lambda_max() << "} {" << HHG_lambda_max_description() << "}\n";
      config_log << "{HHP_E_min} {" << HHP_E_min() << "} {" << HHP_E_min_description() << "}\n";
      config_log << "{HHP_E_max} {" << HHP_E_max() << "} {" << HHP_E_max_description() << "}\n";

      config_log << "{flush_HHP_at_output_step} {" << flush_HHP_at_output_step() << "} {" << flush_HHP_at_output_step_description() << "}\n";

      config_log << "{output_electron_density} {" << output_electron_density() << "} {" << output_electron_density_description() << "}\n";

      config_log << "{pend_path} {" << pend_path() << "} {" << pend_path_description() << "}\n";

      config_log << "{path_input_j0} {" << path_input_j0() << "} {" << path_input_j0_description() << "}\n";

      config_log << "{path_A_w_R} {" << path_A_w_R() << "} {" << path_A_w_R_description() << "}\n";
      config_log << "{path_A_w_I} {" << path_A_w_I() << "} {" << path_A_w_I_description() << "}\n";
      config_log << "{path_electron_density} {" << path_electron_density() << "} {" << path_electron_density_description() << "}\n";
      config_log << "{path_w_active} {" << path_w_active() << "} {" << path_w_active_description() << "}\n";
      config_log << "{path_HHG_R} {" << path_HHG_R() << "} {" << path_HHG_R_description() << "}\n";
      config_log << "{path_HHG_I} {" << path_HHG_I() << "} {" << path_HHG_I_description() << "}\n";
      config_log << "{path_HHG_w} {" << path_HHG_w() << "} {" << path_HHG_w_description() << "}\n";
      config_log << "{path_HHG_E} {" << path_HHG_E() << "} {" << path_HHG_E_description() << "}\n";
      config_log << "{path_HHP_R} {" << path_HHP_R() << "} {" << path_HHP_R_description() << "}\n";
      config_log << "{path_HHP_I} {" << path_HHP_I() << "} {" << path_HHP_I_description() << "}\n";
      config_log << "{path_HHP_w} {" << path_HHP_w() << "} {" << path_HHP_w_description() << "}\n";

      config_log << "{path_config_file} {" << path_config_file() << "} {" << path_config_file_description() << "}\n";
      config_log << "{path_config_log} {" << path_config_log() << "} {" << path_config_log_description() << "}\n";

      config_log.close();

      std::cout << "Config_Settings::print: Successfully wrote config log to " << path_ << std::endl;

    } else {
      std::cout << "Config_Settings::print: Error, could not print to config log at " << path_ << std::endl;
    }
}

int Config_Settings::n_z() { return n_z_; }
void Config_Settings::n_z_set(int value) { n_z_ = value; }
std::string Config_Settings::n_z_description() { return n_z_description_; }
void Config_Settings::n_z_description_set(std::string description) { n_z_description_ = description; }

int Config_Settings::n_r() { return n_r_; }
void Config_Settings::n_r_set(int value) { n_r_ = value; }
std::string Config_Settings::n_r_description() { return n_r_description_; }
void Config_Settings::n_r_description_set(std::string description) { n_r_description_ = description; }

int Config_Settings::n_m() { return n_m_; }
void Config_Settings::n_m_set(int value) { n_m_ = value; }
std::string Config_Settings::n_m_description() { return n_m_description_; }
void Config_Settings::n_m_description_set(std::string description) { n_m_description_ = description; }


int Config_Settings::output_sampling_rate() { return output_sampling_rate_; }
void Config_Settings::output_sampling_rate_set(int value) { output_sampling_rate_ = value; }
std::string Config_Settings::output_sampling_rate_description() { return output_sampling_rate_description_; }
void Config_Settings::output_sampling_rate_description_set(std::string description) { output_sampling_rate_description_ = description; }


int Config_Settings::n_t() { return n_t_; }
void Config_Settings::n_t_set(int value) { n_t_ = value; }
std::string Config_Settings::n_t_description() { return n_t_description_; }
void Config_Settings::n_t_description_set(std::string description) { n_t_description_ = description; }

double Config_Settings::T() { return T_; }
void Config_Settings::T_set(double value) { T_ = value; }
std::string Config_Settings::T_description() { return T_description_; }
void Config_Settings::T_description_set(std::string description) { T_description_ = description; }

double Config_Settings::w_active_min() { return w_active_min_; }
void Config_Settings::w_active_min_set(double value) { w_active_min_ = value; }
std::string Config_Settings::w_active_min_description() { return w_active_min_description_; }
void Config_Settings::w_active_min_description_set(std::string description) { w_active_min_description_ = description; }

double Config_Settings::w_active_max() { return w_active_max_; }
void Config_Settings::w_active_max_set(double value) { w_active_max_ = value; }
std::string Config_Settings::w_active_max_description() { return w_active_max_description_; }
void Config_Settings::w_active_max_description_set(std::string description) { w_active_max_description_ = description; }

double Config_Settings::Z() { return Z_; }
void Config_Settings::Z_set(double value) { Z_ = value; }
std::string Config_Settings::Z_description() { return Z_description_; }
void Config_Settings::Z_description_set(std::string description) { Z_description_ = description; }

double Config_Settings::R() { return R_; }
void Config_Settings::R_set(double value) { R_ = value; }
std::string Config_Settings::R_description() { return R_description_; }
void Config_Settings::R_description_set(std::string description) { R_description_ = description; }

double Config_Settings::press() { return press_; }
void Config_Settings::press_set(double value) { press_ = value; }
std::string Config_Settings::press_description() { return press_description_; }
void Config_Settings::press_description_set(std::string description) { press_description_ = description; }

double Config_Settings::p_av() { return p_av_; }
void Config_Settings::p_av_set(double value) { p_av_ = value; }
std::string Config_Settings::p_av_description() { return p_av_description_; }
void Config_Settings::p_av_description_set(std::string description) { p_av_description_ = description; }

double Config_Settings::rep() { return rep_; }
void Config_Settings::rep_set(double value) { rep_ = value; }
std::string Config_Settings::rep_description() { return rep_description_; }
void Config_Settings::rep_description_set(std::string description) { rep_description_ = description; }

double Config_Settings::fwhm() { return fwhm_; }
void Config_Settings::fwhm_set(double value) { fwhm_ = value; }
std::string Config_Settings::fwhm_description() { return fwhm_description_; }
void Config_Settings::fwhm_description_set(std::string description) { fwhm_description_ = description; }

double Config_Settings::l_0() { return l_0_; }
void Config_Settings::l_0_set(double value) { l_0_ = value; }
std::string Config_Settings::l_0_description() { return l_0_description_; }
void Config_Settings::l_0_description_set(std::string description) { l_0_description_ = description; }

double Config_Settings::ceo() { return ceo_; }
void Config_Settings::ceo_set(double value) { ceo_ = value; }
std::string Config_Settings::ceo_description() { return ceo_description_; }
void Config_Settings::ceo_description_set(std::string description) { ceo_description_ = description; }

double Config_Settings::waist() { return waist_; }
void Config_Settings::waist_set(double value) { waist_ = value; }
std::string Config_Settings::waist_description() { return waist_description_; }
void Config_Settings::waist_description_set(std::string description) { waist_description_ = description; }


double Config_Settings::laser_rel_tol() { return laser_rel_tol_; }
void Config_Settings::laser_rel_tol_set(double value) { laser_rel_tol_ = value; }
std::string Config_Settings::laser_rel_tol_description() { return laser_rel_tol_description_; }
void Config_Settings::laser_rel_tol_description_set(std::string description) { laser_rel_tol_description_ = description; }


int Config_Settings::read_in_laser_pulse() { return read_in_laser_pulse_; }
void Config_Settings::read_in_laser_pulse_set(int value) { read_in_laser_pulse_ = value; }
std::string Config_Settings::read_in_laser_pulse_description() { return read_in_laser_pulse_description_; }
void Config_Settings::read_in_laser_pulse_description_set(std::string description) { read_in_laser_pulse_description_ = description; }

std::string Config_Settings::path_A_w_R_initial() { return path_A_w_R_initial_; }
void Config_Settings::path_A_w_R_initial_set(std::string value) { path_A_w_R_initial_ = value; }
std::string Config_Settings::path_A_w_R_initial_description() { return path_A_w_R_initial_description_; }
void Config_Settings::path_A_w_R_initial_description_set(std::string description) { path_A_w_R_initial_description_ = description; }

std::string Config_Settings::path_A_w_I_initial() { return path_A_w_I_initial_; }
void Config_Settings::path_A_w_I_initial_set(std::string value) { path_A_w_I_initial_ = value; }
std::string Config_Settings::path_A_w_I_initial_description() { return path_A_w_I_initial_description_; }
void Config_Settings::path_A_w_I_initial_description_set(std::string description) { path_A_w_I_initial_description_ = description; }

std::string Config_Settings::path_w_active_initial() { return path_w_active_initial_; }
void Config_Settings::path_w_active_initial_set(std::string value) { path_w_active_initial_ = value; }
std::string Config_Settings::path_w_active_initial_description() { return path_w_active_initial_description_; }
void Config_Settings::path_w_active_initial_description_set(std::string description) { path_w_active_initial_description_ = description; }

int Config_Settings::original_n_z() { return original_n_z_; }
void Config_Settings::original_n_z_set(int value) { original_n_z_ = value; }
std::string Config_Settings::original_n_z_description() { return original_n_z_description_; }
void Config_Settings::original_n_z_description_set(std::string description) { original_n_z_description_ = description; }

double Config_Settings::HHGP_starting_z() { return HHGP_starting_z_; }
void Config_Settings::HHGP_starting_z_set(double value) { HHGP_starting_z_ = value; }
std::string Config_Settings::HHGP_starting_z_description() { return HHGP_starting_z_description_; }
void Config_Settings::HHGP_starting_z_description_set(std::string description) { HHGP_starting_z_description_ = description; }

int Config_Settings::ending_n_z() { return ending_n_z_; }
void Config_Settings::ending_n_z_set(int value) { ending_n_z_ = value; }
std::string Config_Settings::ending_n_z_description() { return ending_n_z_description_; }
void Config_Settings::ending_n_z_description_set(std::string description) { ending_n_z_description_ = description; }

int Config_Settings::interp_points() { return interp_points_; }
void Config_Settings::interp_points_set(int value) { interp_points_ = value; }
std::string Config_Settings::interp_points_description() { return interp_points_description_; }
void Config_Settings::interp_points_description_set(std::string description) { interp_points_description_ = description; }


std::string Config_Settings::gas_pressure_profile() { return gas_pressure_profile_; }
void Config_Settings::gas_pressure_profile_set(std::string value) { gas_pressure_profile_ = value; }
std::string Config_Settings::gas_pressure_profile_description() { return gas_pressure_profile_description_; }
void Config_Settings::gas_pressure_profile_description_set(std::string description) { gas_pressure_profile_description_ = description; }


double Config_Settings::HHG_lambda_min() { return HHG_lambda_min_; }
void Config_Settings::HHG_lambda_min_set(double value) { HHG_lambda_min_ = value; }
std::string Config_Settings::HHG_lambda_min_description() { return HHG_lambda_min_description_; }
void Config_Settings::HHG_lambda_min_description_set(std::string description) { HHG_lambda_min_description_ = description; }

double Config_Settings::HHG_lambda_max() { return HHG_lambda_max_; }
void Config_Settings::HHG_lambda_max_set(double value) { HHG_lambda_max_ = value; }
std::string Config_Settings::HHG_lambda_max_description() { return HHG_lambda_max_description_; }
void Config_Settings::HHG_lambda_max_description_set(std::string description) { HHG_lambda_max_description_ = description; }

double Config_Settings::HHP_E_min() { return HHP_E_min_; }
void Config_Settings::HHP_E_min_set(double value) { HHP_E_min_ = value; }
std::string Config_Settings::HHP_E_min_description() { return HHP_E_min_description_; }
void Config_Settings::HHP_E_min_description_set(std::string description) { HHP_E_min_description_ = description; }

double Config_Settings::HHP_E_max() { return HHP_E_max_; }
void Config_Settings::HHP_E_max_set(double value) { HHP_E_max_ = value; }
std::string Config_Settings::HHP_E_max_description() { return HHP_E_max_description_; }
void Config_Settings::HHP_E_max_description_set(std::string description) { HHP_E_max_description_ = description; }

int Config_Settings::flush_HHP_at_output_step() { return flush_HHP_at_output_step_; }
void Config_Settings::flush_HHP_at_output_step_set(int value) { flush_HHP_at_output_step_ = value; }
std::string Config_Settings::flush_HHP_at_output_step_description() { return flush_HHP_at_output_step_description_; }
void Config_Settings::flush_HHP_at_output_step_description_set(std::string description) { flush_HHP_at_output_step_description_ = description; }

int Config_Settings::output_electron_density() { return output_electron_density_; }
void Config_Settings::output_electron_density_set(int value) { output_electron_density_ = value; }
std::string Config_Settings::output_electron_density_description() { return output_electron_density_description_; }
void Config_Settings::output_electron_density_description_set(std::string description) { output_electron_density_description_ = description; }


std::string Config_Settings::pend_path() { return pend_path_; }
void Config_Settings::pend_path_set(std::string value) { pend_path_ = value; }
std::string Config_Settings::pend_path_description() { return pend_path_description_; }
void Config_Settings::pend_path_description_set(std::string description) { pend_path_description_ = description; }

std::string Config_Settings::path_input_j0() { return path_input_j0_; }
void Config_Settings::path_input_j0_set(std::string value) { path_input_j0_ = value; }
std::string Config_Settings::path_input_j0_description() { return path_input_j0_description_; }
void Config_Settings::path_input_j0_description_set(std::string description) { path_input_j0_description_ = description; }


std::string Config_Settings::path_A_w_R() { return path_A_w_R_; }
void Config_Settings::path_A_w_R_set(std::string value) { path_A_w_R_ = value; }
std::string Config_Settings::path_A_w_R_description() { return path_A_w_R_description_; }
void Config_Settings::path_A_w_R_description_set(std::string description) { path_A_w_R_description_ = description; }

std::string Config_Settings::path_A_w_I() { return path_A_w_I_; }
void Config_Settings::path_A_w_I_set(std::string value) { path_A_w_I_ = value; }
std::string Config_Settings::path_A_w_I_description() { return path_A_w_I_description_; }
void Config_Settings::path_A_w_I_description_set(std::string description) { path_A_w_I_description_ = description; }

std::string Config_Settings::path_w_active() { return path_w_active_; }
void Config_Settings::path_w_active_set(std::string value) { path_w_active_ = value; }
std::string Config_Settings::path_w_active_description() { return path_w_active_description_; }
void Config_Settings::path_w_active_description_set(std::string description) { path_w_active_description_ = description; }

std::string Config_Settings::path_electron_density() { return path_electron_density_; }
void Config_Settings::path_electron_density_set(std::string value) { path_electron_density_ = value; }
std::string Config_Settings::path_electron_density_description() { return path_electron_density_description_; }
void Config_Settings::path_electron_density_description_set(std::string description) { path_electron_density_description_ = description; }


std::string Config_Settings::path_A_w_R_step() { return path_A_w_R_step_; }
void Config_Settings::path_A_w_R_step_set(std::string value) { path_A_w_R_step_ = value; }
std::string Config_Settings::path_A_w_R_description_step() { return path_A_w_R_description_step_; }
void Config_Settings::path_A_w_R_description_step_set(std::string description) { path_A_w_R_description_step_ = description; }

std::string Config_Settings::path_A_w_I_step() { return path_A_w_I_step_; }
void Config_Settings::path_A_w_I_step_set(std::string value) { path_A_w_I_step_ = value; }
std::string Config_Settings::path_A_w_I_description_step() { return path_A_w_I_description_step_; }
void Config_Settings::path_A_w_I_description_step_set(std::string description) { path_A_w_I_description_step_ = description; }

std::string Config_Settings::path_w_active_step() { return path_w_active_step_; }
void Config_Settings::path_w_active_step_set(std::string value) { path_w_active_step_ = value; }
std::string Config_Settings::path_w_active_description_step() { return path_w_active_description_step_; }
void Config_Settings::path_w_active_description_step_set(std::string description) { path_w_active_description_step_ = description; }

std::string Config_Settings::path_electron_density_step() { return path_electron_density_step_; }
void Config_Settings::path_electron_density_step_set(std::string value) { path_electron_density_step_ = value; }
std::string Config_Settings::path_electron_density_description_step() { return path_electron_density_description_step_; }
void Config_Settings::path_electron_density_description_step_set(std::string description) { path_electron_density_description_step_ = description; }


std::string Config_Settings::path_HHG_R() { return path_HHG_R_; }
void Config_Settings::path_HHG_R_set(std::string value) { path_HHG_R_ = value; }
std::string Config_Settings::path_HHG_R_description() { return path_HHG_R_description_; }
void Config_Settings::path_HHG_R_description_set(std::string description) { path_HHG_R_description_ = description; }

std::string Config_Settings::path_HHG_I() { return path_HHG_I_; }
void Config_Settings::path_HHG_I_set(std::string value) { path_HHG_I_ = value; }
std::string Config_Settings::path_HHG_I_description() { return path_HHG_I_description_; }
void Config_Settings::path_HHG_I_description_set(std::string description) { path_HHG_I_description_ = description; }

std::string Config_Settings::path_HHG_w() { return path_HHG_w_; }
void Config_Settings::path_HHG_w_set(std::string value) { path_HHG_w_ = value; }
std::string Config_Settings::path_HHG_w_description() { return path_HHG_w_description_; }
void Config_Settings::path_HHG_w_description_set(std::string description) { path_HHG_w_description_ = description; }

std::string Config_Settings::path_HHG_E() { return path_HHG_E_; }
void Config_Settings::path_HHG_E_set(std::string value) { path_HHG_E_ = value; }
std::string Config_Settings::path_HHG_E_description() { return path_HHG_E_description_; }
void Config_Settings::path_HHG_E_description_set(std::string description) { path_HHG_E_description_ = description; }


std::string Config_Settings::path_HHP_R() { return path_HHP_R_; }
void Config_Settings::path_HHP_R_set(std::string value) { path_HHP_R_ = value; }
std::string Config_Settings::path_HHP_R_description() { return path_HHP_R_description_; }
void Config_Settings::path_HHP_R_description_set(std::string description) { path_HHP_R_description_ = description; }

std::string Config_Settings::path_HHP_I() { return path_HHP_I_; }
void Config_Settings::path_HHP_I_set(std::string value) { path_HHP_I_ = value; }
std::string Config_Settings::path_HHP_I_description() { return path_HHP_I_description_; }
void Config_Settings::path_HHP_I_description_set(std::string description) { path_HHP_I_description_ = description; }

std::string Config_Settings::path_HHP_w() { return path_HHP_w_; }
void Config_Settings::path_HHP_w_set(std::string value) { path_HHP_w_ = value; }
std::string Config_Settings::path_HHP_w_description() { return path_HHP_w_description_; }
void Config_Settings::path_HHP_w_description_set(std::string description) { path_HHP_w_description_ = description; }


std::string Config_Settings::path_HHG_R_step() { return path_HHG_R_step_; }
void Config_Settings::path_HHG_R_step_set(std::string value) { path_HHG_R_step_ = value; }
std::string Config_Settings::path_HHG_R_description_step() { return path_HHG_R_description_step_; }
void Config_Settings::path_HHG_R_description_step_set(std::string description) { path_HHG_R_description_step_ = description; }

std::string Config_Settings::path_HHG_I_step() { return path_HHG_I_step_; }
void Config_Settings::path_HHG_I_step_set(std::string value) { path_HHG_I_step_ = value; }
std::string Config_Settings::path_HHG_I_description_step() { return path_HHG_I_description_step_; }
void Config_Settings::path_HHG_I_description_step_set(std::string description) { path_HHG_I_description_step_ = description; }

std::string Config_Settings::path_HHG_w_step() { return path_HHG_w_step_; }
void Config_Settings::path_HHG_w_step_set(std::string value) { path_HHG_w_step_ = value; }
std::string Config_Settings::path_HHG_w_description_step() { return path_HHG_w_description_step_; }
void Config_Settings::path_HHG_w_description_step_set(std::string description) { path_HHG_w_description_step_ = description; }

std::string Config_Settings::path_HHG_E_step() { return path_HHG_E_step_; }
void Config_Settings::path_HHG_E_step_set(std::string value) { path_HHG_E_step_ = value; }
std::string Config_Settings::path_HHG_E_description_step() { return path_HHG_E_description_step_; }
void Config_Settings::path_HHG_E_description_step_set(std::string description) { path_HHG_E_description_step_ = description; }


std::string Config_Settings::path_HHP_R_step() { return path_HHP_R_step_; }
void Config_Settings::path_HHP_R_step_set(std::string value) { path_HHP_R_step_ = value; }
std::string Config_Settings::path_HHP_R_description_step() { return path_HHP_R_description_step_; }
void Config_Settings::path_HHP_R_description_step_set(std::string description) { path_HHP_R_description_step_ = description; }

std::string Config_Settings::path_HHP_I_step() { return path_HHP_I_step_; }
void Config_Settings::path_HHP_I_step_set(std::string value) { path_HHP_I_step_ = value; }
std::string Config_Settings::path_HHP_I_description_step() { return path_HHP_I_description_step_; }
void Config_Settings::path_HHP_I_description_step_set(std::string description) { path_HHP_I_description_step_ = description; }


std::string Config_Settings::path_config_file() { return path_config_file_; }
void Config_Settings::path_config_file_set(std::string value) { path_config_file_ = value; }
std::string Config_Settings::path_config_file_description() { return path_config_file_description_; }
void Config_Settings::path_config_file_description_set(std::string description) { path_config_file_description_ = description; }

std::string Config_Settings::path_config_log() { return path_config_log_; }
void Config_Settings::path_config_log_set(std::string value) { path_config_log_ = value; }
std::string Config_Settings::path_config_log_description() { return path_config_log_description_; }
void Config_Settings::path_config_log_description_set(std::string description) { path_config_log_description_ = description; }

