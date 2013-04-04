TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ../tracking/Tracker.cpp \
    ../tracking/track.cpp \
    ../tracking/PatternDetector.cpp \
    ../tracking/Pattern.cpp \
    ../tracking/Helper.cpp \
    ../tst/tst-track.cpp

OTHER_FILES += \
    ../tracking/Makefile

HEADERS += \
    ../tracking/Tracker.hpp \
    ../tracking/PatternDetector.hpp \
    ../tracking/Pattern.hpp \
    ../tracking/Helper.hpp

