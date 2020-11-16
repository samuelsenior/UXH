#!/usr/bin/env python

# -*- coding: utf-8 -*-
"""
Created on Sun Oct  4 18:18:49 2020

@author: smsen
"""

import os
import csv
import re

class ConfigIO:
    def __init__(self):
        pass
    
    def readConfig(self, fileName):
        config = {}
        with open(fileName) as configFile:
            config_tmp = csv.reader(configFile)
            for config_entry in config_tmp:
                if len(config_entry) > 0:
                    config_entry = config_entry[0]
                    config_entry = re.findall(r'\{([^{]+)\}', config_entry)
                    if len(config_entry) == 3:
                        config[config_entry[0]] = {"value":config_entry[1], "value_str":config_entry[1], "description":config_entry[2]}
                        
                        if config[config_entry[0]]['description'][0] == "(":
                            dataType = re.findall(r'\(([^()]+)\)', config[config_entry[0]]['description'])
                            
                            if dataType[0] == "default":
                                dataType.pop(0)

                            if dataType[0] == "int":
                                config[config_entry[0]]['value'] = int(config[config_entry[0]]['value'])
                            elif dataType[0] == "double":
                                config[config_entry[0]]['value'] = float(config[config_entry[0]]['value'])
                            elif dataType[0] == "std::string":
                                pass
                            else:
                                print('{}.readConfig(): Warning: Unrecognised datatype "{}", assuming it\'s "std::string"...'.format(type(self).__name__, dataType[0]))
        return config
    def writeConfig(self):
        pass
    
    
if __name__ == "__main__":
    print(os.getcwd())
    test = ConfigIO()
    config = test.readConfig("../../UPPE/configFiles/config_UPPE.txt")
    
    for key, value in config.items():
        print(key, value)
