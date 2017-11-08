my_path := $(dir $(lastword $(MAKEFILE_LIST)))

include $(my_path)/3.4.2/run_robotests.mk
