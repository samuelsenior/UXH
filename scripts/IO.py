#!/usr/bin/env python

# -*- coding: utf-8 -*-
"""
Created on Sun Oct  4 18:18:49 2020

@author: smsen
"""

import os
import csv
import re
from collections import OrderedDict

class ConfigIO:
    def __init__(self, name, fileName):
        self.name = name
        self.config = OrderedDict([])
        self.fileName = fileName
    
    def readConfig(self):
        with open(self.fileName) as configFile:
            config_tmp = csv.reader(configFile)
            for config_entry in config_tmp:
                if len(config_entry) > 0:
                    config_entry = config_entry[0]
                    config_entry = re.findall(r'\{([^{]+)\}', config_entry)
                    if len(config_entry) == 3:
                        self.config[config_entry[0]] = {"value":config_entry[1], "value_str":config_entry[1], "description":config_entry[2]}
                        
                        if self.config[config_entry[0]]['description'][0] == "(":
                            dataType = re.findall(r'\(([^()]+)\)', self.config[config_entry[0]]['description'])
                            
                            if dataType[0] == "default":
                                dataType.pop(0)

                            if dataType[0] == "int":
                                self.config[config_entry[0]]['value'] = int(self.config[config_entry[0]]['value'])
                            elif dataType[0] == "double":
                                self.config[config_entry[0]]['value'] = float(self.config[config_entry[0]]['value'])
                            elif dataType[0] == "std::string":
                                pass
                            else:
                                print('{}.readConfig(): Warning: Unrecognised datatype "{}", assuming it\'s "std::string"...'.format(self.name, dataType[0]))
                    
    def writeConfig(self):
        pass
    
    
if __name__ == "__main__":
    print(os.getcwd())
    test = ConfigIO("UPPE", "../UPPE/configFiles/config_UPPE.txt")
    test.readConfig()
    
    for key, value in test.config.items():
        print(key, value)