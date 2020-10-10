#!/usr/bin/env python

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (QApplication, QCheckBox, QComboBox,
        QDialog, QFileDialog, QGridLayout, QHBoxLayout, QLabel, QLineEdit,
        QPushButton, QScrollArea, QSizePolicy, QStyleFactory, QTabWidget,
        QVBoxLayout, QWidget)

import sys, os

from Config import Config
        
class UXH_configEditor(QDialog):
    def __init__(self, parent=None):
        super(UXH_configEditor, self).__init__(parent)
        
        self.UXHConfig = Config("UXH")
        self.UXHConfig.loadConfig("../../UPPE/configFiles/config_UPPE.txt", "UPPE")
        self.UXHConfig.loadConfig("../../UPPE/configFiles/config_XNLO.txt", "XNLO")
        self.UXHConfig.loadConfig("../../UPPE/configFiles/config_HHGP.txt", "HHGP")
        
        QApplication.setStyle(QStyleFactory.create("Fusion"))
        QApplication.setPalette(QApplication.style().standardPalette())

        simulationTypeComboBox = QComboBox()
        simulationTypeComboBox.addItems(["UXH", "UPPE", "XNLO", "HHGP"])

        simulationTypeLabel = QLabel("Simulation Type:")
        simulationTypeLabel.setBuddy(simulationTypeComboBox)
        
        self.workingDir = os.getcwd()
        self.workingDirLineEdit = QLineEdit(self.workingDir)
        workingDirLabel = QLabel("Working Directory:")
        workingDirLabel.setBuddy(self.workingDirLineEdit)
        browseWorkingDirButton = QPushButton("Select Folder")
        browseWorkingDirButton.clicked.connect(self.pickNewWorkingDir)

        self.useStylePaletteCheckBox = QCheckBox("&Use style's standard palette")
        self.useStylePaletteCheckBox.setChecked(True)

        self.createUXHTabWidget()

        simulationTypeComboBox.activated[str].connect(self.simulationType)

        topLayout = QHBoxLayout()
        topLayout.addWidget(simulationTypeLabel)
        topLayout.addWidget(simulationTypeComboBox)
        topLayout.addWidget(workingDirLabel)
        topLayout.addWidget(self.workingDirLineEdit)
        topLayout.addWidget(browseWorkingDirButton)

        mainLayout = QGridLayout()
        mainLayout.addLayout(topLayout, 0, 0)
        mainLayout.addWidget(self.UXHTabWidget, 1, 0)
        
        self.setLayout(mainLayout)

        self.setWindowTitle("UXH configEditor")
        
        self.setGeometry(500, 500, 900, 500)
    
    def pickNewWorkingDir(self):
        self.workingDir = self.workingDirLineEdit.text()
        dialog = QFileDialog()
        newPath = dialog.getExistingDirectory(self, "Select Folder", self.workingDir)
        if newPath != "":
            self.workingDir = newPath
            self.workingDirLineEdit.setText(self.workingDir)
            
    def selectFile(self, file):
        dialog = QFileDialog()
        return dialog.getOpenFileName(self, "Select File", file.rpartition("/")[0], "*")
        
    def selectUPPEFile(self):
        newFile = self.selectFile(self.UPPEConfigFileLocation.text())[0]
        self.UPPEConfigFileLocation.setText(newFile)
        self.UXHConfig.loadConfig(newFile, "UPPE")
        
    def selectXNLOFile(self):
        newFile = self.selectFile(self.XNLOConfigFileLocation.text())[0]
        self.XNLOConfigFileLocation.setText(newFile)
        self.UXHConfig.loadConfig(newFile, "XNLO")
        
    def selectHHGPFile(self):
        newFile = self.selectFile(self.HHGPConfigFileLocation.text())[0]
        self.HHGPConfigFileLocation.setText(newFile)
        self.UXHConfig.loadConfig(newFile, "HHGP")
        
    def simulationType(self):
        pass
    
    def createUPPEConfigValuesTab(self):
                        
        self.UPPEConfigValuesTab = QWidget()
        
        self.UPPEConfigFileLocation = QLineEdit(self.UXHConfig.UPPEFilename)
        self.UPPEConfigFileLocationLabel = QLabel("UPPE config file:")
        self.UPPEConfigFileLocationLabel.setBuddy(self.UPPEConfigFileLocation)
        self.UPPEConfigFileLocationButton = QPushButton("Select File")
        self.UPPEConfigFileLocationButton.clicked.connect(self.selectUPPEFile)
        
        self.UPPEConfigValuesTabTopLayout = QHBoxLayout()
        self.UPPEConfigValuesTabTopLayout.addWidget(self.UPPEConfigFileLocationLabel)
        self.UPPEConfigValuesTabTopLayout.addWidget(self.UPPEConfigFileLocation)
        self.UPPEConfigValuesTabTopLayout.addWidget(self.UPPEConfigFileLocationButton)

        for key, value in self.UXHConfig.UPPE.items():
            self.UXHConfig.UPPE[key].addWidgets(QLabel(key),
                                           QLineEdit(value.value_str),
                                           QLabel(value.description))
            if self.UXHConfig.UPPE[key].active == False:
                self.UXHConfig.UPPE[key].valueWidget.setEnabled(False)
            
        
        self.UPPEConfigValuesTabLayout = QVBoxLayout(self.UPPEConfigValuesTab)
        self.UPPEConfigValuesTabScrollArea = QScrollArea(self.UPPEConfigValuesTab)
        self.UPPEConfigValuesTabScrollArea.setWidgetResizable(True)
        self.UPPEConfigValuesTabScrollAreaWidgetContents = QWidget()
        self.UPPEConfigValuesTabGrid = QGridLayout(self.UPPEConfigValuesTabScrollAreaWidgetContents)
        self.UPPEConfigValuesTabScrollArea.setWidget(self.UPPEConfigValuesTabScrollAreaWidgetContents)
        self.UPPEConfigValuesTabLayout.addLayout(self.UPPEConfigValuesTabTopLayout)
        self.UPPEConfigValuesTabLayout.addWidget(self.UPPEConfigValuesTabScrollArea)

        self.UPPEConfigValuesTabGrid.setHorizontalSpacing(10)
                
        self.UPPEConfigValuesTabGrid.addWidget(QLabel("Name:"), 0, 0,
                                               alignment=Qt.AlignTop)
        self.UPPEConfigValuesTabGrid.addWidget(QLabel("Value:"), 0, 1,
                                               alignment=Qt.AlignTop)
        self.UPPEConfigValuesTabGrid.addWidget(QLabel("Description:"), 0, 2,
                                               alignment=Qt.AlignTop)
        
        i = 0
        for key, value in self.UXHConfig.UPPE.items():
            self.UPPEConfigValuesTabGrid.addWidget(self.UXHConfig.UPPE[key].nameWidget,
                                                   i+1, 0)
            self.UPPEConfigValuesTabGrid.addWidget(self.UXHConfig.UPPE[key].valueWidget,
                                                   i+1, 1)
            self.UPPEConfigValuesTabGrid.addWidget(self.UXHConfig.UPPE[key].descriptionWidget,
                                                   i+1, 2)
            i += 1
        del i
                        
        self.UPPEConfigValuesTabGrid.setRowStretch(len(self.UXHConfig.UPPE),
                                                   len(self.UXHConfig.UPPE))
        self.UPPEConfigValuesTabGrid.setColumnStretch(1, 1)
                                
    def createXNLOConfigValuesTab(self):
                
        self.XNLOConfigValuesTab = QWidget()
        
        self.XNLOConfigFileLocation = QLineEdit(self.UXHConfig.XNLOFilename)
        self.XNLOConfigFileLocationLabel = QLabel("XNLO config file:")
        self.XNLOConfigFileLocationLabel.setBuddy(self.XNLOConfigFileLocation)
        self.XNLOConfigFileLocationButton = QPushButton("Select File")
        self.XNLOConfigFileLocationButton.clicked.connect(self.selectXNLOFile)

        self.XNLOConfigValuesTabTopLayout = QHBoxLayout()
        self.XNLOConfigValuesTabTopLayout.addWidget(self.XNLOConfigFileLocationLabel)
        self.XNLOConfigValuesTabTopLayout.addWidget(self.XNLOConfigFileLocation)
        self.XNLOConfigValuesTabTopLayout.addWidget(self.XNLOConfigFileLocationButton)
                
        for key, value in self.UXHConfig.XNLO.items():
            self.UXHConfig.XNLO[key].addWidgets(QLabel(key),
                                           QLineEdit(value.value_str),
                                           QLabel(value.description))
            if self.UXHConfig.XNLO[key].active == False:
                self.UXHConfig.XNLO[key].valueWidget.setEnabled(False)
        
        self.XNLOConfigValuesTabLayout = QVBoxLayout(self.XNLOConfigValuesTab)
        self.XNLOConfigValuesTabScrollArea = QScrollArea(self.XNLOConfigValuesTab)
        self.XNLOConfigValuesTabScrollArea.setWidgetResizable(True)
        self.XNLOConfigValuesTabScrollAreaWidgetContents = QWidget()
        self.XNLOConfigValuesTabGrid = QGridLayout(self.XNLOConfigValuesTabScrollAreaWidgetContents)
        self.XNLOConfigValuesTabScrollArea.setWidget(self.XNLOConfigValuesTabScrollAreaWidgetContents)
        self.XNLOConfigValuesTabLayout.addLayout(self.XNLOConfigValuesTabTopLayout)
        self.XNLOConfigValuesTabLayout.addWidget(self.XNLOConfigValuesTabScrollArea)
    

        self.XNLOConfigValuesTabGrid.setHorizontalSpacing(10)
                
        self.XNLOConfigValuesTabGrid.addWidget(QLabel("Name:"), 0, 0,
                                               alignment=Qt.AlignTop)
        self.XNLOConfigValuesTabGrid.addWidget(QLabel("Value:"), 0, 1,
                                               alignment=Qt.AlignTop)
        self.XNLOConfigValuesTabGrid.addWidget(QLabel("Description:"), 0, 2,
                                               alignment=Qt.AlignTop)
            
        i = 0
        for key, value in self.UXHConfig.XNLO.items():
            self.XNLOConfigValuesTabGrid.addWidget(self.UXHConfig.XNLO[key].nameWidget,
                                                   i+1, 0)
            self.XNLOConfigValuesTabGrid.addWidget(self.UXHConfig.XNLO[key].valueWidget,
                                                   i+1, 1)
            self.XNLOConfigValuesTabGrid.addWidget(self.UXHConfig.XNLO[key].descriptionWidget,
                                                   i+1, 2)
            i += 1
        del i
            
        self.XNLOConfigValuesTabGrid.setRowStretch(len(self.UXHConfig.XNLO),
                                                   len(self.UXHConfig.XNLO))
        self.XNLOConfigValuesTabGrid.setColumnStretch(1, 1)
                
    def createHHGPConfigValuesTab(self):
                
        self.HHGPConfigValuesTab = QWidget()
        
        self.HHGPConfigFileLocation = QLineEdit(self.UXHConfig.HHGPFilename)
        self.HHGPConfigFileLocationLabel = QLabel("HHGP config file:")
        self.HHGPConfigFileLocationLabel.setBuddy(self.HHGPConfigFileLocation)
        self.HHGPConfigFileLocationButton = QPushButton("Select File")
        self.HHGPConfigFileLocationButton.clicked.connect(self.selectHHGPFile)

        self.HHGPConfigValuesTabTopLayout = QHBoxLayout()
        self.HHGPConfigValuesTabTopLayout.addWidget(self.HHGPConfigFileLocationLabel)
        self.HHGPConfigValuesTabTopLayout.addWidget(self.HHGPConfigFileLocation)
        self.HHGPConfigValuesTabTopLayout.addWidget(self.HHGPConfigFileLocationButton)
        
        for key, value in self.UXHConfig.HHGP.items():
            self.UXHConfig.HHGP[key].addWidgets(QLabel(key),
                                           QLineEdit(value.value_str),
                                           QLabel(value.description))
            if self.UXHConfig.HHGP[key].active == False:
                self.UXHConfig.HHGP[key].valueWidget.setEnabled(False)
        
        self.HHGPConfigValuesTabLayout = QVBoxLayout(self.HHGPConfigValuesTab)
        self.HHGPConfigValuesTabScrollArea = QScrollArea(self.HHGPConfigValuesTab)
        self.HHGPConfigValuesTabScrollArea.setWidgetResizable(True)
        self.HHGPConfigValuesTabScrollAreaWidgetContents = QWidget()
        self.HHGPConfigValuesTabGrid = QGridLayout(self.HHGPConfigValuesTabScrollAreaWidgetContents)
        self.HHGPConfigValuesTabScrollArea.setWidget(self.HHGPConfigValuesTabScrollAreaWidgetContents)
        self.HHGPConfigValuesTabLayout.addLayout(self.HHGPConfigValuesTabTopLayout)
        self.HHGPConfigValuesTabLayout.addWidget(self.HHGPConfigValuesTabScrollArea)
    

        self.HHGPConfigValuesTabGrid.setHorizontalSpacing(10)
                
        self.HHGPConfigValuesTabGrid.addWidget(QLabel("Name:"), 0, 0,
                                               alignment=Qt.AlignTop)
        self.HHGPConfigValuesTabGrid.addWidget(QLabel("Value:"), 0, 1,
                                               alignment=Qt.AlignTop)
        self.HHGPConfigValuesTabGrid.addWidget(QLabel("Description:"), 0, 2,
                                               alignment=Qt.AlignTop)
        
        i = 0
        for key, value in self.UXHConfig.HHGP.items():
            self.HHGPConfigValuesTabGrid.addWidget(self.UXHConfig.HHGP[key].nameWidget,
                                                   i+1, 0)
            self.HHGPConfigValuesTabGrid.addWidget(self.UXHConfig.HHGP[key].valueWidget,
                                                   i+1, 1)
            self.HHGPConfigValuesTabGrid.addWidget(self.UXHConfig.HHGP[key].descriptionWidget,
                                                   i+1, 2)
            i += 1
        del i
            
        self.HHGPConfigValuesTabGrid.setRowStretch(len(self.UXHConfig.HHGP),
                                                   len(self.UXHConfig.HHGP))
        self.HHGPConfigValuesTabGrid.setColumnStretch(1, 1)
        
    def createUXHTabWidget(self):
        self.UXHTabWidget = QTabWidget()
        self.UXHTabWidget.setSizePolicy(QSizePolicy.Preferred,
                                        QSizePolicy.Ignored)
        
        self.createUPPEConfigValuesTab()
        self.createXNLOConfigValuesTab()
        self.createHHGPConfigValuesTab()
        
        self.UXHTabWidget.addTab(self.UPPEConfigValuesTab, "UPPE")
        self.UXHTabWidget.addTab(self.XNLOConfigValuesTab, "XNLO")
        self.UXHTabWidget.addTab(self.HHGPConfigValuesTab, "HHGP")


if __name__ == '__main__':
    appctxt = QApplication([])
    gallery = UXH_configEditor()
    gallery.show()
    sys.exit(appctxt.exec_())
