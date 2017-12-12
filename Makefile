# File: makefile
#
# Description: Command line makefile.
#
# Syntax:      make                  		- make all
#
#              make v7_flash        		- make v7_flash.c
#              make 717xusage        		- make 717xusage.c
#              make nbddcacq			- make nbddcacq.c
#	       make ddc_multichan               - make ddc_multichan.c
#	       make ddc_adaptive_relay          - make ddc_adaptive_relay.c
#	       make adaptive_relay_dmaio        - make adaptive_relay_dmaio.c
#              make ddcacq			- make ddcacq.c
#              make adcacq           		- make adcacq.c
#              make adc_circlrdmabuf 		- make_adc_circlrdmabuf.c
#              make adc_multichan    		- make_adc_multichan.c
#              make adc_trig         		- make adc_trig.c
#              make cfgcdc           		- make cfgcdc.c
#              make cfg7192                     - make cfg7192.c                       		
#              make dac_circlrDmaBuf 		- make dac_circlrDmaBuf.c
#              make dac_clksync      		- make dac_clksync.c
#              make dacmode          		- make dacmode.c
#              make dac_multiwave    		- make dac_multiwave.c
#              make dac_trig         		- make dac_trig.c
#              make ducmode          		- make ducmode.c
#              make interp          		- make interp.c
#              make beamformer          	- make beamformer.c
#              make flashload        		- make flashload.c
#              make regdump          		- make regdump.c
#              make sensor           		- make sensor.c
#              make show_info        		- make show_info.c
#
#
# tools
ASM    = as
CC     = gcc
AR     = ar
LNK    = ld
RM     = rm


# Setup arch specific options
WD_ARCH         =$(shell uname -m)
        ifeq    ($(WD_ARCH),i686)
 
		LIB_DIR             := $(READYFLOW_71620_LINUX)/x86/lib

                PTK_READYFLOW_CFLAGS:= -I $(READYFLOW_71620_LINUX)/x86/include
                PTK_READYFLOW_CFLAGS+= -L $(LIB_DIR)
		PTK_READYFLOW_CFLAGS+= -I $(READYFLOW_71620_LINUX)/include  
                PTK_READYFLOW_CFLAGS+= -DP716x_LNX32_LE

		numeral := $(shell echo $(WD_BASEDIR) | awk -F'/' '{print $$NF}' |  sed -r 's/.*([0-9]{4}).*/\1/')
        endif

        ifeq    ($(WD_ARCH),ppc)
 
		LIB_DIR             := $(READYFLOW_71620_LINUX)/ppc/lib

                PTK_READYFLOW_CFLAGS:= -I $(READYFLOW_71620_LINUX)/ppc/include
                PTK_READYFLOW_CFLAGS+= -L $(LIB_DIR)
		PTK_READYFLOW_CFLAGS+= -I $(READYFLOW_71620_LINUX)/include  
                PTK_READYFLOW_CFLAGS+= -DP716x_LNX32_BE
        endif

        ifeq    ($(WD_ARCH),x86_64)
 
		LIB_DIR             := $(READYFLOW_71620_LINUX)/x86_64/lib

                PTK_READYFLOW_CFLAGS:= -I $(READYFLOW_71620_LINUX)/x86_64/include
                PTK_READYFLOW_CFLAGS+= -L $(LIB_DIR)
		PTK_READYFLOW_CFLAGS+= -I $(READYFLOW_71620_LINUX)/include  
                PTK_READYFLOW_CFLAGS+= -DP716x_LNX64_LE

		numeral := $(shell echo $(WD_BASEDIR) | awk -F'/' '{print $$NF}' |  sed -r 's/.+([0-9]{4}).+/\1/')
        endif

# Libraries
LIB           = 716xrf.lib

# options
CFLAGS =-O0 -g -w -DLINUX -DP71620 $(PTK_READYFLOW_CFLAGS) -Wall -fPIC -DRW_MULTI_THREAD -DREG_WIDTH_64BIT -D_REENTRANT -o $@.out -lm -lrt -L $(WD_BASEDIR)/kplugin -lptk716x -lpthread -lwdapi$(numeral)
CFLAGS717X =-O0 -g -w -DLINUX -DP71620 $(PTK_READYFLOW_CFLAGS) -Wall -fPIC -DRW_MULTI_THREAD -DREG_WIDTH_64BIT -D_REENTRANT -o $@.out -lm -lrt -L $(WD_BASEDIR)/kplugin -lptk717x -lpthread -lwdapi$(numeral)

