QMAKE_TARGET  = AnnualLeaveCommon
PROJECT_DIR	  := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
QMAKE = $(QNX_HOST)/usr/bin/qmake
 
ifdef NUMBER_OF_PROCESSORS
  MAKEFLAGS += -j${NUMBER_OF_PROCESSORS}
endif

QMAKEFEATURES = $(dir $(PWD)/$(lastword $(MAKEFILE_LIST)))
export QMAKEFEATURES
 
default: Device-Release	
 
.PHONY: FORCE
 
Makefile-all: arm/Makefile arm-p/Makefile x86/Makefile translations/Makefile	
 
arm/Makefile: $(QMAKE_TARGET).pro
	@mkdir -p arm
	cd arm && $(QMAKE) -spec blackberry-armv7le-qcc ../$(QMAKE_TARGET).pro CONFIG+=debug_and_release CONFIG+=device
 
arm-p/Makefile: $(QMAKE_TARGET).pro
	@mkdir -p arm-p
	cd arm-p && $(QMAKE) -spec blackberry-armv7le-qcc ../$(QMAKE_TARGET).pro CONFIG+=debug_and_release CONFIG+=device CONFIG+=profile
 
x86/Makefile: $(QMAKE_TARGET).pro
	@mkdir -p x86
	cd x86 && $(QMAKE) -spec blackberry-x86-qcc ../$(QMAKE_TARGET).pro  CONFIG+=debug_and_release CONFIG+=simulator
 
Device-Release: arm/Makefile FORCE
	$(MAKE) -C ./arm -f Makefile release
 
Device-Debug: arm/Makefile FORCE	
	$(MAKE) -C ./arm -f Makefile debug
 
Device-Profile: arm-p/Makefile FORCE
	$(MAKE) -C ./arm-p -f Makefile debug
 
Simulator-Debug: x86/Makefile FORCE
	$(MAKE) -C ./x86 -f Makefile debug

clean: FORCE
	rm -rf arm arm-p x86
	rm -f $(I18N_DIR)/*.qm

