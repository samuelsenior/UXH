# -*- coding: utf-8 -*-
"""
Created on Thu Oct  8 16:33:24 2020

@author: smsen
"""
from collections import OrderedDict

from IO import ConfigIO

class ConfigEntry:
    def __init__(self, name, description, defaultValue, value=None, active=True, inactiveMessage="(inactive)"):
        self.name = name
        self.defaultValue = defaultValue
        self.value = value
        self.description = description
        self.active = active
        self.inactiveMessage = inactiveMessage
        
        #if self.value == None:
        #    self.value_str = ""
        #else:
        #    self.value_str = str(self.value)
        
        if self.value == None:
            if self.active == True:
                self.value = self.defaultValue
                self.value_str= str(self.value)
                self.description = "(default) " + self.description
            else:
                self.description = "(inactive) " + self.description
                self.value_str = self.inactiveMessage

class Config(ConfigIO):
    def __init__(self, configType):
        super().__init__()
        self.configType = configType
        
        self.setUXHBaseConfig()
        
    def setUXHBaseConfig(self):
        
        # ---------------- #
        # -  UPPE Config - #
        # ---------------- #
        self.UPPE = OrderedDict([])
        
        self.UPPE["n_z"] = ConfigEntry("n_z", "(int) Number of steps along the capillary", 300)
        self.UPPE["n_r"] = ConfigEntry("n_r", "(int) Number of radial positions in the capillary", 20)
        self.UPPE["n_m"] = ConfigEntry("n_m", "(int) Number of modes", 20)
        
        self.UPPE["output_sampling_rate"] = ConfigEntry("output_sampling_rate", "(int) Output sampling rate in Z", 1)
        
        self.UPPE["n_t"] = ConfigEntry("n_t", "(int) Number of time steps", 16384)
        self.UPPE["T"] = ConfigEntry("T", "(double) Total time being simulated over / s", 1000.0e-15)
        self.UPPE["w_active_min"] = ConfigEntry("w_active_min", "(double) Minimum positive angular frequency / rad/sec", 2.0e14)
        self.UPPE["w_active_max"] = ConfigEntry("w_active_max", "(double) Maximum positive angular frequency / rad/sec", 8.0e15)
        
        self.UPPE["Z"] = ConfigEntry("Z", "(double) Length of capillary / m", 70.0e-3)
        self.UPPE["R"] = ConfigEntry("R", "(double) Radius of capillary / m", 75.0e-6)
        
        self.UPPE["press"] = ConfigEntry("press", "Gas pressure in capillary / bar", 150.0e-3)
        
        self.UPPE["p_av"] = ConfigEntry("p_av", "(double) Average laser power / W", 1.0)
        self.UPPE["rep"] = ConfigEntry("rep", "(double) Laser repetition rate", 1.0e3)
        self.UPPE["fwhm"] = ConfigEntry("fwhm", "(double) Full width at half maximum of laser amplitude / s", 70.71e-15)
        self.UPPE["l_0"] = ConfigEntry("l_0", "(double) Laser central wavelength / m", 800.0e-9)
        self.UPPE["ceo"] = ConfigEntry("ceo", "(double) Laser carrier envelope phase", 0.0)
        self.UPPE["waist"] = ConfigEntry("waist", "(double) Laser carrier envelope phase / m", 48e-6)
        
        self.UPPE["laser_rel_tol"] = ConfigEntry("laser_rel_tol", "(double) Laser propagation relative tolerance/error", 1.0e-8)

        self.UPPE["read_in_laser_pulse"] = ConfigEntry("read_in_laser_pulse", "(int) Switch to read in an initial laser pulse", 0)
        self.UPPE["path_A_w_R_initial"] = ConfigEntry("path_A_w_R_initial", "(std::string) Initial path of A_w_R", "")
        self.UPPE["path_A_w_I_initial"] = ConfigEntry("path_A_w_I_initial", "(std::string) Initial path of A_w_I", "")
        self.UPPE["path_w_active_initial"] = ConfigEntry("path_w_active_initial", "(std::string) Initial path of w_active", "")
        
        self.UPPE["HHGP_starting_z"] = ConfigEntry("HHGP_starting_z", "(double) Starting distance for HHGP in capillary", 0)
        self.UPPE["original_n_z"] = ConfigEntry("original_n_z", "(int) Original n_z for read in files", 300)
        self.UPPE["ending_n_z"] = ConfigEntry("ending_n_z", "(int) Ending step for UPPE block", 300)
        self.UPPE["interp_points"] = ConfigEntry("interp_points", "(int) Number of interpolation points between each step", 0)
        
        self.UPPE["gas_pressure_profile"] = ConfigEntry("gas_pressure_profile", "(std::string) Switch for gas pressure profile", "capillary")
        
        self.UPPE["HHG_lambda_min"] = ConfigEntry("HHG_lambda_min", "(double) Minimum generated high harmonic wavelength outputted / m", 123.98e-9)
        self.UPPE["HHG_lambda_max"] = ConfigEntry("HHG_lambda_max", "(double) Maximum generated high harmonic wavelength outputted / m", 6.0e-9)
        self.UPPE["HHP_E_min"] = ConfigEntry("HHP_E_min", "(double) Minimum propagated high harmonic energy outputted / eV", 10.0)
        self.UPPE["HHP_E_max"] = ConfigEntry("HHP_E_max", "(double) Maximum propagated high harmonic energy outputted / eV", 206.0)
        
        self.UPPE["output_electron_density"] = ConfigEntry("output_electron_density", "(int) Switch for outputting electron density", 0)
        
        self.UPPE["pend_path"] = ConfigEntry("pend_path", "(std::string) Output filename pending switch", "prepend")
        self.UPPE["path_input_j0"] = ConfigEntry("path_input_j0", "(std::string) Input path to J0_zeros.bin", "../../input/J0_zeros.bin")
        
        self.UPPE["path_A_w_R"] = ConfigEntry("path_A_w_R", "(std::string) Output path of A_w_R", "../output/A_w_R.bin")
        self.UPPE["path_A_w_I"] = ConfigEntry("path_A_w_I", "(std::string) Output path of A_w_I", "../output/A_w_I.bin")
        self.UPPE["path_w_active"] = ConfigEntry("path_w_active", "(std::string) Output path of w_active", "../output/w_active.bin")
        
        self.UPPE["path_electron_density"] = ConfigEntry("path_electron_density", "(std::string) Path of electron_density", "../output/electron_density.bin")
        
        self.UPPE["path_HHG_R"] = ConfigEntry("path_HHG_R", "(std::string) Path of HHG_R", "../output/HHG_R.bin")
        self.UPPE["path_HHG_I"] = ConfigEntry("path_HHG_I", "(std::string) Path of HHG_I", "../output/HHG_I.bin")
        self.UPPE["path_HHG_w"] = ConfigEntry("path_HHG_w", "(std::string) Path of HHG w", "../output/HHG_w.bin")
        
        self.UPPE["path_HHG_E"] = ConfigEntry("path_HHG_E", "(std::string) Path of HHG E", "../output/HHG_E.bin")
        
        self.UPPE["path_HHP_R"] = ConfigEntry("path_HHP_R", "(std::string) Path of HHP_R", "../output/HHP_R.bin")
        self.UPPE["path_HHP_I"] = ConfigEntry("path_HHP_I", "(std::string) Path of HHP_I", "../output/HHP_I.bin")
        self.UPPE["path_HHP_w"] = ConfigEntry("path_HHP_w", "(std::string) Path of HHP w", "../output/HHP_w.bin")
        
        self.UPPE["path_config_log"] = ConfigEntry("path_config_log", "(std::string) Output path to config_log.txt", "../output/config_log.txt")
        
        # ---------------- #
        # -  XNLO Config - #
        # ---------------- #
        
        self.XNLO = OrderedDict([])
        
        self.XNLO["atoms_per_worker"] = ConfigEntry("atoms_per_worker", "(int) The number of atoms per worker thread", 2)
        
        self.XNLO["x_min"] = ConfigEntry("x_min", "(double) The x_min value, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["x_max"] = ConfigEntry("x_max", "(double) The x_max value, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.XNLO["N_t"] = ConfigEntry("N_t", "(int) The N_t value", 20000)
        self.XNLO["t_min"] = ConfigEntry("t_min", "(double) The t_min value", -75.0e-15)
        self.XNLO["t_max"] = ConfigEntry("t_max", "(double) The t_max value", 100.0e-15)
        
        self.XNLO["P_av"] = ConfigEntry("P_av", "(double) Average laser power, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["RR"] = ConfigEntry("RR", "(double) Repetition rate, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["FWHM"] = ConfigEntry("FWHM", "(double) Laser full width at half maximum, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["l_0"] = ConfigEntry("l_0", "(double) Laser central wavelength, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["CEO"] = ConfigEntry("CEO", "(double) Laser carrier envelope phase, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.XNLO["spot_radius"] = ConfigEntry("spot_radius", "(double) Laser spot radius, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.XNLO["alpha"] = ConfigEntry("alpha", "(double) Value to scale atomic potential with", 1.45)
        
        self.XNLO["read_in_laser_pulse"] = ConfigEntry("read_in_laser_pulse", "(int) Switch to read in laser pulse", 0)
        self.XNLO["output_wavefunction"] = ConfigEntry("output_wavefunction", "(int) Switch to output wavefunction", 0)
        
        self.XNLO["pend_path"] = ConfigEntry("pend_path", "(std::string) Pending switch", "prepend")
        
        self.XNLO["path_laser_A_w_R"] = ConfigEntry("path_laser_A_w_R", "(std::string) Path to A_w_R of laser pulse input", "../../UPPE/output/A_w_R.bin")
        self.XNLO["path_laser_A_w_I"] = ConfigEntry("path_laser_A_w_I", "(std::string) Path to A_w_I of laser pulse input", "../../UPPE/output/A_w_I.bin")
        self.XNLO["path_laser_w_active"] = ConfigEntry("path_laser_w_active", "(std::string) Path to w_active of laser pulse input", "../../UPPE/output/w_active.bin")
        
        self.XNLO["path_dipole"] = ConfigEntry("path_dipole", "(std::string) Output path of acceleration", "../output/acceleration.bin")
        self.XNLO["path_w"] = ConfigEntry("path_w", "(std::string) Output path of w", "../output/w.bin")
        self.XNLO["path_E"] = ConfigEntry("path_E", "(std::string) Output path of E", "../output/E.bin")
        
        self.XNLO["path_config_log"] = ConfigEntry("path_config_log", "(std::string) Output path of config_log", "../output/config_log.txt")

        # ---------------- #
        # -  HHGP Config - #
        # ---------------- #
        
        self.HHGP = OrderedDict([])
        
        self.HHGP["n_z"] = ConfigEntry("n_z", "(int) Number of steps in Z, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["n_r"] = ConfigEntry("n_r", "(int) Number of radial points, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["n_m"] = ConfigEntry("n_m", "(int) Number of modes, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.HHGP["n_t"] = ConfigEntry("n_t", "(int) Number of time steps, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["T"] = ConfigEntry("T", "(double) Total time simulated / s, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["w_active_min"] = ConfigEntry("w_active_min", "(double) Minimum positive angular frequency / rad/s, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["w_active_max"] = ConfigEntry("w_active_max", "(double) Maximum positive angular frequency / rad/s, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.HHGP["Z"] = ConfigEntry("Z", "(double) Length of capillary / m, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["R"] = ConfigEntry("R", "(double) Radius of capillary / m, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.HHGP["press"] = ConfigEntry("press", "(double) Pressure of the gas in the capillary / bar, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.HHGP["p_av"] = ConfigEntry("p_av", "(double) Average laser power / W, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["rep"] = ConfigEntry("rep", "(double) Laser repetition rate, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["fwhm"] = ConfigEntry("fwhm", "(double) Laser full width at half maximum / s, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["l_0"] = ConfigEntry("l_0", "(double) Laser central wavelength / m, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["ceo"] = ConfigEntry("ceo", "(double) Laser carrier envelope phase, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        self.HHGP["waist"] = ConfigEntry("waist", "(double) Laser waist / m, set through UPPE", None, active=False, inactiveMessage="(Set through UPPE)")
        
        self.HHGP["inital_propagation_step"] = ConfigEntry("inital_propagation_step", "Initial propagation step", 1)
        
        self.HHGP["gas_pressure_profile"] = ConfigEntry("gas_pressure_profile", "(std::string) Switch for gas pressure profile", "capillary")
        
        self.HHGP["pend_path"] = ConfigEntry("pend_path", "(std::string) Pending switch", "prepend")
        self.HHGP["path_input_j0"] = ConfigEntry("path_input_j0", "(std::string) Input path to J0_zeros.bin", "../../input/J0_zeros.bin")
        self.HHGP["path_A_w_R"] = ConfigEntry("path_A_w_R", "(std::string) Output path of A_w_R", "../output/HHG_R.bin")
        self.HHGP["path_A_w_I"] = ConfigEntry("path_A_w_I", "(std::string) Output path of A_w_I", "../output/HHG_I.bin")
        self.HHGP["path_w_active"] = ConfigEntry("path_w_active", "(std::string) Output path of w_active", "../output/HHG_w.bin")
        
        self.HHGP["path_config_log"] = ConfigEntry("path_config_log", "(std::string) Output path to config_log.txt", "../output/config_log.txt")

    def setUPPEBaseConfig(self):
        self.UPPE = OrderedDict([])
        
        self.UPPE["n_z"] = ConfigEntry("n_z", "(int) Number of steps along the capillary", 300)
        self.UPPE["n_r"] = ConfigEntry("n_r", "(int) Number of radial positions in the capillary", 20)
        self.UPPE["n_m"] = ConfigEntry("n_m", "(int) Number of modes", 20)
        
        self.UPPE["output_sampling_rate"] = ConfigEntry("output_sampling_rate", "(int) Output sampling rate in Z", 1)
        
        self.UPPE["n_t"] = ConfigEntry("n_t", "(int) Number of time steps", 16384)
        self.UPPE["T"] = ConfigEntry("T", "(double) Total time being simulated over / s", 1000.0e-15)
        self.UPPE["w_active_min"] = ConfigEntry("w_active_min", "(double) Minimum positive angular frequency / rad/sec", 2.0e14)
        self.UPPE["w_active_max"] = ConfigEntry("w_active_max", "(double) Maximum positive angular frequency / rad/sec", 8.0e15)
        
        self.UPPE["Z"] = ConfigEntry("Z", "(double) Length of capillary / m", 70.0e-3)
        self.UPPE["R"] = ConfigEntry("R", "(double) Radius of capillary / m", 75.0e-6)
        
        self.UPPE["press"] = ConfigEntry("press", "Gas pressure in capillary / bar", 150.0e-3)
        
        self.UPPE["p_av"] = ConfigEntry("p_av", "(double) Average laser power / W", 1.0)
        self.UPPE["rep"] = ConfigEntry("rep", "(double) Laser repetition rate", 1.0e3)
        self.UPPE["fwhm"] = ConfigEntry("fwhm", "(double) Full width at half maximum of laser amplitude / s", 70.71e-15)
        self.UPPE["l_0"] = ConfigEntry("l_0", "(double) Laser central wavelength / m", 800.0e-9)
        self.UPPE["ceo"] = ConfigEntry("ceo", "(double) Laser carrier envelope phase", 0.0)
        self.UPPE["waist"] = ConfigEntry("waist", "(double) Laser carrier envelope phase / m", 48e-6)
        
        self.UPPE["laser_rel_tol"] = ConfigEntry("laser_rel_tol", "(double) Laser propagation relative tolerance/error", 1.0e-8)

        self.UPPE["read_in_laser_pulse"] = ConfigEntry("read_in_laser_pulse", "(int) Switch to read in an initial laser pulse", 0)
        self.UPPE["path_A_w_R_initial"] = ConfigEntry("path_A_w_R_initial", "(std::string) Initial path of A_w_R", "")
        self.UPPE["path_A_w_I_initial"] = ConfigEntry("path_A_w_I_initial", "(std::string) Initial path of A_w_I", "")
        self.UPPE["path_w_active_initial"] = ConfigEntry("path_w_active_initial", "(std::string) Initial path of w_active", "")
        
        #self.UPPE["HHGP_starting_z"] = ConfigEntry("HHGP_starting_z", "(double) Starting distance for HHGP in capillary", 0)
        #self.UPPE["original_n_z"] = ConfigEntry("original_n_z", "(int) Original n_z for read in files", 300)
        #self.UPPE["ending_n_z"] = ConfigEntry("ending_n_z", "(int) Ending step for UPPE block", 300)
        #self.UPPE["interp_points"] = ConfigEntry("interp_points", "(int) Number of interpolation points between each step", 0)
        
        self.UPPE["gas_pressure_profile"] = ConfigEntry("gas_pressure_profile", "(std::string) Switch for gas pressure profile", "capillary")
        
        #self.UPPE["HHG_lambda_min"] = ConfigEntry("HHG_lambda_min", "(double) Minimum generated high harmonic wavelength outputted / m", 123.98e-9)
        #self.UPPE["HHG_lambda_max"] = ConfigEntry("HHG_lambda_max", "(double) Maximum generated high harmonic wavelength outputted / m", 6.0e-9)
        #self.UPPE["HHP_E_min"] = ConfigEntry("HHP_E_min", "(double) Minimum propagated high harmonic energy outputted / eV", 10.0)
        #self.UPPE["HHP_E_max"] = ConfigEntry("HHP_E_max", "(double) Maximum propagated high harmonic energy outputted / eV", 206.0)
        
        self.UPPE["output_electron_density"] = ConfigEntry("output_electron_density", "(int) Switch for outputting electron density", 0)
        
        self.UPPE["pend_path"] = ConfigEntry("pend_path", "(std::string) Output filename pending switch", "prepend")
        self.UPPE["path_input_j0"] = ConfigEntry("path_input_j0", "(std::string) Input path to J0_zeros.bin", "../../input/J0_zeros.bin")
        
        self.UPPE["path_A_w_R"] = ConfigEntry("path_A_w_R", "(std::string) Output path of A_w_R", "../output/A_w_R.bin")
        self.UPPE["path_A_w_I"] = ConfigEntry("path_A_w_I", "(std::string) Output path of A_w_I", "../output/A_w_I.bin")
        self.UPPE["path_w_active"] = ConfigEntry("path_w_active", "(std::string) Output path of w_active", "../output/w_active.bin")
        
        self.UPPE["path_electron_density"] = ConfigEntry("path_electron_density", "(std::string) Path of electron_density", "../output/electron_density.bin")
        
        #self.UPPE["path_HHG_R"] = ConfigEntry("path_HHG_R", "(std::string) Path of HHG_R", "../output/HHG_R.bin")
        #self.UPPE["path_HHG_I"] = ConfigEntry("path_HHG_I", "(std::string) Path of HHG_I", "../output/HHG_I.bin")
        #self.UPPE["path_HHG_w"] = ConfigEntry("path_HHG_w", "(std::string) Path of HHG w", "../output/HHG_w.bin")
        
        #self.UPPE["path_HHG_E"] = ConfigEntry("path_HHG_E", "(std::string) Path of HHG E", "../output/HHG_E.bin")
        
        #self.UPPE["path_HHP_R"] = ConfigEntry("path_HHP_R", "(std::string) Path of HHP_R", "../output/HHP_R.bin")
        #self.UPPE["path_HHP_I"] = ConfigEntry("path_HHP_I", "(std::string) Path of HHP_I", "../output/HHP_I.bin")
        #self.UPPE["path_HHP_w"] = ConfigEntry("path_HHP_w", "(std::string) Path of HHP w", "../output/HHP_w.bin")
        
        self.UPPE["path_config_log"] = ConfigEntry("path_config_log", "(std::string) Output path to config_log.txt", "../output/config_log.txt")
    
    def setXNLOBaseConfig(self):
        pass
    
    def setHHGPBaseConfig(self):
        pass
        
    def setBaseConfig(self):
        if self.configType == "UXH":
            self.setUXHBaseConfig()
        elif self.configType == "UPPE":
            self.setUPPEBaseConfig()
        elif self.configType == "XNLO":
            self.setXNLOBaseConfig()
        elif self.configType == "HHGP":
            self.setHHGPBaseConfig()
            
    def loadConfig(self, filename, configType):
        config_tmp = self.readConfig(filename)
        
        if configType == "UPPE":
            self.UPPEFilename = filename
            config = self.UPPE
        elif configType == "XNLO":
            self.XNLOFilename = filename
            config = self.XNLO
        elif configType == "HHGP":
            self.HHGPFilename = filename
            config = self.HHGP
        
        for key, value in config_tmp.items():
            try:
                if config[key].active == True:
                    try:
                        config[key].value = value['value']
                        config[key].value_str = value['value_str']
                        config[key].description = value['description']
                    except:
                        print("{}.loadConfig(): Warning, could not set config entry '{}' with value {}".format(type(self).__name__, key, value['value']))
            except:
                pass
            
        
if __name__ == "__main__":
    test = Config("UXH")
    test.loadConfig("../../UPPE/configFiles/config_UPPE.txt", "UPPE")
    test.loadConfig("../../UPPE/configFiles/config_XNLO.txt", "XNLO")
    test.loadConfig("../../UPPE/configFiles/config_HHGP.txt", "HHGP")
    
    for key, value in test.UPPE.items():
        print(key, value.value, value.description)
        
    for key, value in test.XNLO.items():
        print(key, value.value, value.description)
        
    for key, value in test.HHGP.items():
        print(key, value.value, value.description)