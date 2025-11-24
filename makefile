# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -MMD -MP -DGLAD_GL
LDFLAGS = 

# SFML configuration
SFML_DIR = C:/SFML-3.0.2
SFML_INC = -I"$(SFML_DIR)/include"
SFML_LIB = -L"$(SFML_DIR)/lib"
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# OpenGL flags
GL_FLAGS = -lopengl32 -lgdi32

# GLAD configuration
GLAD_INC = -Iglad/include
GLAD_SRC = glad/src/glad.c
GLAD_OBJ = obj/glad.o

# GLM configuration
GLM_INC = -Iglm

# Project structure
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SHADERDIR = $(SRCDIR)/shaders

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPENDS = $(OBJECTS:.o=.d)

# Target
TARGET = $(BINDIR)/3DGameOfLife.exe

# Default target
all: $(TARGET)

# Create directories
$(shell if not exist "$(OBJDIR)" mkdir "$(OBJDIR)")
$(shell if not exist "$(BINDIR)" mkdir "$(BINDIR)")
$(shell if not exist "$(BINDIR)\shaders" mkdir "$(BINDIR)\shaders")

# Main target
$(TARGET): $(OBJECTS) $(GLAD_OBJ)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) -o $@ $(LDFLAGS) $(SFML_LIB) $(SFML_FLAGS) $(GL_FLAGS)

# Compile C++ sources
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) $(GLAD_INC) $(GLM_INC) $(SFML_INC) -c $< -o $@

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC)
	$(CXX) $(CXXFLAGS) $(GLAD_INC) -c $< -o $@

# Copy shaders to binary directory
shaders:
	@echo Copying shaders...
	@copy "$(SHADERDIR)\*.glsl" "$(BINDIR)\shaders\" >nul 2>&1 || echo No shaders to copy

# Copy SFML DLLs
copy-dlls:
	@echo Copying SFML DLLs...
	@copy "$(SFML_DIR)\bin\*.dll" "$(BINDIR)\" >nul 2>&1 || echo Could not copy DLLs

# Build with shaders and DLLs
build: $(TARGET) shaders copy-dlls

# Run the program
run: build
	@cd "$(BINDIR)" && .\3DGameOfLife.exe

# Clean build artifacts
clean:
	@if exist "$(OBJDIR)" rmdir /s /q "$(OBJDIR)"
	@if exist "$(BINDIR)" rmdir /s /q "$(BINDIR)"

.PHONY: all build run clean shaders copy-dlls