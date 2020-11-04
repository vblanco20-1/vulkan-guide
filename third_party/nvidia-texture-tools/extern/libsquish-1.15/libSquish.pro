TARGET = squish
TEMPLATE = lib

include(libSquish.pri)

QT -= gui

CONFIG += staticlib thread
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
   unix:TARGET = $$join(TARGET,,,_debug)
}

MOC_DIR = mocs
OBJECTS_DIR = objs
RCC_DIR = rccs
UI_DIR = uics

CONFIG(debug, debug|release) {
   unix:MOC_DIR = $$join(MOC_DIR,,,_debug)
   unix:OBJECTS_DIR = $$join(OBJECTS_DIR,,,_debug)
   unix:RCC_DIR = $$join(RCC_DIR,,,_debug)
   unix:UI_DIR = $$join(UI_DIR,,,_debug)
   win32:MOC_DIR = $$join(MOC_DIR,,,d)
   win32:OBJECTS_DIR = $$join(OBJECTS_DIR,,,d)
   win32:RCC_DIR = $$join(RCC_DIR,,,d)
   win32:UI_DIR = $$join(UI_DIR,,,d)
}

unix:QMAKE_CXXFLAGS += -DSQUISH_USE_OPENMP -fopenmp

