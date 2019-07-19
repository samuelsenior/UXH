//
//  config_settings.hpp
//
//  Modified by Samuel Senior on 05/02/2017.
//  Reads in input parameters and settings from a config file.
//

#ifndef __CONFIG_SETTINGS_HPP_INCLUDED__
#define __CONFIG_SETTINGS_HPP_INCLUDED__

#include <string>

//namespace UPPE {

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
    output_sampling_rate, 
    n_t, T, w_active_min, w_active_max,
    Z, R,
    press,
    p_av, rep, fwhm, l_0, ceo, waist,
    laser_rel_tol,
    read_in_laser_pulse, path_A_w_R_initial, path_A_w_I_initial, path_w_active_initial,
    original_n_z,
    HHGP_starting_z,
    ending_n_z,
    interp_points,
    gas_pressure_profile,
    HHG_lambda_min,
    HHG_lambda_max,
    HHP_E_min, 
    HHP_E_max,
    output_electron_density,
    pend_path,
    path_input_j0,
    path_A_w_R, path_A_w_I, path_w_active,
    path_electron_density,
    path_HHG_R, path_HHG_I, path_HHG_w, path_HHG_E,
    path_HHP_R, path_HHP_I, path_HHP_w,
    path_config_file, path_config_log,
    LAST_SN_ENTRY
  };

    // Input parameters and settings (the values are the default values)
    int n_z_ = 50;
    int n_r_ = 20;
    int n_m_ = 20;

    int output_sampling_rate_ = 1;

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
    double l_0_ = 800.0e-9;
    double ceo_ = 0.0;
    double waist_ = 48.0e-6;

    double laser_rel_tol_ = 1e-6;

    int read_in_laser_pulse_ = 0;
    std::string path_A_w_R_initial_ = "../output/A_w_R.bin";
    std::string path_A_w_I_initial_ = "../output/A_w_I.bin";
    std::string path_w_active_initial_ = "../output/w_active.bin";
    int original_n_z_ = n_z_;

    double HHGP_starting_z_ = 0.0;

    int ending_n_z_ = n_z_;

    int interp_points_ = 0;

    std::string gas_pressure_profile_ = "capillary";

    double HHG_lambda_min_ = 6.0e-9;
    double HHG_lambda_max_ = 130.0e-9;
    double HHP_E_min_ = 10.0;
    double HHP_E_max_ = 206.0;

    int output_electron_density_ = 0;

    std::string path_input_j0_ = "../../input/J0_zeros.bin";

    std::string path_A_w_R_ = "../output/A_w_R.bin";
    std::string path_A_w_I_ = "../output/A_w_I.bin";
    std::string path_w_active_ = "../output/w_active.bin";
    std::string path_electron_density_ = "../output/electron_density.bin";

    std::string path_A_w_R_step_ = "../output/A_w_R.bin";
    std::string path_A_w_I_step_ = "../output/A_w_I.bin";
    std::string path_w_active_step_ = "../output/w_active.bin";
    std::string path_electron_density_step_ = "../output/electron_density.bin";

    std::string path_HHG_R_ = "../output/HHG_R.bin";
    std::string path_HHG_I_ = "../output/HHG_I.bin";
    std::string path_HHG_w_ = "../output/HHG_w.bin";
    std::string path_HHG_E_ = "../output/HHG_E.bin";

    std::string path_HHP_R_ = "../output/HHP_R.bin";
    std::string path_HHP_I_ = "../output/HHP_I.bin";
    std::string path_HHP_w_ = "../output/HHP_w.bin";

    std::string path_HHG_R_step_ = "../output/HHG_R.bin";
    std::string path_HHG_I_step_ = "../output/HHG_I.bin";
    std::string path_HHG_w_step_ = "../output/HHG_w.bin";
    std::string path_HHG_E_step_ = "../output/HHG_E.bin";

    std::string path_HHP_R_step_ = "../output/HHP_R.bin";
    std::string path_HHP_I_step_ = "../output/HHP_I.bin";
    std::string path_HHP_w_step_ = "../output/HHP_w.bin";

    std::string path_config_file_ = "../configFiles/config_UPPE.txt";
    std::string path_config_log_ = "../output/config_log_UPPE.txt";

    std::string pend_path_ = "prepend";

    // Input parameters and settings descriptions (the default value is just '(default value)')
    std::string n_z_description_ = "(default) (int) Number of steps in Z";
    std::string n_r_description_ = "(default) (int) The z_r value";
    std::string n_m_description_ = "(default) (int) Number of modes";

    std::string output_sampling_rate_description_ = "(default) (int) Output sampling rate in Z";

    std::string n_t_description_ = "(default) (int) The n_t value";
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

    std::string laser_rel_tol_description_ = "(default) (double) Laser propagation relative tolerance/error";

    std::string read_in_laser_pulse_description_= "(default) (int) Switch to read in an initial laser pulse";
    std::string path_A_w_R_initial_description_ = "(default) (std::string) Initial path of A_w_R";
    std::string path_A_w_I_initial_description_ = "(default) (std::string) Initial path of A_w_I";
    std::string path_w_active_initial_description_ = "(default) (std::string) Initial path of w_active";
    std::string original_n_z_description_ = "(default) (int) Original n_z from read in file";

    std::string HHGP_starting_z_description_ = "(default) (double) Starting distance for HHGP in capillary";

    std::string ending_n_z_description_ = "(default) (int) Ending step for UPPE block";

    std::string interp_points_description_ = "(default) (int) Number of interpolation points between each step";

    std::string gas_pressure_profile_description_ = "(default) (string) Switch for gas pressure profile";

    std::string HHG_lambda_min_description_ = "(default) (double) Minimum generated high harmonic wavelength outputted";
    std::string HHG_lambda_max_description_ = "(default) (double) Maximum generated high harmonic wavelength outputted";
    std::string HHP_E_min_description_ = "(default) (double) Minimum propagated high harmonic energy outputted";
    std::string HHP_E_max_description_ = "(default) (double) Maximum propagated high harmonic energy outputted";

    std::string output_electron_density_description_ = "(default) (int) Switch for outputting electron density";

    std::string pend_path_description_ = "(default) (std::string) Pending switch";

    std::string path_input_j0_description_ = "(default) (std::string) Path to J0_zeros.bin";

    std::string path_A_w_R_description_ = "(default) (std::string) Path of A_w_R";
    std::string path_A_w_I_description_ = "(default) (std::string) Path of A_w_I";
    std::string path_w_active_description_ = "(default) (std::string) Path of w_active";
    std::string path_electron_density_description_ = "(default) (std::string) Path of electron_density";

    std::string path_A_w_R_description_step_ = "(default) (std::string) Path of A_w_R step";
    std::string path_A_w_I_description_step_ = "(default) (std::string) Path of A_w_I step";
    std::string path_w_active_description_step_ = "(default) (std::string) Path of w_active step";
    std::string path_electron_density_description_step_ = "(default) (std::string) Path of electron_density step";

    std::string path_HHG_R_description_ = "(default) (std::string) Path of HHG_R";
    std::string path_HHG_I_description_ = "(default) (std::string) Path of HHG_I";
    std::string path_HHG_w_description_ = "(default) (std::string) Path of HHG w";
    std::string path_HHG_E_description_ = "(default) (std::string) Path of HHG E";

    std::string path_HHP_R_description_ = "(default) (std::string) Path of HHP_R";
    std::string path_HHP_I_description_ = "(default) (std::string) Path of HHP_I";
    std::string path_HHP_w_description_ = "(default) (std::string) Path of HHP w";

    std::string path_HHG_R_description_step_ = "(default) (std::string) Path of HHG_R step";
    std::string path_HHG_I_description_step_ = "(default) (std::string) Path of HHG_I step";
    std::string path_HHG_w_description_step_ = "(default) (std::string) Path of HHG w step";
    std::string path_HHG_E_description_step_ = "(default) (std::string) Path of HHG E step";

    std::string path_HHP_R_description_step_ = "(default) (std::string) Path of HHP_R step";
    std::string path_HHP_I_description_step_ = "(default) (std::string) Path of HHP_I step";
    std::string path_HHP_w_description_step_ = "(default) (std::string) Path of HHP w step";

    std::string path_config_file_description_ = "(default) (std::string) config.txt path";
    std::string path_config_log_description_ = "(default) (std::string) config_log.txt path";

    static const char * setting_name[];

    // Private variables for the step path workings
    std::string pending_string_stepWorkings;// = std::to_string(static_cast<unsigned long long>(step));
    int pending_string_len_stepWorkings;// = std::to_string(static_cast<unsigned long long>(step - 1)).length();
    std::string path_A_R_stepWorkings;// = path_A_w_R();
    std::string path_A_I_stepWorkings;// = path_A_w_I();
    std::string path_w_stepWorkings;// = path_w_active();
    std::string path_e_stepWorkings;// = path_electron_density();
    std::string path_hhg_r_stepWorkings;// = path_HHG_R();
    std::string path_hhg_i_stepWorkings;// = path_HHG_I();
    std::string path_hhg_w_stepWorkings;// = path_HHG_w();
    std::string path_hhg_E_stepWorkings;// = path_HHG_E();
    std::string path_hhp_r_stepWorkings;
    std::string path_hhp_i_stepWorkings;
    std::string path_hhp_w_stepWorkings;
    std::size_t found_stepWorkings;// = path_A_R.find_last_of("/");
    std::string tmp_stepWorkings;// = path_A_R.substr(found+1);
    size_t count_underscore_stepWorkings;// = std::count(tmp.begin(), tmp.end(), '_');

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
    void step_path(int step, std::string variable);
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

    int output_sampling_rate();
    void output_sampling_rate_set(int);
    std::string output_sampling_rate_description();
    void output_sampling_rate_description_set(std::string);

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

    double laser_rel_tol();
    void laser_rel_tol_set(double);
    std::string laser_rel_tol_description();
    void laser_rel_tol_description_set(std::string);

    int read_in_laser_pulse();
    void read_in_laser_pulse_set(int);
    std::string read_in_laser_pulse_description();
    void read_in_laser_pulse_description_set(std::string);
    std::string path_A_w_R_initial();
    void path_A_w_R_initial_set(std::string);
    std::string path_A_w_R_initial_description();
    void path_A_w_R_initial_description_set(std::string);
    std::string path_A_w_I_initial();
    void path_A_w_I_initial_set(std::string);
    std::string path_A_w_I_initial_description();
    void path_A_w_I_initial_description_set(std::string);
    std::string path_w_active_initial();
    void path_w_active_initial_set(std::string);
    std::string path_w_active_initial_description();
    void path_w_active_initial_description_set(std::string);
    int original_n_z();
    void original_n_z_set(int);
    std::string original_n_z_description();
    void original_n_z_description_set(std::string);

    double HHGP_starting_z();
    void HHGP_starting_z_set(double);
    std::string HHGP_starting_z_description();
    void HHGP_starting_z_description_set(std::string);

    int ending_n_z();
    void ending_n_z_set(int);
    std::string ending_n_z_description();
    void ending_n_z_description_set(std::string);

    int interp_points();
    void interp_points_set(int);
    std::string interp_points_description();
    void interp_points_description_set(std::string);

    std::string gas_pressure_profile();
    void gas_pressure_profile_set(std::string);
    std::string gas_pressure_profile_description();
    void gas_pressure_profile_description_set(std::string);


    double HHG_lambda_min();
    void HHG_lambda_min_set(double);
    std::string HHG_lambda_min_description();
    void HHG_lambda_min_description_set(std::string);

    double HHG_lambda_max();
    void HHG_lambda_max_set(double);
    std::string HHG_lambda_max_description();
    void HHG_lambda_max_description_set(std::string);

    double HHP_E_min();
    void HHP_E_min_set(double);
    std::string HHP_E_min_description();
    void HHP_E_min_description_set(std::string);

    double HHP_E_max();
    void HHP_E_max_set(double);
    std::string HHP_E_max_description();
    void HHP_E_max_description_set(std::string);


    int output_electron_density();
    void output_electron_density_set(int);
    std::string output_electron_density_description();
    void output_electron_density_description_set(std::string);

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

    std::string path_A_w_R_step();
    void path_A_w_R_step_set(std::string);
    std::string path_A_w_R_description_step();
    void path_A_w_R_description_step_set(std::string);
    std::string path_A_w_I_step();
    void path_A_w_I_step_set(std::string);
    std::string path_A_w_I_description_step();
    void path_A_w_I_description_step_set(std::string);
    std::string path_w_active_step();
    void path_w_active_step_set(std::string);
    std::string path_w_active_description_step();
    void path_w_active_description_step_set(std::string);
    std::string path_electron_density_step();
    void path_electron_density_step_set(std::string);
    std::string path_electron_density_description_step();
    void path_electron_density_description_step_set(std::string);

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

    std::string path_HHP_R();
    void path_HHP_R_set(std::string);
    std::string path_HHP_R_description();
    void path_HHP_R_description_set(std::string);
    std::string path_HHP_I();
    void path_HHP_I_set(std::string);
    std::string path_HHP_I_description();
    void path_HHP_I_description_set(std::string);
    std::string path_HHP_w();
    void path_HHP_w_set(std::string);
    std::string path_HHP_w_description();
    void path_HHP_w_description_set(std::string);

    std::string path_HHG_R_step();
    void path_HHG_R_step_set(std::string);
    std::string path_HHG_R_description_step();
    void path_HHG_R_description_step_set(std::string);
    std::string path_HHG_I_step();
    void path_HHG_I_step_set(std::string);
    std::string path_HHG_I_description_step();
    void path_HHG_I_description_step_set(std::string);
    std::string path_HHG_w_step();
    void path_HHG_w_step_set(std::string);
    std::string path_HHG_w_description_step();
    void path_HHG_w_description_step_set(std::string);
    std::string path_HHG_E_step();
    void path_HHG_E_step_set(std::string);
    std::string path_HHG_E_description_step();
    void path_HHG_E_description_step_set(std::string);

    std::string path_HHP_R_step();
    void path_HHP_R_step_set(std::string);
    std::string path_HHP_R_description_step();
    void path_HHP_R_description_step_set(std::string);
    std::string path_HHP_I_step();
    void path_HHP_I_step_set(std::string);
    std::string path_HHP_I_description_step();
    void path_HHP_I_description_step_set(std::string);
    std::string path_HHP_w_step();
    void path_HHP_w_step_set(std::string);
    std::string path_HHP_w_description_step();
    void path_HHP_w_description_step_set(std::string);

    std::string path_config_file();
    void path_config_file_set(std::string);
    std::string path_config_file_description();
    void path_config_file_description_set(std::string);
    std::string path_config_log();
    void path_config_log_set(std::string);
    std::string path_config_log_description();
    void path_config_log_description_set(std::string);

};

//} // UPPE namespace

#endif
