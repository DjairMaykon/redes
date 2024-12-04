## Projetos de Redes: Comunicação Cliente-Servidor com Sockets em C

Este repositório contém dois projetos de redes, implementando comunicação cliente-servidor utilizando sockets em C. Além disso, há uma pasta dedicada à análise de desempenho, com scripts em Python para avaliar métricas como throughput e bandwidth.

---

## Estrutura do Repositório

```
📦
├── p1/                # Projeto 1: Sistema Cliente-Servidor de listagem de diretórios do cliente
├── p2/                # Projeto 2: TO-DO
├── analise/           # Scripts para análise de desempenho
└── README.md          # Este arquivo
```

---

### **Projeto 1 (`p1/`)**
#### Descrição
Este projeto implementa um sistema cliente-servidor em C. A funcionalidade principal é transferir uma lista de nome de arquivos de um diretório especificado do cliente para o servidor, que salva os arquivos em um único arquivo consolidado.

#### **Funcionamento**
- **Cliente**:
  - Recebe como argumento o caminho de um diretório contendo arquivos.
  - Envia a lista de arquivos ao servidor utilizando sockets.
  
- **Servidor**:
  - Recebe a lista de arquivos do cliente.
  - Salva os dados recebidos em um arquivo local.

Aqui está o Makefile atualizado para que você escolha explicitamente entre compilar para Windows ou Linux ao invés de o script detectar o sistema operacional automaticamente:

### Makefile

```makefile
# Nome dos executáveis
CLIENTE_EXEC = cliente
SERVIDOR_EXEC = servidor

# Fontes
CLIENTE_SRC = cliente.c
SERVIDOR_SRC = servidor.c

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
all: $(CLIENTE_EXEC)$(EXEC_SUFFIX) $(SERVIDOR_EXEC)$(EXEC_SUFFIX)

# Compilar cliente
$(CLIENTE_EXEC)$(EXEC_SUFFIX): $(CLIENTE_SRC)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -o $@ $<

# Compilar servidor
$(SERVIDOR_EXEC)$(EXEC_SUFFIX): $(SERVIDOR_SRC)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -o $@ $<

# Executar cliente
run-cliente:
	./$(CLIENTE_EXEC)$(EXEC_SUFFIX)

# Executar servidor
run-servidor:
	./$(SERVIDOR_EXEC)$(EXEC_SUFFIX)

# Limpeza
clean:
	rm -f $(CLIENTE_EXEC)$(EXEC_SUFFIX) $(SERVIDOR_EXEC)$(EXEC_SUFFIX)
```

---

### Atualização do README

#### **Compilação e Execução com Makefile**

1. Certifique-se de ter o GNU Make e o compilador `gcc` instalados.
   - **Ubuntu**: `sudo apt install build-essential`
   - **Windows**: Use o **MinGW** ou outra distribuição compatível.

2. Navegue até a pasta `p1`:
   ```bash
   cd p1
   ```

3. Escolha a plataforma para compilação:
   - Para Linux:
     ```bash
     make PLATFORM=linux
     ```
   - Para Windows:
     ```bash
     make PLATFORM=windows
     ```

4. Execute os programas:
   - Para o servidor:
     ```bash
     make run-servidor
     ```
   - Para o cliente:
     ```bash
     make run-cliente
     ```

5. Limpe os arquivos gerados:
   ```bash
   make clean
   ```

---

### **Projeto 2 (`p2/`)**
- **Status**: TO-DO
- **Descrição**: Implementação futura a ser definida.

---

### **Análise (`analise/`)**
- **Status**: TO-DO
- **Descrição**: Implementação futura a ser definida.
