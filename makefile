# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -MMD -MP -DGLAD_GL \
           -Iinclude -I"C:/libs/glm" -I"C:/libs/glad/include" -I"C:/libs/GLFW-3.4/include" \
           -Isrc -Isrc/gui -I"C:/libs/SFML-3.0.2/include"

LDFLAGS = 

# Backend configuration
GLFW_DIR = C:/libs/GLFW-3.4
GLFW_LIB = -L"$(GLFW_DIR)/lib-vc2022" -lglfw3

SFML_DIR = C:/libs/SFML-3.0.2
SFML_LIB = -L"$(SFML_DIR)/lib" -lsfml-graphics -lsfml-window -lsfml-system

# OpenGL flags
GL_FLAGS = -lopengl32 -lgdi32

# GLAD configuration
GLAD_SRC = c:/libs/glad/src/glad.c
GLAD_OBJ = obj/glad.o

# Project structure
SRCDIR = src
OBJDIR = obj
SHADERDIR = $(SRCDIR)/shaders

# Output directory for final executable
TARGETDIR = output
TARGET = $(TARGETDIR)/3DGameOfLife.exe

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/gui/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst $(SRCDIR)/gui/%.cpp,$(OBJDIR)/gui/%.o,$(OBJECTS))
DEPENDS = $(OBJECTS:.o=.d)

# Default target
all: $(TARGET)

# Create directories
$(OBJDIR):
	if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"

$(OBJDIR)/gui:
	if not exist "$(OBJDIR)/gui" mkdir "$(OBJDIR)/gui"

$(TARGETDIR):
	if not exist "$(TARGETDIR)" mkdir "$(TARGETDIR)"

$(TARGETDIR)/shaders:
	if not exist "$(TARGETDIR)/shaders" mkdir "$(TARGETDIR)/shaders"

# Main executable target (builds inside output/)
$(TARGET): $(TARGETDIR) $(OBJDIR) $(OBJDIR)/gui $(OBJECTS) $(GLAD_OBJ)
	$(CXX) $(OBJECTS) $(GLAD_OBJ) -o $@ $(LDFLAGS) $(SFML_LIB) $(GLFW_LIB) $(GL_FLAGS)

# Compile C++ sources
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GUI sources
$(OBJDIR)/gui/%.o: $(SRCDIR)/gui/%.cpp | $(OBJDIR)/gui
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Copy shaders to output/shaders/
shaders: $(TARGETDIR)/shaders
	@echo Copying shaders...
	@copy "$(SHADERDIR)\*.glsl" "$(TARGETDIR)\shaders\" >nul 2>&1 || echo No shaders found.

# Copy SFML and GLFW DLLs to output/
copy-dlls: $(TARGETDIR)
	@echo Copying SFML DLLs...
	@if exist "$(SFML_DIR)\bin\*.dll" ( \
		copy /Y "$(SFML_DIR)\bin\*.dll" "$(TARGETDIR)\" >nul && \
		echo SFML DLLs copied successfully. \
	) else ( \
		echo No SFML DLLs found to copy. \
	)

	@echo Copying GLFW DLLs...
	@if exist "$(GLFW_DIR)\bin\*.dll" ( \
		copy /Y "$(GLFW_DIR)\bin\*.dll" "$(TARGETDIR)\" >nul && \
		echo GLFW DLLs copied successfully. \
	) else ( \
		echo No GLFW DLLs found to copy. \
	)


# Build full package (exe + shaders + dlls)
build: all shaders copy-dlls

# Run the program
run: build
	@$(TARGET)

.PHONY: all build run shaders copy-dlls
