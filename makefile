# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -MMD -MP -DGLAD_GL \
           -Iinclude -Iexternal/glm -Iexternal/glad/include -Isrc -Isrc/gui \
           -I"C:/SFML-3.0.2/include"
LDFLAGS = 

# SFML configuration
SFML_DIR = C:/SFML-3.0.2
SFML_LIB = -L"$(SFML_DIR)/lib" -lsfml-graphics -lsfml-window -lsfml-system

# OpenGL flags
GL_FLAGS = -lopengl32 -lgdi32

# GLAD configuration
GLAD_SRC = external/glad/src/glad.c
GLAD_OBJ = obj/glad.o

# Project structure
SRCDIR = src
OBJDIR = obj
SHADERDIR = $(SRCDIR)/shaders

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/gui/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst $(SRCDIR)/gui/%.cpp,$(OBJDIR)/gui/%.o,$(OBJECTS))
DEPENDS = $(OBJECTS:.o=.d)

# Target (in root directory)
TARGET = 3DGameOfLife.exe

# Default target
all: $(TARGET)

# Create directories
$(OBJDIR):
	if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"

$(OBJDIR)/gui:
	if not exist "$(OBJDIR)/gui" mkdir "$(OBJDIR)/gui"

# Main target
$(TARGET): $(OBJDIR) $(OBJDIR)/gui $(OBJECTS) $(GLAD_OBJ)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) -o $@ $(LDFLAGS) $(SFML_LIB) $(GL_FLAGS)

# Compile C++ sources
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GUI sources
$(OBJDIR)/gui/%.o: $(SRCDIR)/gui/%.cpp | $(OBJDIR)/gui
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Copy shaders to binary directory (optional)
shaders:
	@echo Copying shaders...
	@copy "$(SHADERDIR)\*.glsl" ".\shaders\" >nul 2>&1 || echo No shaders to copy

# Copy SFML DLLs to current dir (optional)
copy-dlls:
	@echo Copying SFML DLLs...
	@copy "$(SFML_DIR)\bin\*.dll" ".\" >nul 2>&1 || echo Could not copy DLLs

# Build with shaders and DLLs
build: all shaders copy-dlls

# Run the program
run: build
	@3DGameOfLife.exe

# Clean build artifacts
clean:
	@if exist "$(OBJDIR)" rmdir /s /q "$(OBJDIR)"
	@if exist "shaders" rmdir /s /q "shaders"
	@if exist "3DGameOfLife.exe" del "3DGameOfLife.exe"

.PHONY: all build run clean shaders copy-dlls
