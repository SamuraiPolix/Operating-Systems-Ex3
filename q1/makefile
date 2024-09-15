DEBUG ?= 1

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
GCOV = -fprofile-arcs -ftest-coverage
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all  --error-exitcode=99

ifeq ($(DEBUG), 1)
    CXXFLAGS += -DDEBUG
endif

# Source files
SRCS = main.cpp Kosaraju.cpp

OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = kosaraju

OUTPUTS_FOLDER = outputs
INPUTS_FOLDER = inputs
EXPECTED_FOLDER = expected
GCOV_OUTPUTS = gcov_outputs

all: $(TARGET)

# Disable debug mode to ignore debug prints
test:
	make clean 
	make test1 DEBUG=0

test1: $(TARGET)
	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt > $(OUTPUTS_FOLDER)/output1.txt
	diff $(OUTPUTS_FOLDER)/output1.txt $(EXPECTED_FOLDER)/expected1.txt || echo "Output files are not equal"

.PHONY: all clean coverage tidy valgrind clean_gcov_folder test

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule for compiling source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

coverage: 
	$(CXX) $(GCOV) -o $(TARGET) $(SRCS)

	./$(TARGET) < $(INPUTS_FOLDER)/input1.txt > $(OUTPUTS_FOLDER)/output1.txt || true
	gcov kosaraju-main.cpp kosaraju-Kosaraju.cpp > $(GCOV_OUTPUTS)/stdout.txt
	mv *.gcov $(GCOV_OUTPUTS)
	mv *.gcda $(GCOV_OUTPUTS)
	mv *.gcno $(GCOV_OUTPUTS)

tidy:
	clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --
	# clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

valgrind: $(TARGET)
	valgrind --tool=memcheck $(VALGRIND_FLAGS) ./$(TARGET) 2>&1 | { egrep "lost| at " || true; }

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

clean_gcov_folder:
	rm -f $(GCOV_OUTPUTS)/* $(TARGET).gcda $(TARGET).gcno $(TARGET).cpp.gcov $(TARGET).gcov