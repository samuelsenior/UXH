//
//  config_settings.hpp
//
//  Modified by Samuel Senior on 05/02/2017.
//  Reads in input parameters and settings from a config file.
//

#ifndef __CONFIG_SETTINGS_HPP_INCLUDED__
#define __CONFIG_SETTINGS_HPP_INCLUDED__

#include <string>

//------------------------------------------------------------------------------------------------//
//  Class definition
//------------------------------------------------------------------------------------------------//

/*!
Modified by Samuel Senior on 05/02/2017.
Reads in input parameters and settings from a config file.
*/
class Config_Settings {

private:

  enum class SN{
    n_z = 0, n_r, n_m,
    n_t, T, w_active_min, w_active_max,
    Z, R,
    press,
    p_av, rep, fwhm, l_0, ceo, waist,
    pend_path,
    path_input_j0,
    path_A_w_R, path_A_w_I, path_w_active,
    path_HHG_R, path_HHG_I, path_HHG_w, path_HHG_E,
    path_config_file, path_config_log,
    LAST_SN_ENTRY
  };

    // Input parameters and settings (the values are the default values)
    int n_z_ = 50;
    int n_r_ = 20;
    int n_m_ = 20;

    int n_t_ = 4096;
    double T_ = 500.0e-15;
    double w_active_min_ = 2.0e14;
    double w_active_max_ = 8.0e15;

    double Z_ = 5.0e-3;
    double R_ = 75.0e-6;

    double press_ = 100.0e-3;

    double p_av_ = 1.0;
    double rep_ = 1.0e3;
    double fwhm_ = 40e-15;
    double l_0_ = 800e-9;
    double ceo_ = 0.0;
    double waist_ = 48.0e-6;

    std::string path_input_j0_ = "../input/J0_zeros.bin";

    std::string path_A_w_R_ = "../output/A_w_R.bin";
    std::string path_A_w_I_ = "../output/A_w_I.bin";
    std::string path_w_active_ = "../output/w_active.bin";
    std::string path_electron_density_ = "../output/electron_density.bin";

    std::string path_HHG_R_ = "../output/HHG_R.bin";
    std::string path_HHG_I_ = "../output/HHG_I.bin";
    std::string path_HHG_w_ = "../output/HHG_w.bin";
    std::string path_HHG_E_ = "../output/HHG_E.bin";

    std::string path_config_file_ = "../configFiles/config.txt";
    std::string path_config_log_ = "../output/config_log.txt";

    std::string pend_path_ = "prepend";

    // Input parameters and settings descriptions (the default value is just '(default value)')
    std::string n_z_description_ = "(default) (int) Number of steps in Z";
    std::string n_r_description_ = "(default) (int) The z_r value";
    std::string n_m_description_ = "(default) (int) Number of modes";

    std::string n_t_description_ = "(default) (int) The z_t value";
    std::string T_description_ = "(default) (double) The T value";
    std::string w_active_min_description_ = "(default) (double) Minimum angular frequency";
    std::string w_active_max_description_ = "(default) (double) Maximum angular frequency";

    std::string Z_description_ = "(default) (double) Length of capillary";
    std::string R_description_ = "(default) (double) Radius of capillary";

    std::string press_description_ = "(default) (double) Pressure of the gas";

    std::string p_av_description_ = "(default) (double) The p_av value";
    std::string rep_description_ = "(default) (double) The rep value";
    std::string fwhm_description_ = "(default) (double) Full width at half max";
    std::string l_0_description_ = "(default) (double) Laser central wavelength";
    std::string ceo_description_ = "(default) (double) The ceo value";
    std::string waist_description_= "(default) (double) The waist value";

    std::string pend_path_description_ = "(default) (std::string) Pending switch";

    std::string path_input_j0_description_ = "(default) (std::string) Path to J0_zeros.bin";

    std::string path_A_w_R_description_ = "(default) (std::string) Path of A_w_R";
    std::string path_A_w_I_description_ = "(default) (std::string) Path of A_w_I";
    std::string path_w_active_description_ = "(default) (std::string) Path of w_active";
    std::string path_electron_density_description_ = "(default) (std::string) Path of electron_density";

    std::string path_HHG_R_description_ = "(default) (std::string) Path of HHG_R";
    std::string path_HHG_I_description_ = "(default) (std::string) Path of HHG_I";
    std::string path_HHG_w_description_ = "(default) (std::string) Path of HHG w";
    std::string path_HHG_E_description_ = "(default) (std::string) Path of HHG E";

