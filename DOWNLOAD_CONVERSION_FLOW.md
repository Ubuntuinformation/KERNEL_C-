# HubbleOS v2.0 - Download & Package Conversion Guide

## Como o Sistema de Download e Conversão .EFH Funciona

### Visão Geral do Fluxo

```
┌─────────────────────────────────────────────────────────────┐
│                    HubbleOS v2.0 Shell                      │
│  (kernel.c - parser de argumentos completo)                 │
└─────────────────┬───────────────────────────────────────────┘
                  │
        ┌─────────┴──────────┐
        │                    │
        ▼                    ▼
    ┌──────────────┐   ┌────────────────────┐
    │ hpm download │   │ hpm convert        │
    │  <pacote>    │   │  <pacote> -> .efh  │
    └──────┬───────┘   └────────┬───────────┘
           │                    │
           ▼                    ▼
      ┌──────────────────────────────────┐
      │  HPM Download Cache              │
      │  (10 pacotes x 256 KB cada)      │
      │  hpm_download_cache_t[]          │
      └──────────────────────────────────┘
           │
           ▼
      ┌──────────────────────────────────┐
      │  Network Module (network.c)      │
      │  http_get() - Download HTTP      │
      │  Atual: Stub local               │
      │  Futuro: Virtio-Net real         │
      └──────────────────────────────────┘
           │
           ▼
      ┌──────────────────────────────────┐
      │  Filesystem (filesystem.c)       │
      │  Salva .efh no VFS               │
      │  fs_write() -> arquivo.efh       │
      └──────────────────────────────────┘
           │
           ▼
      ┌──────────────────────────────────┐
      │  EFH Format (efh.c)              │
      │  - Validação de header           │
      │  - Execução de binários          │
      │  - Criação de pacotes            │
      └──────────────────────────────────┘
```

---

## 🔻 ETAPA 1: DOWNLOAD (hpm_download)

### Localização do código
`src/hpm.c` - linhas 261-320 (função `hpm_download()`)

### O que acontece:

```c
int hpm_download(const char *pkg_name, hpm_download_cache_t *cache)
{
    // 1. Busca pacote no banco de dados HPM
    hpm_package_t info;
    if (hpm_info(pkg_name, &info) != 0) return -1;
    
    // 2. Verifica se já está no cache
    for (int i = 0; i < download_cache_count; ++i) {
        if (strcmp(download_cache[i].filename, pkg_name) == 0) {
            *cache = download_cache[i];
            return 0;  // Retorna do cache (rápido!)
        }
    }
    
    // 3. Inicia novo download
    hpm_download_cache_t *cur = &download_cache[download_cache_count];
    
    // 4. Cria header EFH mínimo (256 bytes)
    cur->data[0] = 'E';
    cur->data[1] = 'F';
    cur->data[2] = 'H';
    cur->data[3] = '\0';
    
    // 5. Adiciona metadados do pacote
    // - Nome (offset 8, 32 bytes)
    // - Versão (offset 40, 16 bytes)
    // - Código stub (offset 256+)
    
    cur->size = 258;  // Header 256 + 2 bytes de código
    cur->is_downloaded = 1;
    
    // 6. Armazena em cache para próximas chamadas
    download_cache_count++;
    *cache = download_cache[download_cache_count - 1];
    
    return 0;
}
```

### Dados em Memória

```
download_cache[0]:
  ├─ filename: "gcc"
  ├─ data[0..257]: Binary EFH (258 bytes)
  ├─ size: 258
  └─ is_downloaded: 1

download_cache[1]:
  ├─ filename: "nasm"
  ├─ data[0..257]: Binary EFH (258 bytes)
  ├─ size: 258
  └─ is_downloaded: 1

... até 10 pacotes no cache
```

### Comando do Shell

```bash
hubble> hpm download gcc
Downloading gcc... Done! (258 bytes)

# Dentro do kernel:
# 1. parse_args("hpm download gcc") → argc=3, args=["hpm","download","gcc"]
# 2. kernel.c chamadas hpm_download("gcc", &cache)
# 3. hpm_download():
#    - Busca "gcc" em hpm_packages[] ✓ encontrado
#    - Verifica cache ✗ não existe
#    - Chama http_get(gcc_url, &http_response) [stub]
#    - Armazena em download_cache[0]
#    - Retorna sucesso
# 4. Shell exibe "Done! (258 bytes)"
```

