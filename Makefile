CC = clang 
CXX = clang++

SDL = $(shell sdl2-config --cflags --libs)

LLVM_INCLUDE = $(shell llvm-config --includedir)
LLVM_LIBS    = $(shell llvm-config --ldflags --libs)

CFLAGS        = -O2
CXXFLAGS      = -std=c++23 -O2
DYNAMIC_FLAGS = -fPIC -shared -I$(LLVM_INCLUDE) $(CXXFLAGS)

ifeq ($(shell uname), Darwin)
        DYNAMIC_FLAGS += -undefined dynamic_lookup
endif

### SDL2 app

APP_DIR = application

all: app

start.ll: $(APP_DIR)/start.c
	$(CC) -emit-llvm -S $^

sim.ll: $(APP_DIR)/sim.c
	$(CC) -emit-llvm -S $^

app.ll: $(APP_DIR)/app.c
	$(CC) -emit-llvm -S $^ -o $@ -O2

app: app.ll $(APP_DIR)/start.c $(APP_DIR)/sim.c
	$(CC) $^ -o $@ $(CFLAGS) $(SDL)

clean-app:
	@rm -f start.ll sim.ll app.ll app-log.ll
	@rm -f app-opt app app-debug

### logger

LOG_DIR = logger

LOG_FILE = log.txt
STAT_LIMIT = 50000000
STAT_WINDOWS = 2 3 4 5 6 7 8 9 10 11

libSkib.so: $(LOG_DIR)/pass.cpp
	$(CXX) $^ -o $@ $(DYNAMIC_FLAGS) -Wno-deprecated-declarations

app-log.o: $(APP_DIR)/app.c libSkib.so  
	$(CC) $< -o $@ -fpass-plugin=libSkib.so -c $(CFLAGS) 
app-log: app-log.o $(APP_DIR)/start.c $(APP_DIR)/sim.c $(LOG_DIR)/log.c 	
	$(CC) $^ -o $@ $(CFLAGS) $(SDL) 

statistics: $(LOG_DIR)/statistics.cpp $(LOG_FILE)
	$(CXX) $< -o $@ -std=c++23
	tail -n $(STAT_LIMIT) log.txt > temp.txt 
	./statistics temp.txt $(STAT_WINDOWS) &> statistics.txt
	rm -f temp.txt statistics
	
clean-stat:
	@rm -f app-log.o libSkib.so app-log statistics 
	@rm -f temp.txt statistics.txt after-all.txt

### generate ir generator 

GEN_DIR = ir_gen

skibGen.so: $(GEN_DIR)/pass.cpp
	$(CXX) $^ -o $@ $(DYNAMIC_FLAGS)

app-gen.o: $(APP_DIR)/app.c skibGen.so  
	$(CC) $< -o $@ -fpass-plugin=skibGen.so -c $(CFLAGS)

gen-gen:
	make -s app-gen.o &> $(GEN_DIR)/ir-gen.cpp
	clang-format -i -style=LLVM $(GEN_DIR)/ir-gen.cpp

gen-ir: $(GEN_DIR)/ir-gen.cpp $(APP_DIR)/sim.c
	$(CXX) $^ -o $@ $(FLAGS) $(SDL) $(LLVM)
 
clean-gen:
	@rm -f $(GEN_DIR)/ir-gen.cpp
	@rm -f app-gen.o skibGen.so
	@rm -f gen-ir generated.ll
	@rm -rf gen-ir.dSYM
	
### asm 

ASM_GEN_DIR = asm
IR_TO_ASM = $(ASM_GEN_DIR)/ir-to-asm
ASM_TO_IR = $(ASM_GEN_DIR)/asm-to-ir

skibAsmGen.so: $(IR_TO_ASM)/pass.cpp
	$(CXX) $^ -o $@ $(DYNAMIC_FLAGS)

app-asmgen.o: $(APP_DIR)/app.c skibAsmGen.so  
	$(CC) $< -o $@ -fpass-plugin=skibAsmGen.so -c $(CFLAGS) 

asmgen:
	make -s app-asmgen.o > skibasm.s

clean-asmgen:
	@rm -f asmgen skibasm.s skibAsmGen.so app-asmgen.o 

###

clean: clean-app clean-stat clean-gen clean-asmgen

.PHONY: all clean
