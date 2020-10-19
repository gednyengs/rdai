APP		= test

APP_OBJS	= rdai-cma.c app.c

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f  $(APP) *.o
