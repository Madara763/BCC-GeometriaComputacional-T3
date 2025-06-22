CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

EXEC = bsp

# Regra padrão
all: $(EXEC)

# Linka o executável
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compila arquivos objeto na raiz (sem bin/)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# mensagens de debug
debug: CXXFLAGS += -DDEBUG
debug: all

# Limpa arquivos objetos
clean:
	rm -f $(SRC_DIR)/*.o

# Limpa tudo
purge: clean
	rm -f $(EXEC)