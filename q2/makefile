DEBUG ?= 1
COVERAGE ?= 0
PROFILING ?= 0

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
GCOV = -fprofile-arcs -ftest-coverage
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all  --error-exitcode=99

ifeq ($(DEBUG), 1)
    CXXFLAGS += -DDEBUG
endif

ifeq ($(COVERAGE), 1)
    CXXFLAGS += $(GCOV)
endif

ifeq ($(PROFILING), 1)
	CXXFLAGS += -pg
endif

# Source files
SRCS = main.cpp kosaraju_list.cpp kosaraju_deque.cpp kosaraju_adjmatrix.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

TARGET = kosaraju_main
DEPS = kosaraju_list.hpp kosaraju_deque.hpp kosaraju_adjmatrix.hpp

# Directories
OUTPUTS_FOLDER = outputs
INPUTS_FOLDER = inputs
EXPECTED_FOLDER = expected
GCOV_OUTPUTS = gcov_outputs
GPROF_OUTPUTS = gprof_outputs

all: $(TARGET)

.PHONY: all clean test test1

# Disable debug mode to ignore debug prints
test:
	make clean
	make all DEBUG=0
	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt > $(OUTPUTS_FOLDER)/output1.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input2.txt > $(OUTPUTS_FOLDER)/output2.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input3.txt > $(OUTPUTS_FOLDER)/output3.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input4.txt > $(OUTPUTS_FOLDER)/output4.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput1.txt > $(OUTPUTS_FOLDER)/output5.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput2.txt > $(OUTPUTS_FOLDER)/output6.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput3.txt > $(OUTPUTS_FOLDER)/output7.txt || true
	diff $(OUTPUTS_FOLDER)/output1.txt $(EXPECTED_FOLDER)/expected1.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output2.txt $(EXPECTED_FOLDER)/expected2.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output3.txt $(EXPECTED_FOLDER)/expected3.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output4.txt $(EXPECTED_FOLDER)/expected4.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output5.txt $(EXPECTED_FOLDER)/expected5.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output6.txt $(EXPECTED_FOLDER)/expected6.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output7.txt $(EXPECTED_FOLDER)/expected7.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"

coverage:
	make clean
	make all COVERAGE=1 DEBUG=0
	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input2.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input3.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input4.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput1.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput2.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalidinput3.txt || true
	gcov kosaraju_list.cpp kosaraju_deque.cpp kosaraju_adjmatrix.cpp > $(GCOV_OUTPUTS)/stdout.txt
	mv *.gcov $(GCOV_OUTPUTS) || true
	mv *.gcda $(GCOV_OUTPUTS) || true
	mv *.gcno $(GCOV_OUTPUTS) || true

# use -pg flag for gprof
profile:
	make clean
	make all PROFILING=1
	python3 inputs/generate_gprof_inputs.py
	./$(TARGET) < $(INPUTS_FOLDER)/input_large_deque.txt > /dev/null || true
	mv gmon.out $(GPROF_OUTPUTS)/gmon_deque.out
	./$(TARGET) < $(INPUTS_FOLDER)/input_large_adjmatrix.txt > /dev/null || true
	mv gmon.out $(GPROF_OUTPUTS)/gmon_adjmatrix.out
	./$(TARGET) < $(INPUTS_FOLDER)/input_large_list.txt > /dev/null || true
	mv gmon.out $(GPROF_OUTPUTS)/gmon_list.out
	gprof $(TARGET) $(GPROF_OUTPUTS)/gmon_deque.out > $(GPROF_OUTPUTS)/gprof_deque.txt
	gprof $(TARGET) $(GPROF_OUTPUTS)/gmon_adjmatrix.out > $(GPROF_OUTPUTS)/gprof_adjmatrix.txt
	gprof $(TARGET) $(GPROF_OUTPUTS)/gmon_list.out > $(GPROF_OUTPUTS)/gprof_list.txt

tidy:
	clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --
	# clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

valgrind: $(TARGET)
	valgrind --tool=memcheck $(VALGRIND_FLAGS) ./$(TARGET) 2>&1 | { egrep "lost| at " || true; }

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c main.cpp

kosaraju_list.o: kosaraju_list.cpp kosaraju_list.hpp
	$(CXX) $(CXXFLAGS) -c $<

kosaraju_deque.o: kosaraju_deque.cpp kosaraju_deque.hpp
	$(CXX) $(CXXFLAGS) -c $<

kosaraju_adjmatrix.o: kosaraju_adjmatrix.cpp kosaraju_adjmatrix.hpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

clean_gcov:
	rm -f $(GCOV_OUTPUTS)/*.gcov $(GCOV_OUTPUTS)/*.gcda $(GCOV_OUTPUTS)/*.gcno $(GCOV_OUTPUTS)/stdout.txt $(GCOV_OUTPUTS)/*.gcda
