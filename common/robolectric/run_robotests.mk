my_path := $(dir $(lastword $(MAKEFILE_LIST)))

include $(my_path)/3.1.1/run_robotests.mk
