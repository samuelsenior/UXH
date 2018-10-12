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

//------------------------------------------------------------------------------------------------//
//  Class implementation
//------------------------------------------------------------------------------------------------//
/*! Constructor */
Config_Settings::Config_Settings() {  }

//------------------------------------------------------------------------------------------------//

const char * Config_Settings::setting_name[] = {
  "n_z", "n_r", "n_m",
  "n_t", "T", "w_active_min", "w_active_max",
  "Z", "R",
  "press",
  "p_av", "rep", "fwhm", "l_0", "ceo", "waist",
  "pend_path",
  "path_input_j0",
  "path_A_w_R", "path_A_w_I", "path_w_active",
  "path_HHG_R", "path_HHG_I", "path_HHG_w", "path_HHG_E",
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
    } //while
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
  //std::string path = set_path(path_A_w_R(), pending_string);

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
        path = set_path(path_config_log(), pending_string);
      } else {
        if (print_to_screen == true) {
          std::cout << "Config_Settings::check_paths: Info, unique path " << path << " found!\n";
        }
        unique_path = true;
      }
    }
  }

  //path_A_w_R_set(path);
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

  //path = set_path(path_config_log(), pending_string);
  //path_config_log_set(path);
}

void Config_Settings::step_path(int step) {

  std::string pending_string = std::to_string(static_cast<unsigned long long>(step));
  std::string path_A_R = path_A_w_R();
  std::string path_A_I = path_A_w_I();
  std::string path_w = path_w_active();

  std::string path_e = path_electron_density();

  std::string path_hhg_r = path_HHG_R();
  std::string path_hhg_i = path_HHG_I();
  std::string path_hhg_w = path_HHG_w();
  std::string path_hhg_E = path_HHG_E();


  static std::size_t found = path_A_R.find_last_of("/");
  if (found == -1) {
    if (step == 1) {
      path_A_R = path_A_R.substr(0, 3) + "_" + pending_string + "_" + path_A_R.substr(3);
    } else if (step > 1 && step < 10) {
      path_A_R = pending_string + "_" + path_A_R.substr(1, path_A_R.size());
    } else if (step > 10) {
      path_A_R = pending_string + "_" + path_A_R.substr(2, path_A_R.size());
    }
  } else {
    if (step == 1) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+4);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+4);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+4);
      //path_A = path_A.substr(0, found+1) + pending_string + "_" + path_A.substr(found+1);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+4);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+4);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+4);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+4);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+4);
    } else if (step > 1 && step < 10) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+6);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+6);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+6);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+6);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+6);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+6);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+6);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+6);
    } else if (step == 10) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+6);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+6);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+6);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+6);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+6);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+6);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+6);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+6);
    } else if (step > 10 && step < 100) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+7);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+7);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+7);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+7);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+7);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+7);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+7);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+7);
    } else if (step == 100) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+7);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+7);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+7);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+7);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+7);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+7);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+7);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+7);
    } else if (step > 100 && step < 1000) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+8);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+8);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+8);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+8);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+8);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+8);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+8);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+8);
    } else if (step == 1000) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+8);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+8);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+8);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+8);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+8);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+8);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+8);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+8);
    } else if (step > 1000 && step < 10000) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+9);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+9);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+9);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+9);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+9);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+9);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+9);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+9);
    } else if (step == 10000) {
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+9);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+9);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+9);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+9);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+9);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+9);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+9);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+9);
    } else {
      std::cout << "Config_Settings::step_path: Error, currently do not support nz values greater than 10000!" << std::endl;
      path_A_R = path_A_R.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_R.substr(found+1+9);
      path_A_I = path_A_I.substr(0, found+1+3) + "_" + pending_string + "_" + path_A_I.substr(found+1+9);
      path_w = path_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_w.substr(found+1+9);
      path_e = path_e.substr(0, found+1+3) + "_" + pending_string + "_" + path_e.substr(found+1+9);
      path_hhg_r = path_hhg_r.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_r.substr(found+1+9);
      path_hhg_i = path_hhg_i.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_i.substr(found+1+9);
      path_hhg_w = path_hhg_w.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_w.substr(found+1+9);
      path_hhg_E = path_hhg_E.substr(0, found+1+3) + "_" + pending_string + "_" + path_hhg_E.substr(found+1+9);
    }
  }


  //std::cout << path_A_w_R().substr(3, path_A_w_R().size()) << std::endl;
  //std::cout << path_A_R << std::endl;
  //if (step > 1 && step << 10) path_A_w_R_set(path_A_w_R().substr(3, path_A_w_R().size()));
  //if (step >= 10) path_A_w_R_set(path_A_w_R().substr(3, path_A_w_R().size()));
  //std::string path = set_path(path_A_w_R(), pending_string, "prepend");
  //std::cout << path << std::endl;
  path_A_w_R_set(path_A_R);

  //std::string path = set_path(path_A_w_I(), pending_string, "prepend");
  path_A_w_I_set(path_A_I);

  //path = set_path(path_w_active(), pending_string, "prepend");
  path_w_active_set(path_w);

  path_electron_density_set(path_e);

  path_HHG_R_set(path_hhg_r);
  path_HHG_I_set(path_hhg_i);
  path_HHG_w_set(path_hhg_w);
  path_HHG_E_set(path_hhg_E);
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
    std::cout << "   n_z:              " << n_z() << "                           " << n_z_description() << std::endl;
    std::cout << "   n_r:              " << n_r() << "                           " << n_r_description() << std::endl;
    std::cout << "   n_m:              " << n_m() << "                           " << n_m_description() << std::endl;
    std::cout << "   n_t:              " << n_t() << "                         " << n_t_description() << std::endl;
    std::cout << "   T:                " << T() << "                        " << T_description() << std::endl;
    std::cout << "   w_active_min:     " << w_active_min() << "                        " << w_active_min_description() << std::endl;
    std::cout << "   w_active_max:     " << w_active_max() << "                        " << w_active_max_description() << std::endl;
    std::cout << "   Z:                " << Z() << "                        " << Z_description() << std::endl;
    std::cout << "   R:                " << R() << "                      " << R_description() << std::endl;
    std::cout << "   press:            " << press() << "                          " << press_description() << std::endl;
    std::cout << "   p_av:             " << p_av() << "                            " << p_av_description() << std::endl;
    std::cout << "   rep:              " << rep() << "                         " << rep_description() << std::endl;
    std::cout << "   fwhm:             " << fwhm() << "                        " << fwhm_description() << std::endl;
    std::cout << "   l_0:              " << l_0() << "                        " << l_0_description() << std::endl;
    std::cout << "   ceo:              " << ceo() << "                            " << ceo_description() << std::endl;
    std::cout << "   waist:            " << waist() << "                      " << waist_description() << std::endl;
    std::cout << "   pend_path         " << pend_path() << "                      " << pend_path_description() << std::endl;
    std::cout << "   path_input_j0:    " << path_input_j0() << "        " << path_input_j0_description() << std::endl;
    std::cout << "   path_A_w_R:       " << path_A_w_R() << "      " << path_A_w_R_description() << std::endl;
    std::cout << "   path_A_w_I:       " << path_A_w_I() << "      " << path_A_w_I_description() << std::endl;
    std::cout << "   path_w_active:    " << path_w_active() << "   " << path_w_active_description() << std::endl;
    std::cout << "   path_HHG_R:         " << path_HHG_R() << "   " << path_HHG_R_description() << std::endl;
    std::cout << "   path_config_file: " << path_config_file() << "                 " << path_config_file_description() << std::endl;
    std::cout << "   path_config_log:  " << path_config_log() << " " << path_config_log_description() << std::endl;
    std::cout << "-------------------------------------------------------------------------------\n";
}

