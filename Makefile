SRCS = \
	dispatch_after.c \
	dispatch_api.c \
	dispatch_apply.c \
	dispatch_c99.c \
	dispatch_cascade.c \
	dispatch_cf_main.c \
	dispatch_concur.c \
	dispatch_context_for_key.c \
	dispatch_data.c \
	dispatch_deadname.c \
	dispatch_debug.c \
	dispatch_drift.c \
	dispatch_group.c \
	dispatch_io.c \
	dispatch_io_net.c \
	dispatch_overcommit.c \
	dispatch_pingpong.c \
	dispatch_priority.c \
	dispatch_proc.c \
	dispatch_queue_finalizer.c \
	dispatch_read.c \
	dispatch_read2.c \
	dispatch_readsync.c \
	dispatch_select.c \
	dispatch_sema.c \
	dispatch_starfish.c \
	dispatch_suspend_timer.c \
	dispatch_sync_on_main.c \
	dispatch_test.c \
	dispatch_timer.c \
	dispatch_timer_bit31.c \
	dispatch_timer_bit63.c \
	dispatch_timer_set_time.c \
	dispatch_timer_short.c \
	dispatch_timer_timeout.c \
	dispatch_transform.c \
	dispatch_vm.c \
	dispatch_vnode.c \

OBJS = $(subst .c,.o,$(SRCS))

CFLAGS = -I./dispatch
LIBS = -framework CoreFoundation
TARGET = $(subst .c,,$(SRCS))

.SUFFIXES: .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	${CC} -o $@ $(OBJS) $(LIBS)

.c.o :
	${CC} -c $(CFLAGS) -I. $< -o $@

clean :
	rm -f *.o $(TARGET)

debug:
	@echo "SRCS: ${SRCS}"
	@echo "OBJS: ${OBJS}"
	@echo "TARGET: ${TARGET}"

.PHONY: clean debug
