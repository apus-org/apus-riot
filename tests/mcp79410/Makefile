APPLICATION = tests_mcp79410
BOARD ?= hsd-apus
RIOTBASE ?= $(CURDIR)/../../../RIOT

EXTERNAL_BOARD_DIRS ?= $(CURDIR)/../../boards
EXTERNAL_MODULE_DIRS += $(CURDIR)/../../modules

USEMODULE += mcp79410
USEMODULE += ztimer_msec
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
