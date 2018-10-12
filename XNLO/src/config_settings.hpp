//
//  config_settings.hpp
//
//  Modified by Samuel Senior on 05/02/2017.
//  Reads in input parameters and settings from a config file.
//

#ifndef __CONFIG_SETTINGS_HPP_INCLUDED__
#define __CONFIG_SETTINGS_HPP_INCLUDED__

#include <string>

namespace XNLO{

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
    atoms_per_worker = 0,
    x_min, x_max,
    N_t, t_min, t_max,
    P_av, RR, FWHM, l_0, CEO, spot_radius,
    alpha,
    read_in_laser_pulse,
    output_wavefunction,
    pend_path,
    path_input_j0,
    path_laser_A_w_R,
    path_laser_A_w_I,
    path_laser_w_active,
    path_dipole, path_w,
    path_E,
    path_config_file, path_config_log,
    LAST_SN_ENTRY
  };

    // Input parameters and settings (the values are the default values)
    int atoms_per_worker_ = 2;

    double x_min_ = 0;
    double x_max_ = 100e-6;

    int N_t_ = 262144;//std::pow(2.0, 18);
    double t_min_ = -100e-15;
    double t_max_ = 100e-15;

    double P_av_ = 0.18;
    double RR_ = 1000;
    double FWHM_ = 15e-15;
    double l_0_ = 795e-9;
    double CEO_ = 0;
    double spot_radius_ = 42e-6;

    double alpha_ = 1.45;

    int output_wavefunction_ = 0;
    int read_in_laser_pulse_ = 0;

    std::string pend_path_ = "prepend";

    std::string path_input_j0_ = "../input/J0_zeros.bin";

    std::string path_laser_A_w_R_ = "../../UPPE/output/000_1_A_w_R.bin";
    std::string path_laser_A_w_I_ = "../../UPPE/output/000_1_A_w_I.bin";
    std::string path_laser_w_active_ = "../../UPPE/output/000_1_w_active.bin";

    std::string path_dipole_ = "../output/dipole.bin";
    std::string path_w_ = "../output/w.bin";

    std::string path_E_ = "../output/E.bin";

    std::string path_config_file_ = "./config.txt";
    std::string path_config_log_ = "../output/config_log.txt";

    // Input parameters and settings descriptions (the default value is just '(default value)')
    std::string atoms_per_worker_description_ = "(default) (int) The number of atoms per worker";

    std::string x_min_description_ = "(default) (double) The x_min value}";
    std::string x_max_description_ = "(default) (double) the x_max value";

    std::string N_t_description_ = "(default) (int) The N_t value";
    std::string t_min_description_ = "(default) (double) The t_min value";
    std::string t_max_description_ = "(default) (double) The t_max value";

    std::string P_av_description_ = "(default) (double) The P_av value";
    std::string RR_description_ = "(default) (double) The RR value";
    std::string FWHM_description_ = "(default) (double) The FWHM value";
    std::string l_0_description_ = "(default) (double) The l_0 value";
    std::string CEO_description_ = "(default) (double) The CEO value";
    std::string spot_radius_description_ = "(default) (double) The spot_radius value";

    std::string alpha_description_ = "(default) (double) The alpha value";

    std::string read_in_laser_pulse_description_ = "(default) (int) Switch to read in laser pulse";

    std::string output_wavefunction_description_ = "(default) (int) Switch to output wavefunction";

    std::string pend_path_description_ = "(default) (std::string) Pending switch";

    std::string path_input_j0_description_ = "(default) (std::string) Path to J0_zeros.bin";

    std::string path_laser_A_w_R_description_ = "(default) (std::string) Path to A_w_R of laser pulse input";
    std::string path_laser_A_w_I_description_ = "(default) (std::string) Path to A_w_I of laser pulse input";
    std::string path_laser_w_active_description_ = "(default) (std::string) Path to w_active of laser pulse input";

    std::string path_dipole_description_ = "(default) (std::string) Output path of acceleration";
    std::string path_w_description_ = "(default) (std::string) Output path of w";

    std::string path_E_description_ = "(default) (std::string) Ouput path of electric field";

    std::string path_config_file_description_ = "(default) (std::string) config.txt path";
    std::string path_config_log_description_ = "(default) (std::string) Output path of config_log.txt";

    static const char * setting_name[];

    //Private Functions
    void set_variable(std::string&, std::string&, std::string&);
    std::string set_path(std::string, std::string);
    std::string set_pre_path(std::string, std::string);
    std::string set_post_path(std::string, std::string);

