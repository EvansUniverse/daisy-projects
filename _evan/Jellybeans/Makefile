# Project Name
TARGET = Jellybeans

# Sources
CPP_SOURCES = Jellybeans.cpp \
../lib/ev_theory/theory.cpp \
../lib/ev_theory/arp.cpp \
../lib/ev_theory/chord.cpp \
../lib/ev_theory/tempo.cpp \
../lib/ev_gui/menuItem.cpp \
../lib/ev_gui/menuItemInt.cpp \
../lib/ev_gui/menu.cpp \
../lib/ev_gui/patchGui.cpp

# Electrosmith library Locations
LIBDAISY_DIR = ../../libdaisy
DAISYSP_DIR = ../../DaisySP

# Compiler flag to optimize for binary size. Reduces bin size by ~10%
OPT = -Os

# Core location and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile	