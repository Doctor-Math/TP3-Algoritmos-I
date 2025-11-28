# Diretórios
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Arquivo alvo
TARGET = $(BIN_DIR)/tp3.out

# Lista de arquivos fonte e objeto
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror -I$(INCLUDE_DIR) -std=c++17

# Variável para arquivo de teste (padrão)
TEST_FILE ?= Testes/testCase01.txt

# Regra principal
all: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

# Regra para gerar o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para gerar os objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Criar diretórios se não existirem
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Recompilar do zero
rebuild: clean all

# Rodar com entrada de teste
run: all
	./$(TARGET) < $(TEST_FILE)