    std::string path_config_file_description_ = "(default) (std::string) config.txt path";
    std::string path_config_log_description_ = "(default) (std::string) config_log.txt path";

    static const char * setting_name[];

    // Private Functions
    void set_variable(std::string&, std::string&, std::string&, bool print_to_screen=true);
    std::string set_path(std::string, std::string, std::string pend="");
    std::string set_pre_path(std::string, std::string);
    std::string set_post_path(std::string, std::string);

public:
    // Functions
    Config_Settings();
    void read_in(std::string, bool print_to_screen = true);
    void check_paths(bool print_to_screen = true);
    void step_path(int step);
    void print();
    void print(std::string);

    int n_z();
    void n_z_set(int);
    std::string n_z_description();
    void n_z_description_set(std::string);
    int n_r();
    void n_r_set(int);
    std::string n_r_description();
    void n_r_description_set(std::string);
    int n_m();
    void n_m_set(int);
    std::string n_m_description();
    void n_m_description_set(std::string);

    int n_t();
    void n_t_set(int);
    std::string n_t_description();
    void n_t_description_set(std::string);
    double T();
    void T_set(double);
    std::string T_description();
    void T_description_set(std::string);
    double w_active_min();
    void w_active_min_set(double);
    std::string w_active_min_description();
    void w_active_min_description_set(std::string);
    double w_active_max();
    void w_active_max_set(double);
    std::string w_active_max_description();
    void w_active_max_description_set(std::string);

    double Z();
    void Z_set(double);
    std::string Z_description();
    void Z_description_set(std::string);
    double R();
    void R_set(double);
    std::string R_description();
    void R_description_set(std::string);

    double press();
    void press_set(double);
    std::string press_description();
    void press_description_set(std::string);

    double p_av();
    void p_av_set(double);
    std::string p_av_description();
    void p_av_description_set(std::string);
    double rep();
    void rep_set(double);
    std::string rep_description();
    void rep_description_set(std::string);
    double fwhm();
    void fwhm_set(double);
    std::string fwhm_description();
    void fwhm_description_set(std::string);
    double l_0();
    void l_0_set(double);
    std::string l_0_description();
    void l_0_description_set(std::string);
    double ceo();
    void ceo_set(double);
    std::string ceo_description();
    void ceo_description_set(std::string);
    double waist();
    void waist_set(double);
    std::string waist_description();
    void waist_description_set(std::string);

    std::string pend_path();
    void pend_path_set(std::string);
    std::string pend_path_description();
    void pend_path_description_set(std::string);

    std::string path_input_j0();
    void path_input_j0_set(std::string);
    std::string path_input_j0_description();
    void path_input_j0_description_set(std::string);

    std::string path_A_w_R();
    void path_A_w_R_set(std::string);
    std::string path_A_w_R_description();
    void path_A_w_R_description_set(std::string);
    std::string path_A_w_I();
    void path_A_w_I_set(std::string);
    std::string path_A_w_I_description();
    void path_A_w_I_description_set(std::string);
    std::string path_w_active();
    void path_w_active_set(std::string);
    std::string path_w_active_description();
    void path_w_active_description_set(std::string);

    std::string path_electron_density();
    void path_electron_density_set(std::string);
    std::string path_electron_density_description();
    void path_electron_density_description_set(std::string);

    std::string path_HHG_R();
    void path_HHG_R_set(std::string);
    std::string path_HHG_R_description();
    void path_HHG_R_description_set(std::string);

    std::string path_HHG_I();
    void path_HHG_I_set(std::string);
    std::string path_HHG_I_description();
    void path_HHG_I_description_set(std::string);

    std::string path_HHG_w();
    void path_HHG_w_set(std::string);
    std::string path_HHG_w_description();
    void path_HHG_w_description_set(std::string);

    std::string path_HHG_E();
    void path_HHG_E_set(std::string);
    std::string path_HHG_E_description();
    void path_HHG_E_description_set(std::string);

    std::string path_config_file();
    void path_config_file_set(std::string);
    std::string path_config_file_description();
    void path_config_file_description_set(std::string);
    std::string path_config_log();
    void path_config_log_set(std::string);
    std::string path_config_log_description();
    void path_config_log_description_set(std::string);

};

#endif
