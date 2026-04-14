# Nombre del ejecutable
TARGET = AtomicRacers

# Directorios
SRC_DIR = src
BUILD_DIR = build
GLAD_DIR = glad
FMOD_DIR = fmod
FMOD_CORE_INC = $(FMOD_DIR)/api/core/inc
FMOD_STUDIO_INC = $(FMOD_DIR)/api/studio/inc
FMOD_CORE_LIB = $(FMOD_DIR)/api/core/lib/x86_64
FMOD_STUDIO_LIB = $(FMOD_DIR)/api/studio/lib/x86_64

# Compilador y flags
CXXFLAGS = -std=c++23 -Wall -Wconversion -Wextra -Wpedantic -g
LDFLAGS = -Wl,-rpath,$(FMOD_CORE_LIB):$(FMOD_STUDIO_LIB)

# Librerías
LIBS = -lglfw -lraylib -lpthread -ldl -lrt -lX11
LIBS += -lBulletDynamics -lBulletCollision -lLinearMath -lBullet3Dynamics
LIBS += -lassimp -lvlc
LIBS += $(FMOD_CORE_LIB)/libfmod.so
LIBS += $(FMOD_STUDIO_LIB)/libfmodstudio.so

# Includes
INCLUDES = -I$(SRC_DIR) -I$(GLAD_DIR)/include
INCLUDES += -I$(FMOD_CORE_INC) -I$(FMOD_STUDIO_INC)
INCLUDES += -I/usr/include/bullet

# Archivos fuente y objetos
CPP_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_FILES))

# GLAD
GLAD_SRC = $(GLAD_DIR)/src/glad.c
GLAD_OBJ = $(BUILD_DIR)/glad.o

# Regla principal
all: $(TARGET)

# Compilar el ejecutable en la raíz
$(TARGET): $(OBJ_FILES) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) $(LDFLAGS)

# Compilar .cpp → build/.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compilar GLAD
$(GLAD_OBJ): $(GLAD_SRC)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ -I$(GLAD_DIR)/include

# Limpieza
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
