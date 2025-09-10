.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h include/version.h

USER_OBJECTS=\
	user/core.o\
	user/exit.o\
	user/help.o\
	user/version.o\
	user/clock.o
  