TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

CODECFORTR = Windows-1251
TRANSLATIONS += kvant_en.ts
TRANSLATIONS += kvant_fr.ts

CONFIG += debug

RESOURCES = kvant.qrc

# Input
HEADERS += BoundaryCondition.h
HEADERS += BreakStatus.h
HEADERS += EGView.h
HEADERS += EnzView.h
HEADERS += EofqaView.h
HEADERS += FindBand.h
HEADERS += GView.h
HEADERS += LevelModel.h
HEADERS += LevelNumber.h
HEADERS += LineEditDelegate.h
HEADERS += MainWindow.h
HEADERS += Matrix.h
HEADERS += MomentumView.h
HEADERS += MyParam.h
HEADERS += MySplitter.h
HEADERS += PhysicalModel.h
HEADERS += Plotter.h
HEADERS += PlotterDialog.h
HEADERS += PotentialModel.h
HEADERS += PotentialViewMovable.h
HEADERS += PotentialViewMultiWell.h
HEADERS += ProgramSetting.h
HEADERS += QuasiLevelModel.h
HEADERS += ScalePhin.h
HEADERS += ScalePsin.h
HEADERS += ScalesUx.h
HEADERS += ScaleTE.h
HEADERS += ScaleWPK.h
HEADERS += ScaleWPX.h
HEADERS += TimeView.h
HEADERS += TofzView.h
HEADERS += TransmissionView.h
HEADERS += Uch2x.h
HEADERS += Ulinear.h
HEADERS += Uparabolic.h
HEADERS += Version.h
HEADERS += WaveFunctionView.h
HEADERS += WavePacketKView.h
HEADERS += WavePacketXView.h
HEADERS += WPparametersM.h
HEADERS += WPparametersP.h
HEADERS += Zview.h

SOURCES += BoundaryCondition.cpp
SOURCES += BreakStatus.cpp
SOURCES += EGView.cpp
SOURCES += EnzView.cpp
SOURCES += EofqaView.cpp
SOURCES += FindBand.cpp
SOURCES += GView.cpp
SOURCES += LevelModel.cpp
SOURCES += LevelNumber.cpp
SOURCES += LineEditDelegate.cpp
SOURCES += main.cpp
SOURCES += MainWindow.cpp
SOURCES += MomentumView.cpp
SOURCES += MyParam.cpp
SOURCES += MySplitter.cpp
SOURCES += PhysicalModel.cpp
SOURCES += Plotter.cpp
SOURCES += PlotterDialog.cpp
SOURCES += PotentialModel.cpp
SOURCES += PotentialViewMovable.cpp
SOURCES += PotentialViewMultiWell.cpp
SOURCES += PotentialWidget.cpp
SOURCES += ProgramSetting.cpp
SOURCES += QuasiLevelModel.cpp
SOURCES += ScalePhin.cpp
SOURCES += ScalePsin.cpp
SOURCES += ScalesUx.cpp
SOURCES += ScaleWPK.cpp
SOURCES += ScaleWPX.cpp
SOURCES += TimeView.cpp
SOURCES += TofzView.cpp
SOURCES += TransmissionView.cpp
SOURCES += Uch2x.cpp
SOURCES += Ulinear.cpp
SOURCES += Uparabolic.cpp
SOURCES += WaveFunctionView.cpp
SOURCES += WavePacketKView.cpp
SOURCES += WavePacketXView.cpp
SOURCES += WPparametersM.cpp
SOURCES += WPparametersP.cpp
SOURCES += Zview.cpp

#FORMS += define-potential.ui
