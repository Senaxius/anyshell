CC = g++
FLAGS = -g
FLAGS += -Wl,--no-as-needed
FLAGS += -lmariadb
FLAGS += -std=c++0x -pthread
MAKEFLAGS += --silent

SOURCEDIR = src
BUILDDIR = .

EXECUTABLE = anyshell
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/*.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(BUILDDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(FLAGS) $^ -o $@

# $(OBJECTS): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
# 	$(CC) $(FLAGS) $< -o $@

clean:
	rm -f $(BUILDDIR)/*o $(BUILDDIR)/$(EXECUTABLE)

run:
	$(BUILDDIR)/$(EXECUTABLE)