# Suffixes
.SUFFIXES: .o .c .asm .out

# File List 
all:
	$(MAKE) v7_flash
	$(MAKE) 717xusage
	$(MAKE) nbddcacq
	$(MAKE) ddc_multichan
	$(MAKE) ddc_adaptive_relay
	$(MAKE) adaptive_relay_dmaio
	$(MAKE) ddcacq
	$(MAKE) adcacq
	$(MAKE) adc_circlrdmabuf
	$(MAKE) adc_multichan
	$(MAKE) adc_trig    
	$(MAKE) cfg7192
	$(MAKE) cfgcdc    
	$(MAKE) dac_circlrDmaBuf
	$(MAKE) dac_clksync
	$(MAKE) dacmode
	$(MAKE) dac_multiwave 
	$(MAKE) dac_trig 
	$(MAKE) ducmode
	$(MAKE) interp
	$(MAKE) beamformer
	$(MAKE) flashload
	$(MAKE) regdump    
	$(MAKE) sensor    
	$(MAKE) show_info    
	
v7_flash:
	$(CC) v7_flash.c $(LIB_DIR)/$(LIB) $(CFLAGS717X) 

717xusage:
	$(CC) 717xusage.c $(LIB_DIR)/$(LIB) $(CFLAGS717X) 

nbddcacq:
	$(CC) nbddcacq.c $(LIB_DIR)/$(LIB) $(CFLAGS)

ddc_multichan:
	$(CC) ddc_multichan.c $(LIB_DIR)/$(LIB) $(CFLAGS)

ddc_adaptive_relay:
	$(CC) ddc_adaptive_relay.c $(LIB_DIR)/$(LIB) $(CFLAGS)

adaptive_relay_dmaio:
	$(CC) adaptive_relay_dmaio.c $(LIB_DIR)/$(LIB) $(CFLAGS)

ddcacq:
	$(CC) ddcacq.c $(LIB_DIR)/$(LIB) $(CFLAGS)

adcacq:
	$(CC) adcacq.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

adc_circlrdmabuf:
	$(CC) adc_circlrdmabuf.c $(LIB_DIR)/$(LIB) $(CFLAGS)

adc_multichan:
	$(CC) adc_multichan.c $(LIB_DIR)/$(LIB) $(CFLAGS)

adc_trig:
	$(CC) adc_trig.c $(LIB_DIR)/$(LIB) $(CFLAGS)

cfgcdc:
	$(CC) cfgcdc.c $(LIB_DIR)/$(LIB) $(CFLAGS)

cfg7192:
	$(CC) cfg7192.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

dac_circlrDmaBuf:
	$(CC) dac_circlrDmaBuf.c $(LIB_DIR)/$(LIB) $(CFLAGS)

dac_clksync:
	$(CC) dac_clksync.c $(LIB_DIR)/$(LIB) $(CFLAGS)

dac_multiwave:
	$(CC) dac_multiwave.c $(LIB_DIR)/$(LIB) $(CFLAGS)

dac_trig:
	$(CC) dac_trig.c $(LIB_DIR)/$(LIB) $(CFLAGS)

dacmode:
	$(CC) dacmode.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

ducmode:
	$(CC) ducmode.c $(LIB_DIR)/$(LIB) $(CFLAGS)

interp:
	$(CC) interp.c $(LIB_DIR)/$(LIB) $(CFLAGS)

beamformer:
	$(CC) beamformer.c $(LIB_DIR)/$(LIB) $(CFLAGS)

flashload:
	$(CC) flashload.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

regdump:
	$(CC) regdump.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

sensor:
	$(CC) sensor.c $(LIB_DIR)/$(LIB) $(CFLAGS)

show_info:
	$(CC) show_info.c $(LIB_DIR)/$(LIB) $(CFLAGS) 

clean:
	rm *.out

