CC = gcc
CFLAGS = -Isrc/SDL2/include -Isrc/GLEW/include
LDFLAGS = -Lsrc/SDL2/lib -Lsrc/GLEW/lib/Release/x64 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lglew32 -lopengl32 -Wall

SRC = src/main.c src/mesh.c src/math3d.c src/shader.c
BUILD_DIR = src/build
OBJ = $(SRC:src/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/main.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	$(TARGET)

clean:
	del /Q $(subst /,\,$(OBJ)) $(subst /,\,$(TARGET))