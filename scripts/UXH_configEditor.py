#!/usr/bin/env python

from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (QApplication, QCheckBox, QComboBox,
        QDialog, QFileDialog, QGridLayout, QHBoxLayout, QLabel, QLineEdit,
        QPushButton, QSizePolicy, QStyleFactory, QTabWidget, QWidget)

import sys, os

class ConfigEntry:
    def __init__(self, name, desc):
        self.nameWidget = name
        self.descWidget = desc
        self.valueTextBox = QLineEdit()
        
        self.name = self.nameWidget.text()
        self.desc = self.descWidget.text()
        self.value = 0
        

class UXH_configEditor(QDialog):
    def __init__(self, parent=None):
        super(UXH_configEditor, self).__init__(parent)
        
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
        browseWorkingDirButton = QPushButton()
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
        
        self.setGeometry(500, 500, 500, 250)
    
    def pickNewWorkingDir(self):
        dialog = QFileDialog()
        newPath = dialog.getExistingDirectory(self, "Select Folder", self.workingDir)
        if newPath != "":
            self.workingDir = newPath
            self.workingDirLineEdit.setText(self.workingDir)
        
    def simulationType(self):
        pass
        
    def createUXHTabWidget(self):
        self.UXHTabWidget = QTabWidget()
        self.UXHTabWidget.setSizePolicy(QSizePolicy.Preferred,
                                        QSizePolicy.Ignored)
        
        UPPETab = QWidget()
        
        UPPEClasses = [ConfigEntry(QLabel("P_av ", self), QLabel("Average laser power / W", self)),
                       ConfigEntry(QLabel("l_0 ", self), QLabel("Driving wavelength / m", self)),
                       ConfigEntry(QLabel("FWHM ", self), QLabel("Full width half maximum / s", self)),
                       ConfigEntry(QLabel("RepRate ", self), QLabel("Repetition rate", self))]
        
        grid = QGridLayout()
        grid.setHorizontalSpacing(10)
        UPPETab.setLayout(grid)
        
        grid.addWidget(QLabel("Name:"), 0, 0, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Value:"), 0, 1, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Description:"), 0, 2, alignment=Qt.AlignTop)
        
        for i, config in enumerate(UPPEClasses):
            grid.addWidget(UPPEClasses[i].nameWidget, i+1, 0, alignment=Qt.AlignTop)
            grid.addWidget(UPPEClasses[i].valueTextBox, i+1, 1, alignment=Qt.AlignTop)
            grid.addWidget(UPPEClasses[i].descWidget, i+1, 2, alignment=Qt.AlignTop)
            
        grid.setRowStretch(len(UPPEClasses), len(UPPEClasses))
        grid.setColumnStretch(2, 2)
        
            
        XNLOTab = QWidget()
        
        XNLOClasses = [ConfigEntry(QLabel("P_av ", self), QLabel("Average laser power / W", self)),
                       ConfigEntry(QLabel("l_0 ", self), QLabel("Driving wavelength / m", self)),
                       ConfigEntry(QLabel("FWHM ", self), QLabel("Full width half maximum / s", self)),
                       ConfigEntry(QLabel("RepRate ", self), QLabel("Repetition rate", self))]
        
        
        
        grid = QGridLayout()
        grid.setHorizontalSpacing(10)
        XNLOTab.setLayout(grid)
        
        grid.addWidget(QLabel("Name:"), 0, 0, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Value:"), 0, 1, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Description:"), 0, 2, alignment=Qt.AlignTop)
        
        for i, config in enumerate(XNLOClasses):
            grid.addWidget(XNLOClasses[i].nameWidget, i+1, 0, alignment=Qt.AlignTop)
            grid.addWidget(XNLOClasses[i].valueTextBox, i+1, 1, alignment=Qt.AlignTop)
            grid.addWidget(XNLOClasses[i].descWidget, i+1, 2, alignment=Qt.AlignTop)
            
        grid.setRowStretch(len(XNLOClasses), len(XNLOClasses))
        grid.setColumnStretch(2, 2)
            
            
        HHGPTab = QWidget()
        
        HHGPClasses = [ConfigEntry(QLabel("P_av ", self), QLabel("Average laser power / W", self)),
                       ConfigEntry(QLabel("l_0 ", self), QLabel("Driving wavelength / m", self)),
                       ConfigEntry(QLabel("FWHM ", self), QLabel("Full width half maximum / s", self)),
                       ConfigEntry(QLabel("RepRate ", self), QLabel("Repetition rate", self))]
        
        grid = QGridLayout()
        grid.setHorizontalSpacing(10)
        HHGPTab.setLayout(grid)
        
        grid.addWidget(QLabel("Name:"), 0, 0, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Value:"), 0, 1, alignment=Qt.AlignTop)
        grid.addWidget(QLabel("Description:"), 0, 2, alignment=Qt.AlignTop)
        
        for i, config in enumerate(HHGPClasses):
            grid.addWidget(HHGPClasses[i].nameWidget, i+1, 0, alignment=Qt.AlignTop)
            grid.addWidget(HHGPClasses[i].valueTextBox, i+1, 1, alignment=Qt.AlignTop)
            grid.addWidget(HHGPClasses[i].descWidget, i+1, 2, alignment=Qt.AlignTop)
        
        grid.setRowStretch(len(HHGPClasses), len(HHGPClasses))
        grid.setColumnStretch(2, 2)
        
        
        self.UXHTabWidget.addTab(UPPETab, "UPPE")
        self.UXHTabWidget.addTab(XNLOTab, "XNLO")
        self.UXHTabWidget.addTab(HHGPTab, "HHGP")


if __name__ == '__main__':
    appctxt = QApplication([])
    gallery = UXH_configEditor()
    gallery.show()
    sys.exit(appctxt.exec_())