void Config_Settings::print(std::string path_) {
    std::ofstream config_log;
    config_log.open(path_);
    if (config_log.is_open()) {
      config_log << "{n_z} {" << n_z() << "} {" << n_z_description() << "}\n";
      config_log << "{n_r} {" << n_r() << "} {" << n_r_description() << "}\n";
      config_log << "{n_m} {" << n_m() << "} {" << n_m_description() << "}\n";

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

      config_log << "{pend_path} {" << pend_path() << "} {" << pend_path_description() << "}\n";

      config_log << "{path_input_j0} {" << path_input_j0() << "} {" << path_input_j0_description() << "}\n";

      config_log << "{path_A_w_R} {" << path_A_w_R() << "} {" << path_A_w_R_description() << "}\n";
      config_log << "{path_A_w_I} {" << path_A_w_I() << "} {" << path_A_w_I_description() << "}\n";
      config_log << "{path_w_active} {" << path_w_active() << "} {" << path_w_active_description() << "}\n";
      config_log << "{path_HHG_R} {" << path_HHG_R() << "} {" << path_HHG_R_description() << "}\n";

      config_log << "{path_config_file} {" << path_config_file() << "} {" << path_config_file_description() << "}\n";
      config_log << "{path_config_log} {" << path_config_log() << "} {" << path_config_log_description() << "}\n";

      config_log.close();

      std::cout << "Config_Settings::print: Successfully wrote config log to " << path_ << std::endl;

    } else {
      std::cout << "Config_Settings::print: Error, could not print to config log at " << path_ << std::endl;
    }
}