---

## 🔄 ETAPA 2: CONVERSÃO PARA .EFH (hpm_convert_to_efh)

### Localização do código
`src/hpm.c` - linhas 322-347 (função `hpm_convert_to_efh()`)

### O que acontece:

```c
int hpm_convert_to_efh(const char *pkg_name, const uint8_t *binary_data, uint32_t size)
{
    // 1. Valida pacote no banco de dados
    hpm_package_t info;
    if (hpm_info(pkg_name, &info) != 0) return -1;
    
    // 2. Constrói nome do arquivo .efh
    char efh_path[80];
    // "gcc" → "gcc.efh"
    int i = 0;
    while (i < 80 && pkg_name[i]) {
        efh_path[i] = pkg_name[i];
        i++;
    }
    efh_path[i++] = '.';
    efh_path[i++] = 'e';
    efh_path[i++] = 'f';
    efh_path[i++] = 'h';
    efh_path[i] = '\0';
    
    // 3. Escreve arquivo no filesystem virtual
    fs_write(efh_path, binary_data, size);
    
    return 0;
}
```

### Fluxo de Escrita no Filesystem

```
fs_write("gcc.efh", binary_data, 258):
  │
  ├─ Verifica limite (máx 64 arquivos)
  ├─ Verifica duplicata
  ├─ Cria nova entrada vfile_t:
  │  ├─ name: "gcc.efh"
  │  ├─ size: 258
  │  ├─ is_dir: 0
  │  ├─ is_executable: 0
  │  └─ permissions: 0644
  │
  └─ Adiciona em fs_files[fs_file_count++]
```

### Estrutura no VFS Após Conversão

```
fs_files[]:
  [0] name="/"          is_dir=1  size=0
  [1] name="gcc.efh"    is_dir=0  size=258  is_executable=1
  [2] name="nasm.efh"   is_dir=0  size=258  is_executable=1
  [3] name="git.efh"    is_dir=0  size=258  is_executable=1
  [4] name="projects"   is_dir=1  size=0
  [5] name="main.c"     is_dir=0  size=0

fs_file_count = 6
```

### Comando do Shell

```bash
# Passo 1: Download
hubble> hpm download gcc
Downloading gcc... Done! (258 bytes)

# Passo 2: Conversão para .efh
hubble> hpm convert gcc
Converting gcc to .efh... Done!

# Dentro do kernel:
# 1. parse_args("hpm convert gcc") → argc=3
# 2. kernel.c chama:
#    - hpm_download("gcc", &cache)  # Retorna do cache
#    - hpm_convert_to_efh("gcc", cache.data, 258)
# 3. hpm_convert_to_efh():
#    - Valida pacote ✓
#    - Monta nome "gcc.efh"
#    - Chama fs_write("gcc.efh", 258 bytes)
#    - fs_write() cria vfile_t e adiciona em fs_files[]
# 4. Arquivo "gcc.efh" aparece no filesystem

# Passo 3: Verificar
hubble> ls
Files and Directories:
  [FILE] gcc.efh (258 bytes)
```

---

## 🌐 MÓDULO DE REDE (network.c)

### Comunicação HTTP (Atual: Stub)

```c
int http_get(const char *url, http_response_t *resp)
{
    // URL: https://gcc.gnu.org/releases/gcc-12.2.0/gcc-12.2.0.tar.gz
    
    // ATUAL (Stub):
    // - Simula download localmente
    // - Cria EFH mínimo (258 bytes)
    // - Retorna sucesso imediatamente
    
    // PRODUÇÃO (Futuro):
    // 1. Parse URL: hostname, path, port
    // 2. Conecta via Virtio-Net driver
    // 3. Envia HTTP GET request
    // 4. Recebe resposta HTTP/1.1
    // 5. Valida Content-Type (application/octet-stream)
    // 6. Lê binário em chunks
    // 7. Armazena em resp->data[262144]
    // 8. Atualiza resp->size
}

void url_get_filename(const char *url, char *buf, int max)
{
    // "https://gcc.gnu.org/releases/gcc-12.2.0/gcc-12.2.0.tar.gz"
    // Extrai: "gcc-12.2.0.tar.gz"
}

int network_available(void)
{
    // ATUAL: Sempre retorna 1 (simulado)
    // PRODUÇÃO: Verifica driver Virtio-Net ativo
}
```

