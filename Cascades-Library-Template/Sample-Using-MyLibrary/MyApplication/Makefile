QMAKE_TARGET  = MyApplication
PROJECT_DIR	  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
I18N_DIR	  := $(PROJECT_DIR)/translations

QMAKEFEATURES = $(CURDIR)/../../MyLibrary
export QMAKEFEATURES

include mk/cs-base.mk