int Config_Settings::n_z() {
    return n_z_;
}
void Config_Settings::n_z_set(int value) {
    n_z_ = value;
}
std::string Config_Settings::n_z_description() {
  return n_z_description_;
}
void Config_Settings::n_z_description_set(std::string description) {
  n_z_description_ = description;
}

int Config_Settings::n_r() {
    return n_r_;
}
void Config_Settings::n_r_set(int value) {
    n_r_ = value;
}
std::string Config_Settings::n_r_description() {
  return n_r_description_;
}
void Config_Settings::n_r_description_set(std::string description) {
  n_r_description_ = description;
}

int Config_Settings::n_m() {
    return n_m_;
}
void Config_Settings::n_m_set(int value) {
    n_m_ = value;
}
std::string Config_Settings::n_m_description() {
  return n_m_description_;
}
void Config_Settings::n_m_description_set(std::string description) {
  n_m_description_ = description;
}

int Config_Settings::n_t() {
    return n_t_;
}
void Config_Settings::n_t_set(int value) {
    n_t_ = value;
}
std::string Config_Settings::n_t_description() {
  return n_t_description_;
}
void Config_Settings::n_t_description_set(std::string description) {
  n_t_description_ = description;
}

double Config_Settings::T() {
    return T_;
}
void Config_Settings::T_set(double value) {
    T_ = value;
}
std::string Config_Settings::T_description() {
  return T_description_;
}
void Config_Settings::T_description_set(std::string description) {
  T_description_ = description;
}

double Config_Settings::w_active_min() {
    return w_active_min_;
}
void Config_Settings::w_active_min_set(double value) {
    w_active_min_ = value;
}
std::string Config_Settings::w_active_min_description() {
  return w_active_min_description_;
}
void Config_Settings::w_active_min_description_set(std::string description) {
  w_active_min_description_ = description;
}

double Config_Settings::w_active_max() {
    return w_active_max_;
}
void Config_Settings::w_active_max_set(double value) {
    w_active_max_ = value;
}
std::string Config_Settings::w_active_max_description() {
  return w_active_max_description_;
}
void Config_Settings::w_active_max_description_set(std::string description) {
  w_active_max_description_ = description;
}

double Config_Settings::Z() {
    return Z_;
}
void Config_Settings::Z_set(double value) {
    Z_ = value;
}
std::string Config_Settings::Z_description() {
  return Z_description_;
}
void Config_Settings::Z_description_set(std::string description) {
  Z_description_ = description;
}

double Config_Settings::R() {
    return R_;
}
void Config_Settings::R_set(double value) {
    R_ = value;
}
std::string Config_Settings::R_description() {
  return R_description_;
}
void Config_Settings::R_description_set(std::string description) {
  R_description_ = description;
}

double Config_Settings::press() {
    return press_;
}
void Config_Settings::press_set(double value) {
    press_ = value;
}
std::string Config_Settings::press_description() {
  return press_description_;
}
void Config_Settings::press_description_set(std::string description) {
  press_description_ = description;
}

double Config_Settings::p_av() {
    return p_av_;
}
void Config_Settings::p_av_set(double value) {
    p_av_ = value;
}
std::string Config_Settings::p_av_description() {
  return p_av_description_;
}
void Config_Settings::p_av_description_set(std::string description) {
  p_av_description_ = description;
}

double Config_Settings::rep() {
    return rep_;
}
void Config_Settings::rep_set(double value) {
    rep_ = value;
}
std::string Config_Settings::rep_description() {
  return rep_description_;
}
void Config_Settings::rep_description_set(std::string description) {
  rep_description_ = description;
}

double Config_Settings::fwhm() {
    return fwhm_;
}
void Config_Settings::fwhm_set(double value) {
    fwhm_ = value;
}
std::string Config_Settings::fwhm_description() {
  return fwhm_description_;
}
void Config_Settings::fwhm_description_set(std::string description) {
  fwhm_description_ = description;
}

double Config_Settings::l_0() {
    return l_0_;
}
void Config_Settings::l_0_set(double value) {
    l_0_ = value;
}
std::string Config_Settings::l_0_description() {
  return l_0_description_;
}
void Config_Settings::l_0_description_set(std::string description) {
  l_0_description_ = description;
}

double Config_Settings::ceo() {
    return ceo_;
}
void Config_Settings::ceo_set(double value) {
    ceo_ = value;
}
std::string Config_Settings::ceo_description() {
  return ceo_description_;
}
void Config_Settings::ceo_description_set(std::string description) {
  ceo_description_ = description;
}

double Config_Settings::waist() {
    return waist_;
}
void Config_Settings::waist_set(double value) {
    waist_ = value;
}
std::string Config_Settings::waist_description() {
  return waist_description_;
}
void Config_Settings::waist_description_set(std::string description) {
  waist_description_ = description;
}

std::string Config_Settings::pend_path() {
    return pend_path_;
}
void Config_Settings::pend_path_set(std::string value) {
    pend_path_ = value;
}
std::string Config_Settings::pend_path_description() {
  return pend_path_description_;
}
void Config_Settings::pend_path_description_set(std::string description) {
  pend_path_description_ = description;
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

std::string Config_Settings::path_A_w_R() {
    return path_A_w_R_;
}
void Config_Settings::path_A_w_R_set(std::string value) {
    path_A_w_R_ = value;
}
std::string Config_Settings::path_A_w_R_description() {
  return path_A_w_R_description_;
}
void Config_Settings::path_A_w_R_description_set(std::string description) {
  path_A_w_R_description_ = description;
}

std::string Config_Settings::path_A_w_I() {
    return path_A_w_I_;
}
void Config_Settings::path_A_w_I_set(std::string value) {
    path_A_w_I_ = value;
}
std::string Config_Settings::path_A_w_I_description() {
  return path_A_w_I_description_;
}
void Config_Settings::path_A_w_I_description_set(std::string description) {
  path_A_w_I_description_ = description;
}

std::string Config_Settings::path_w_active() {
    return path_w_active_;
}
void Config_Settings::path_w_active_set(std::string value) {
    path_w_active_ = value;
}
std::string Config_Settings::path_w_active_description() {
  return path_w_active_description_;
}
void Config_Settings::path_w_active_description_set(std::string description) {
  path_w_active_description_ = description;
}

std::string Config_Settings::path_electron_density() {
    return path_electron_density_;
}
void Config_Settings::path_electron_density_set(std::string value) {
    path_electron_density_ = value;
}
std::string Config_Settings::path_electron_density_description() {
  return path_electron_density_description_;
}
void Config_Settings::path_electron_density_description_set(std::string description) {
  path_electron_density_description_ = description;
}

std::string Config_Settings::path_HHG_R() {
    return path_HHG_R_;
}
void Config_Settings::path_HHG_R_set(std::string value) {
    path_HHG_R_ = value;
}
std::string Config_Settings::path_HHG_R_description() {
  return path_HHG_R_description_;
}
void Config_Settings::path_HHG_R_description_set(std::string description) {
  path_HHG_R_description_ = description;
}

std::string Config_Settings::path_HHG_I() {
    return path_HHG_I_;
}
void Config_Settings::path_HHG_I_set(std::string value) {
    path_HHG_I_ = value;
}
std::string Config_Settings::path_HHG_I_description() {
  return path_HHG_I_description_;
}
void Config_Settings::path_HHG_I_description_set(std::string description) {
  path_HHG_I_description_ = description;
}

std::string Config_Settings::path_HHG_w() {
    return path_HHG_w_;
}
void Config_Settings::path_HHG_w_set(std::string value) {
    path_HHG_w_ = value;
}
std::string Config_Settings::path_HHG_w_description() {
  return path_HHG_w_description_;
}
void Config_Settings::path_HHG_w_description_set(std::string description) {
  path_HHG_w_description_ = description;
}

std::string Config_Settings::path_HHG_E() {
    return path_HHG_E_;
}
void Config_Settings::path_HHG_E_set(std::string value) {
    path_HHG_E_ = value;
}
std::string Config_Settings::path_HHG_E_description() {
  return path_HHG_E_description_;
}
void Config_Settings::path_HHG_E_description_set(std::string description) {
  path_HHG_E_description_ = description;
}

std::string Config_Settings::path_config_file() {
    return path_config_file_;
}
void Config_Settings::path_config_file_set(std::string value) {
    path_config_file_ = value;
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
void Config_Settings::path_config_log_set(std::string value) {
    path_config_log_ = value;
}
std::string Config_Settings::path_config_log_description() {
  return path_config_log_description_;
}
void Config_Settings::path_config_log_description_set(std::string description) {
  path_config_log_description_ = description;
}
