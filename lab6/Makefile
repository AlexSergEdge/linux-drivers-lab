# Constants definition
CC     = gcc
FLAGS = -g -Wall -Wstrict-prototypes -ansi -pedantic

ifneq (${KERNELRELEASE},)
	obj-m := hello.o
else
	KERNEL_SOURCE := /usr/src/linux-headers-3.19.0-25-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif