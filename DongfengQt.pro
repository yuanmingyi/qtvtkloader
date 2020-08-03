#-------------------------------------------------
#
# Project created by QtCreator 2020-08-02T20:01:25
#
#-------------------------------------------------

QT       += core gui widgets opengl
unix: QT += x11extras

TARGET = DongfengQt
TEMPLATE = lib
CONFIG += staticlib create_prl

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    scenewidget.cpp \
    objimporter.cpp \
    importworker.cpp \
    dongfengvis.cpp \
    dongfenganimationcue.cpp \
    dongfenganimation.cpp \
    cameraanimation.cpp \
    animationsceneobserver.cpp

HEADERS += \
    util.h \
    timerutil.h \
    stringutil.h \
    scenewidget.h \
    objimporter.h \
    importworker.h \
    dongfengvis.h \
    dongfenganimationcue.h \
    dongfenganimation.h \
    cameraanimation.h \
    animationsceneobserver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32: VTK_DIR = $$PWD/../dongfeng/win32
else:unix: VTK_DIR = $$PWD/../dongfeng/ubuntu

VTK_Libs = \
    vtkIOInfovis-8.2 \
    vtklibxml2-8.2 \
    vtkRenderingContextOpenGL2-8.2 \
    vtkTestingRendering-8.2 \
    vtkViewsContext2D-8.2 \
    vtkFiltersProgrammable-8.2 \
    vtkFiltersVerdict-8.2 \
    vtkverdict-8.2 \
    vtkFiltersGeneric-8.2 \
    vtkTestingGenericBridge-8.2 \
    vtkDomainsChemistryOpenGL2-8.2 \
    vtkDomainsChemistry-8.2 \
    vtkIOAMR-8.2 \
    vtkFiltersAMR-8.2 \
    vtkIOExodus-8.2 \
    vtkRenderingVolumeOpenGL2-8.2 \
    vtkImagingMath-8.2 \
    vtkFiltersFlowPaths-8.2 \
    vtkFiltersHyperTree-8.2 \
    vtkImagingStencil-8.2 \
    vtkFiltersParallelImaging-8.2 \
    vtkFiltersPoints-8.2 \
    vtkFiltersSMP-8.2 \
    vtkFiltersSelection-8.2 \
    vtkIOParallel-8.2 \
    vtkIOGeometry-8.2 \
    vtkexodusII-8.2 \
    vtkFiltersParallel-8.2 \
    vtkIONetCDF-8.2 \
    vtkjsoncpp-8.2 \
    vtkFiltersTopology-8.2 \
    vtkGUISupportQtSQL-8.2 \
    vtkGeovisCore-8.2 \
    vtkproj-8.2 \
    vtkIOAsynchronous-8.2 \
    vtkIOCityGML-8.2 \
    vtkpugixml-8.2 \
    vtkIOEnSight-8.2 \
    vtkIOExportOpenGL2-8.2 \
    vtkInteractionImage-8.2 \
    vtkIOExportPDF-8.2 \
    vtkIOExport-8.2 \
    vtkRenderingGL2PSOpenGL2-8.2 \
    vtkgl2ps-8.2 \
    vtklibharu-8.2 \
    vtkIOImport-8.2 \
    vtkIOLSDyna-8.2 \
    vtkIOMINC-8.2 \
    vtkNetCDF-8.2 \
    vtkIOMovie-8.2 \
    vtktheora-8.2 \
    vtkogg-8.2 \
    vtkIOPLY-8.2 \
    vtkIOParallelXML-8.2 \
    vtkParallelCore-8.2 \
    vtkIOLegacy-8.2 \
    vtkTestingIOSQL-8.2 \
    vtkIOSQL-8.2 \
    vtksqlite-8.2 \
    vtkIOSegY-8.2 \
    vtkIOTecplotTable-8.2 \
    vtkIOVeraOut-8.2 \
    vtkhdf5_hl-8.2 \
    vtkhdf5-8.2 \
    vtkIOVideo-8.2 \
    vtkImagingStatistics-8.2 \
    vtkRenderingImage-8.2 \
    vtkImagingMorphological-8.2 \
    vtkRenderingLOD-8.2 \
    vtkRenderingQt-8.2 \
    vtkFiltersTexture-8.2 \
    vtkViewsQt-8.2 \
    vtkGUISupportQt-8.2 \
    vtkRenderingOpenGL2-8.2 \
    vtkglew-8.2 \
    vtkViewsInfovis-8.2 \
    vtkChartsCore-8.2 \
    vtkRenderingContext2D-8.2 \
    vtkViewsCore-8.2 \
    vtkInteractionWidgets-8.2 \
    vtkFiltersHybrid-8.2 \
    vtkInteractionStyle-8.2 \
    vtkRenderingAnnotation-8.2 \
    vtkImagingColor-8.2 \
    vtkRenderingVolume-8.2 \
    vtkIOXML-8.2 \
    vtkIOXMLParser-8.2 \
    vtkIOCore-8.2 \
    vtkdoubleconversion-8.2 \
    vtklz4-8.2 \
    vtklzma-8.2 \
    vtkexpat-8.2 \
    vtkFiltersImaging-8.2 \
    vtkImagingGeneral-8.2 \
    vtkImagingSources-8.2 \
    vtkRenderingLabel-8.2 \
    vtkRenderingFreeType-8.2 \
    vtkRenderingCore-8.2 \
    vtkCommonColor-8.2 \
    vtkFiltersGeometry-8.2 \
    vtkfreetype-8.2 \
    vtkInfovisLayout-8.2 \
    vtkInfovisCore-8.2 \
    vtkFiltersExtraction-8.2 \
    vtkFiltersStatistics-8.2 \
    vtkImagingFourier-8.2 \
    vtkFiltersModeling-8.2 \
    vtkFiltersSources-8.2 \
    vtkFiltersGeneral-8.2 \
    vtkCommonComputationalGeometry-8.2 \
    vtkFiltersCore-8.2 \
    vtkImagingHybrid-8.2 \
    vtkImagingCore-8.2 \
    vtkIOImage-8.2 \
    vtkCommonExecutionModel-8.2 \
    vtkCommonDataModel-8.2 \
    vtkCommonMisc-8.2 \
    vtkCommonSystem-8.2 \
    vtkCommonTransforms-8.2 \
    vtkCommonMath-8.2 \
    vtkCommonCore-8.2 \
    vtksys-8.2 \
    vtkDICOMParser-8.2 \
    vtkmetaio-8.2 \
    vtkpng-8.2 \
    vtktiff-8.2 \
    vtkzlib-8.2 \
    vtkjpeg-8.2

for (lib, VTK_Libs) {
    message("add library: $$lib")
    LIBS += -L$$VTK_DIR/lib/vtk-8.2/ -l$$lib
    win32:!win32-g++: PRE_TARGETDEPS += $$VTK_DIR/lib/vtk-8.2/$${lib}.lib
    else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$VTK_DIR/lib/vtk-8.2/lib$${lib}.a
}

win32: LIBS += -luser32 -lgdi32 -ladvapi32 -lglu32 -lopengl32 -ldbghelp
else:unix: LIBS += -ldl -lpthread -lm -lGL -lGLU -lXt -lSM -lICE -lX11 -lXext

INCLUDEPATH += $$VTK_DIR/include/vtk-8.2
DEPENDPATH += $$VTK_DIR/include/vtk-8.2
