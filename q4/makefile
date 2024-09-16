DEBUG ?= 1
COVERAGE ?= 0

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

# Source files
SRCS = main.cpp Kosaraju.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

TARGET = kosaraju
DEPS = Kosaraju.hpp

# Directories
OUTPUTS_FOLDER = outputs
INPUTS_FOLDER = inputs
EXPECTED_FOLDER = expected
GCOV_OUTPUTS = gcov_outputs
GPROF_OUTPUTS = gprof_outputs

all: $(TARGET)

.PHONY: all clean test coverage tidy valgrind clean_gcov

# Disable debug mode to ignore debug prints
test:
	make clean
	make all DEBUG=0
	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt > $(OUTPUTS_FOLDER)/output1.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input2.txt > $(OUTPUTS_FOLDER)/output2.txt || true
	diff $(OUTPUTS_FOLDER)/output1.txt $(EXPECTED_FOLDER)/expected1.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"
	diff $(OUTPUTS_FOLDER)/output2.txt $(EXPECTED_FOLDER)/expected2.txt > /dev/null && echo "Output files are equal" || echo "Output files are not equal"

coverage:
	make clean
	make all COVERAGE=1 DEBUG=0
	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/input2.txt || true
	./$(TARGET) < $(INPUTS_FOLDER)/invalid1.txt || true
	gcov Kosaraju.cpp main.cpp > $(GCOV_OUTPUTS)/stdout.txt
	mv *.gcov $(GCOV_OUTPUTS) || true
	mv *.gcda $(GCOV_OUTPUTS) || true
	mv *.gcno $(GCOV_OUTPUTS) || true

tidy:
	clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --
	# clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

valgrind: $(TARGET)
	valgrind --tool=memcheck $(VALGRIND_FLAGS) ./$(TARGET) 2>&1 | { egrep "lost| at " || true; }

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c main.cpp

Kosaraju.o: Kosaraju.cpp Kosaraju.hpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

clean_gcov:
	rm -f $(GCOV_OUTPUTS)/*.gcov $(GCOV_OUTPUTS)/*.gcda $(GCOV_OUTPUTS)/*.gcno $(GCOV_OUTPUTS)/stdout.txt $(GCOV_OUTPUTS)/*.gcda