public:
    // Functions
    Config_Settings();
    void read_in(std::string, bool print_to_screen = true);
    void check_paths(bool print_to_screen = true);
    void print();
    void print(std::string);

    int atoms_per_worker();
    void atoms_per_worker_set(int);
    std::string atoms_per_worker_description();
    void atoms_per_worker_description_set(std::string);

    double x_min();
    void x_min_set(double);
    std::string x_min_description();
    void x_min_description_set(std::string);
    double x_max();
    void x_max_set(double);
    std::string x_max_description();
    void x_max_description_set(std::string);

    int N_t();
    void N_t_set(int);
    std::string N_t_description();
    void N_t_description_set(std::string);
    double t_min();
    void t_min_set(double);
    std::string t_min_description();
    void t_min_description_set(std::string);
    double t_max();
    void t_max_set(double);
    std::string t_max_description();
    void t_max_description_set(std::string);

    double P_av();
    void P_av_set(double);
    std::string P_av_description();
    void P_av_description_set(std::string);
    double RR();
    void RR_set(double);
    std::string RR_description();
    void RR_description_set(std::string);
    double FWHM();
    void FWHM_set(double);
    std::string FWHM_description();
    void FWHM_description_set(std::string);
    double l_0();
    void l_0_set(double);
    std::string l_0_description();
    void l_0_description_set(std::string);
    double CEO();
    void CEO_set(double);
    std::string CEO_description();
    void CEO_description_set(std::string);
    double spot_radius();
    void spot_radius_set(double);
    std::string spot_radius_description();
    void spot_radius_description_set(std::string);
    double alpha();
    void alpha_set(double);
    std::string alpha_description();
    void alpha_description_set(std::string);

    int output_wavefunction();
    void output_wavefunction_set(int);
    std::string output_wavefunction_description();
    void output_wavefunction_description_set(std::string);

    int read_in_laser_pulse();
    void read_in_laser_pulse_set(int);
    std::string read_in_laser_pulse_description();
    void read_in_laser_pulse_description_set(std::string);

    std::string pend_path();
    void pend_path_set(std::string);
    std::string pend_path_description();
    void pend_path_description_set(std::string);

    std::string path_input_j0();
    void path_input_j0_set(std::string);
    std::string path_input_j0_description();
    void path_input_j0_description_set(std::string);

    std::string path_laser_A_w_R();
    void path_laser_A_w_R_set(std::string);
    std::string path_laser_A_w_R_description();
    void path_laser_A_w_R_description_set(std::string);
    std::string path_laser_A_w_I();
    void path_laser_A_w_I_set(std::string);
    std::string path_laser_A_w_I_description();
    void path_laser_A_w_I_description_set(std::string);
    std::string path_laser_w_active();
    void path_laser_w_active_set(std::string);
    std::string path_laser_w_active_description();
    void path_laser_w_active_description_set(std::string);

    std::string path_dipole();
    void path_dipole_set(std::string);
    std::string path_dipole_description();
    void path_dipole_description_set(std::string);
    std::string path_w();
    void path_w_set(std::string);
    std::string path_w_description();
    void path_w_description_set(std::string);

    std::string path_E();
    void path_E_set(std::string);
    std::string path_E_description();
    void path_E_description_set(std::string);

    std::string path_config_file();
    void path_config_file_set(std::string);
    std::string path_config_file_description();
    void path_config_file_description_set(std::string);
    std::string path_config_log();
    void path_config_log_set(std::string);
    std::string path_config_log_description();
    void path_config_log_description_set(std::string);

};

}

#endif
