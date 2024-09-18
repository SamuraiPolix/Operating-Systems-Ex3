# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -g
VALGRIND_FLAGS=-v --leak-check=full --show-leak-kinds=all  --error-exitcode=99

# Target executable
TARGET = server

# Source files
SRCS = Proactor.cpp Kosaraju.cpp Server.cpp

# Header files (for dependency tracking)
HEADERS = Proactor.hpp Kosaraju.hpp

# Object files (replace .cpp with .o)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

coverage:
	g++ -std=c++17 -Wall -Wextra -pthread -fprofile-arcs -ftest-coverage -o server Proactor.cpp Kosaraju.cpp Server.cpp

#  clang tidy to check evry single thing
tidy:
	clang-tidy $(SRCS) -checks=bugprone-*,clang-analyzer-*,cppcoreguidelines-*,performance-*,portability-*,readability-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-owning-memory --warnings-as-errors=-* --

valgrind: $(TARGET)
	valgrind --tool=memcheck $(VALGRIND_FLAGS) ./$(TARGET) 2>&1

# Clean object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean coverage
