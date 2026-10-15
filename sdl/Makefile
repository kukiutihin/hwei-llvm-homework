CC = clang 
CXX = clang++

### sdl 

all: app

start.ll: start.c
	CC -emit-llvm -S $^

sim.ll: sim.c
	CC -emit-llvm -S $^

app.ll: app.c
	CC -emit-llvm -S $^

app-opt.ll: app.c
	CC -emit-llvm -S $^ -o $@ -O2

app-opt: app-opt.ll start.c sim.c
	CC $^ -lSDL2 -o $@ -O2

app: app.ll start.c sim.c
	CC $^ -lSDL2  -o $@

app-debug: app.ll start.c sim.c
	CC $^ -lSDL2 -g -o $@

app-opt-debug.ll: app.c
	CC -emit-llvm -S $^ -o $@ -O2 -g

### pass

DYNAMIC_FLAGS = -fPIC -shared -I$$(llvm-config --includedir) -std=c++23 -O2
DIR = ../pass
AFTER_ALL_FILE = after-all.txt

LOG_FILE = log.txt
STAT_LIMIT = 50000000
STAT_WINDOWS = 1 2 3 4 5
STAT_FILE = statistics.txt

ifeq ($(shell uname), Darwin)
        DYNAMIC_FLAGS += -undefined dynamic_lookup
endif

libSkib.so: $(DIR)/pass.cpp
	$(CXX) $^ -o $@ $(DYNAMIC_FLAGS)

app-log.o: app.c libSkib.so  
	$(CC) $< -fpass-plugin=libSkib.so -c -o $@

app-log: app-log.o start.c sim.c $(DIR)/log.c 	
	$(CC) $^ -lSDL2 -O2 -o $@

statistics: $(DIR)/statistics.cpp $(LOG_FILE)
	$(CXX) -std=c++23 $< -o $@
	tail -n $(STAT_LIMIT) log.txt > temp.txt 
	./statistics temp.txt $(STAT_WINDOWS) &> $(STAT_FILE)
	rm -f temp.txt statistics
	
clean-stat:
	@rm -f temp.txt statistics.txt after-all.txt

# test-log.ll: test.c 
# 	$(CC) -emit-llvm -S $^ -o $@ -O2
#
# test-log.o: test.c libSkib.so
# 	$(CC) $< -fpass-plugin=libSkib.so -c -o $@
#
# test-log: test-log.o $(DIR)/log.c
# 	$(CC) $^ -O2 -o $@
#
# test-after-all: test.c 
# 	$(CC) $< -c -o $@ -mllvm -print-after-all &> $(AFTER_ALL_FILE)
# 	@rm test-after-all

###

clean:
	@rm -f *.out *.o *.ll 
	@rm -f app app-opt
	@rm -f full.bc ir_gen asm2ir asm2fullir
	@rm -f libSkib.so test-log app-log 
.PHONY: all clean