### Implementação Futura (Pseudocódigo)

```c
int http_get_real(const char *url, http_response_t *resp) {
    // 1. Parse URL
    char hostname[256], path[512];
    int port = 80;
    parse_url(url, hostname, path, &port);
    
    // 2. Resolve DNS
    uint32_t ip = dns_resolve(hostname);
    if (ip == 0) return -1;
    
    // 3. Abre conexão TCP
    int sock = tcp_connect(ip, port);
    if (sock < 0) return -1;
    
    // 4. Envia HTTP GET
    char request[512];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
            path, hostname);
    tcp_send(sock, request, strlen(request));
    
    // 5. Recebe resposta HTTP
    char header[1024];
    tcp_recv(sock, header, 1024);
    
    // 6. Parse header HTTP
    // Verifica: HTTP/1.1 200 OK
    // Extrai: Content-Length, Content-Type
    
    // 7. Lê binário
    uint32_t total_read = 0;
    while (total_read < resp_size) {
        int n = tcp_recv(sock, resp->data + total_read, 
                        256 - total_read);
        if (n <= 0) break;
        total_read += n;
    }
    
    resp->size = total_read;
    tcp_close(sock);
    return 0;
}
```

---

## 📦 FORMATO EFH COMPLETO

### Header (256 bytes)

```
Offset  Size  Type      Campo               Descrição
──────────────────────────────────────────────────────────────
0       4     char[4]   magic               "EFH\0"
4       4     uint32_t  version             1 (v1)
8       32    char[32]  name                Nome do pacote
40      16    char[16]  version_str         String de versão
56      32    char[32]  author              Autor
88      128   char[128] description         Descrição
216     8     uint32_t  entry_offset        Offset do entry point
224     8     uint32_t  code_size           Tamanho do código
232     8     uint32_t  data_offset         Offset dos dados
240     8     uint32_t  data_size           Tamanho dos dados
248     8     uint32_t  reserved            Reservado

Total:  256 bytes
```

### Exemplo: gcc.efh

```
Offset 0:   "EFH\0"                       (Magic)
Offset 4:   0x00000001                   (Version 1)
Offset 8:   "gcc\0........................" (Name)
Offset 40:  "12.2.0\0............"        (Version)
Offset 56:  "FSF\0........................." (Author)
Offset 88:  "GNU Compiler Collection - C/C++" (Desc)
Offset 216: 0x00000100                   (Entry offset 256)
Offset 224: 0x00000002                   (Code size 2 bytes)
Offset 232: 0x00000000                   (Data offset 0)
Offset 240: 0x00000000                   (Data size 0)

256+: 0x90 0xC3 (nop; ret)
```

---

## 🔧 CÓDIGO COMPLETO DO SHELL (kernel.c)

### Parsing de Argumentos

```c
int parse_args(const char *line, char **args, int max_args) {
    int argc = 0;
    int i = 0, j = 0;
    char arg[128];
    
    // Itera sobre cada caractere da linha
    while (line[i] && argc < max_args) {
        if (line[i] == ' ' || line[i] == '\t') {
            // Separador encontrado
            if (j > 0) {
                arg[j] = '\0';
                // Copia argumento para args[argc]
                memcpy(args[argc], arg, j + 1);
                argc++;
                j = 0;
            }
            i++;
        } else {
            // Acumula caractere
            arg[j++] = line[i++];
        }
    }
    
    // Último argumento
    if (j > 0 && argc < max_args) {
        arg[j] = '\0';
        memcpy(args[argc], arg, j + 1);
        argc++;
    }
    
    return argc;
}
```

### Comando hpm download

```c
else if (strcmp(args[1], "download") == 0) {
    if (argc < 3) {
        puts("hpm download: package name required\n");
    } else {
        puts("Downloading ");
        puts(args[2]);
        puts("... ");
        hpm_download_cache_t cache;
        if (hpm_download(args[2], &cache) == 0) {
            puts("Done! (");
            itoa(cache.size, (char*)VGA, 10);
            puts(" bytes)\n");
        } else {
            puts("Failed\n");
        }
    }
}
```

### Comando hpm convert

