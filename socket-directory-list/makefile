# Nome dos executáveis
CLIENT_EXEC = client
SERVER_EXEC = server

# Fontes
CLIENT_SRC = client.c
SERVER_SRC = server.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall -O2

# Extensões para Windows
WINDOWS_SUFFIX = .exe
WINDOWS_FLAGS = -lws2_32

# Extensões para Linux
LINUX_SUFFIX =
LINUX_FLAGS = 

# Variáveis de ambiente para plataforma
PLATFORM ?= linux

# Configuração baseada na plataforma
ifeq ($(PLATFORM), windows)
    EXEC_SUFFIX = $(WINDOWS_SUFFIX)
    EXTRA_FLAGS = $(WINDOWS_FLAGS)
else
    EXEC_SUFFIX = $(LINUX_SUFFIX)
    EXTRA_FLAGS = $(LINUX_FLAGS)
endif

# Alvos principais
all: $(CLIENT_EXEC)$(EXEC_SUFFIX) $(SERVER_EXEC)$(EXEC_SUFFIX)

# Compilar client
$(CLIENT_EXEC)$(EXEC_SUFFIX): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -o $@ $<

# Compilar server
$(SERVER_EXEC)$(EXEC_SUFFIX): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -o $@ $<

# Executar client
run-client:
	./$(CLIENT_EXEC)$(EXEC_SUFFIX)

# Executar server
run-server:
	./$(SERVER_EXEC)$(EXEC_SUFFIX)

# Limpeza
clean:
	rm -f $(CLIENT_EXEC)$(EXEC_SUFFIX) $(SERVER_EXEC)$(EXEC_SUFFIX)
