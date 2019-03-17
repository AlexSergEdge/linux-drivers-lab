# пример Makefile драйвера
# если объявлена переменная среды окружения KERNELRELEASE
# запуски из среды компиляции ядра, которой надо указать компилируемый файл.
ifneq (${KERNELRELEASE},)
	obj-m := hello.o
# Иначе вызов из командной строки.
# Тогда вызываем среду компиляции ядра.
else
	KERNEL_SOURCE := /usr/src/linux-headers-3.19.0-25-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
endif