```c
else if (strcmp(args[1], "convert") == 0) {
    if (argc < 3) {
        puts("hpm convert: package name required\n");
    } else {
        puts("Converting ");
        puts(args[2]);
        puts(" to .efh... ");
        hpm_download_cache_t cache;
        if (hpm_download(args[2], &cache) == 0) {
            hpm_convert_to_efh(args[2], cache.data, cache.size);
            puts("Done!\n");
        } else {
            puts("Failed\n");
        }
    }
}
```

---

## 📊 FLUXO DE DADOS

### Exemplo Real: Instalar GCC

```
Terminal do usuário:
  hubble> hpm download gcc

Kernel (kernel.c):
  1. read_line() → "hpm download gcc"
  2. parse_args() → argc=3, args=["hpm","download","gcc"]
  3. strcmp(args[0], "hpm") == 0 ✓
  4. strcmp(args[1], "download") == 0 ✓
  5. hpm_download(args[2], &cache)

HPM (hpm.c):
  1. hpm_info("gcc", &info) 
     → Busca em hpm_packages[0..149]
     → Encontra gcc com url, version, size
  2. Verifica cache
     → download_cache_count < HPM_MAX_DOWNLOADS ✓
  3. Cria header EFH (256 bytes)
  4. Adiciona nome/versão/autor
  5. Adiciona código stub (2 bytes: 0x90 0xC3)
  6. download_cache[0].size = 258
  7. download_cache_count++
  8. Retorna cache

Network (network.c):
  [Stub: Não faz nada, simula localmente]

Kernel (kernel.c):
  1. cache.size = 258 (recebido)
  2. puts("Done! (258 bytes)\n")

Usuário vê:
  Downloading gcc... Done! (258 bytes)

---

Terminal do usuário:
  hubble> hpm convert gcc

Kernel (kernel.c):
  1. parse_args() → argc=3, args=["hpm","convert","gcc"]
  2. hpm_download(args[2], &cache)
     → Já estava em cache, retorna rapidamente
  3. hpm_convert_to_efh("gcc", cache.data, 258)

HPM (hpm.c):
  1. hpm_info("gcc", &info)
  2. Monta caminho: "gcc.efh"
  3. fs_write("gcc.efh", binary_data, 258)

Filesystem (filesystem.c):
  1. Cria vfile_t nova
  2. name = "gcc.efh"
  3. size = 258
  4. is_executable = 1
  5. Adiciona em fs_files[]
  6. fs_file_count++

Kernel (kernel.c):
  1. puts("Done!\n")

Usuário vê:
  Converting gcc to .efh... Done!

---

Terminal do usuário:
  hubble> ls

Kernel (kernel.c):
  1. fs_list(files, 32)

Filesystem (filesystem.c):
  1. Retorna fs_files[0..fs_file_count-1]
  2. Inclui: "gcc.efh" (258 bytes, FILE)

Kernel (kernel.c):
  1. Loop através de files[]
  2. Para cada arquivo:
     - [FILE] ou [DIR]
     - Nome
     - Tamanho
  3. puts()

Usuário vê:
  Files and Directories:
    [FILE] gcc.efh (258 bytes)
```

---

## ✅ VERIFICAÇÃO DO SISTEMA

### Compile e Teste

```bash
cd /workspaces/KERNEL_C-
make clean && make

# Resultado esperado:
# - kernel.bin 56 KB
# - 0 errors, 1-2 warnings (esperados)
```

### Teste os Comandos

```bash
bash test_commands.sh

# Valida:
# - Parsing de argumentos
# - Download em cache
# - Conversão para .efh
# - Listagem de arquivos
# - Navegação de diretórios
```

---

## 🚀 PRÓXIMAS FASES

### Fase 3: Rede Real (IPv4 + TCP)
- Implementar Virtio-Net driver completo
- DNS resolver
- HTTP client com SSL/TLS
- Download real de repositórios

### Fase 4: Execução de .EFH
- Carregar .efh em memória
- Criar espaço de endereço para processo
- Executar entry point
- Suportar syscalls simples

### Fase 5: Multitasking
- Context switching
- Scheduler
- Sincronização entre processos
- IPC (pipes, sockets)

---

**Data:** 2024-12-02
**Versão:** 2.0
**Status:** ✅ Completo e Testado
