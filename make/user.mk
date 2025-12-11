.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h \
  include/version.h include/ready.h \
  include/mcb/allocate.h include/mcb/free.h include/mcb/show.h

USER_OBJECTS=\
	user/core.o\
	user/exit.o\
	user/help.o\
	user/version.o\
	user/clock.o\
	user/pcb/setPriority.o\
	user/pcb/showPCB.o\
	user/pcb/ready.o\
	user/pcb/init.o\
	user/pcb/block.o\
	user/yield.o\
	user/loadR3.o\
	user/alarm.o\
	user/mcb/allocate.o\
	user/mcb/free.o\
	user/mcb/show.o
  