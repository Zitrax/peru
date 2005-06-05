# Set custom paths if programs are installed
# in unusual places. Else leave empty

# Next line sets path to opencv
CVPATH = /usr/local

# Path to directory over directory magick
MAGICKPATH =

SOURCES	+= Peru.cpp                              \
           main.cpp                              \
           ImageWidget.cpp                       \
           MontageView.cpp                       \
           ../src/CCOCV.cpp                      \
           kstartuplogo.cpp                      \
           TopHatSettings.cpp                    \
           CalibrationParameters.cpp             \
           QLed.cpp                              \
           ../src/stereo/stereo.cpp              \
#          ../src/SortCorners.cpp                \
           ../src/stereo/BlockMatch.cpp          \
           ../src/stereo/CvBirchfield.cpp        \
           ../src/stereo/PyramidBlock.cpp        \
           ../src/Filter.cpp                     \
           ../src/MedianFilter.cpp               \
           ../src/Gaussian.cpp                   \
           ../src/MeanCorrection.cpp             \
           ../src/HistoEq.cpp                    \
           ../src/GradientRemoval.cpp            \
           ../src/ContrastStretch.cpp            \
           ../src/PerspectiveCorrect.cpp         \
           ../src/ccv.cpp                        \
           ../src/preferences.cpp

HEADERS	+= Peru.h                                \
           ../src/CCOCV.h                        \ 
           ImageWidget.h                         \
           MontageView.h                         \
           kstartuplogo.h                        \
           TopHatSettings.h                      \
           CalibrationParameters.h               \
           QLed.h                                \
           ../src/stereo/stereo.h                \
 #         ../src/SortCorners.h                  \
           ../src/stereo/BlockMatch.h            \
           ../src/stereo/CvBirchfield.h          \
           ../src/stereo/PyramidBlock.h          \
           ../src/Filter.h                       \
           ../src/MedianFilter.h                 \
           ../src/Gaussian.h                     \
           ../src/MeanCorrection.h               \
           ../src/HistoEq.h                      \
           ../src/GradientRemoval.h              \
           ../src/ContrastStretch.h              \
           ../src/PerspectiveCorrect.h           \
           ../src/ccv.h                          \
           ../src/preferences.h

count( CVPATH, 1 ) {
INCLUDEPATH += $$CVPATH/include                  \
               $$CVPATH/include/opencv           
}
count( CVPATH, 0 ) {
INCLUDEPATH += /usr/local/include/opencv           
}


INCLUDEPATH += ../src ../src/stereo

# GL things that are not yet used
# SOURCES += ImageWidgetGL.cpp GLWindow.cpp
# HEADERS += ImageWidgetGL.h GLWindow.h
# LIBS += -lGL -lGLU 

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

FORMS	= Perubase.ui \
          TopHatSettingsBase.ui \
          CalibrationParametersBase.ui

TRANSLATIONS = peru_se.ts

IMAGES	= images/filenew    \
          images/fileopen   \
          images/filesave   \
          images/print      \
          images/undo       \
          images/redo       \
          images/editcut    \
          images/editcopy   \
          images/editpaste  \
          images/searchfind 

TEMPLATE = app

CONFIG	+= qt warn_off release #debug

DBFILE	 = Peru.db

LANGUAGE = C++

count( CVPATH, 1 ) {
LIBS += -L$$CVPATH/lib -Wl,-rpath,$$CVPATH/lib 
}
LIBS += -lcv  \
        -lcxcore \
        -lhighgui \
        -lavcodec \
        -lcvaux   \


# For ImageMagick
# LIBS += `Magick++-config --ldflags --libs` 
# INCLUDEPATH += $$MAGICKPATH/magick    \
#               $$MAGICKPATH           \
#               /usr/X11R6/include     \
#               /usr/include/freetype2 \
#               /usr/include/libxml2
# DEFINES += _REENTRANT _FILE_OFFSET_BITS=64

