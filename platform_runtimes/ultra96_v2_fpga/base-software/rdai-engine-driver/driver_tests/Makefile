APP_DMA		= test_dma
APP_CMA		= test_cma

APP_DMA_OBJS	= rdai-cma.c test-dma.c
APP_CMA_OBJS	= rdai-cma.c test-cma.c

all: build

build: $(APP_DMA) $(APP_CMA)

$(APP_DMA): $(APP_DMA_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(APP_CMA): $(APP_CMA_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f  $(APP_DMA) $(APP_CMA) *.o
