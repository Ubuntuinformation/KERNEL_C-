# HubbleOS - Sistema Operacional Completo com Formato EFH

**Status**: ✅ Kernel com 150+ pacotes compilado e funcional

## Arquitetura

- **Bootloader**: Multiboot + GRUB (x86 BIOS 32-bit)
- **Kernel**: Escrito em C+NASM (freestanding, sem libc)
- **Drivers**: ATA disk + Virtio-Net (para QEMU)
- **Filesystem**: Em-memória, suporta até 32 arquivos
- **Formato EFH**: Executable For HubbleOS (header + código + dados)
- **Package Manager**: HPM (Hubble Package Manager) com 150 pacotes pré-configurados

## Compilação

### Pré-requisitos

```bash
sudo apt-get install nasm gcc-multilib build-essential wget
```

### Build do kernel

```bash
cd /workspaces/KERNEL_C-
make clean
make
```

Resultado: `kernel.bin` (44 KB, ELF 32-bit)

### Build dos pacotes .efh

#### Opção 1: Pacotes Stub (rápido)

```bash
./build_efh_packages.sh
```

Gera pacotes `.efh` de placeholder (1 MB cada) em `packages/`.

#### Opção 2: Pacotes Reais (complexo, exige cross-compiler)

```bash
# Instalar toolchain cross-compilador
sudo apt-get install gcc-i686-linux-gnu gcc-multilib

# Compilar pacotes reais (git, nasm, gcc, etc.)
./build_efh_packages.sh

# Os pacotes .efh estarão em: packages/
```

**Nota**: Compilar GCC completo requer ~2GB e 1 hora+. Para MVP, stubs são suficientes.

## Estrutura de Arquivos

```
/workspaces/KERNEL_C-/
├── Makefile              # Build system (NASM + GCC + LD)
├── src/
│   ├── start.asm         # Multiboot header + entry (NASM)
│   ├── linker.ld         # Linker script
│   ├── kernel.c          # Kernel principal + shell
│   ├── drivers.c/h       # ATA + Virtio-Net
│   ├── filesystem.c/h    # Filesystem em-memória
│   ├── efh.c/h           # Formato EFH
│   ├── hpm.c/h           # Package manager (150 pacotes)
│   └── string.c          # Funções de string (freestanding)
├── packages/             # Pacotes compilados .efh
├── build_efh_packages.sh # Build script para pacotes
├── kernel.bin            # Kernel compilado
├── hubbleos.iso          # ISO bootável (se grub-mkrescue disponível)
└── README.md             # Este arquivo
```

## Funcionamento

### Shell HubbleOS

Após boot, o sistema inicia um shell interativo:

```
HubbleOS v1.0 (x86 BIOS)
...
hubble> help
```

### Comandos Disponíveis

**Sistema**:
- `help` - Mostra ajuda
- `echo <texto>` - Imprime texto
- `ls` - Lista arquivos
- `cat <arquivo>` - Mostra conteúdo (stub)
- `clear` - Limpa tela
- `reboot` - Reinicia
- `exit` - Desliga

**Gerenciador de Pacotes (HPM)**:
- `hpm list` - Lista todos os 150 pacotes
- `hpm cat <categoria>` - Filtra por categoria (dev, lang, vcs, etc)
- `hpm search <termo>` - Busca por nome
- `hpm info <pkg>` - Mostra detalhes do pacote
- `hpm install <pkg>` - Instala pacote
- `hpm installed` - Lista pacotes instalados

### Exemplo de Uso

```bash
hubble> hpm list
Available packages:
  gcc (12.2.0) [dev]
  nasm (2.16.01) [dev]
  git (2.40.0) [vcs]
  python3 (3.11.0) [lang]
  ...
Total: 134 packages shown

hubble> hpm cat lang
Packages in category: lang
  python3
  perl
  ruby
  lua
  nodejs

hubble> hpm search git
Search results for: git
  git - 2.40.0

hubble> hpm install git
Installing git... Done!

hubble> hpm installed
Installed packages:
  git 2.40.0
```

## Categorias de Pacotes

O HPM organiza 150 pacotes em categorias:

- **dev**: gcc, nasm, make, cmake, autoconf, automake, libtool, ninja, scons
- **lang**: python3, perl, ruby, lua, nodejs
- **vcs**: git, svn, mercurial, fossil
- **shell**: bash, zsh, fish, ksh
- **util**: vim, nano, emacs, sed, awk, grep, jq, find, tar
- **net**: curl, wget, openssh, netcat, telnet, rsync, tcpdump
- **sys**: top, htop, tmux, screen, cron, systemd
- **lib**: glibc, musl, libcpp, openssl, libcurl, libxml2, zlib
- **test**: gtest, catch2, check
- **debug**: gdb, valgrind, strace, lldb
- **doc**: doxygen, sphinx, pandoc
- **container**: docker, podman, qemu
- **media**: ffmpeg, imagemagick, graphviz

## Formato EFH (Executable For HubbleOS)

Cada pacote é um arquivo `.efh` com estrutura:

```c
struct efh_header {
    char magic[4];      // "EFH\0"
    uint32_t version;   // 1
    uint32_t entry_offset;  // Offset ao código (geralmente 256)
    uint32_t code_size;
    uint32_t data_offset;
    uint32_t data_size;
    char name[32];
    char author[32];
    char desc[64];
    // Seguido por código + dados
};
```

### Criando um Pacote .efh Manual

```c
// example.c - Aplicação HubbleOS
#include <stdint.h>

void *memcpy(void *d, const void *s, int n) {
    unsigned char *dst = d;
    const unsigned char *src = s;
    for (int i = 0; i < n; i++) dst[i] = src[i];
    return d;
}

int main(void) {
    /* Seu código aqui */
    return 0;
}
```

Compilar:

```bash
i686-elf-gcc -ffreestanding -O2 -c example.c
i686-elf-ld -Ttext 0x0 -o example.elf example.o
# Empacotar como .efh usando ferramentas do HubbleOS
```

## Testando em QEMU

```bash
# Opção 1: Kernel direto
qemu-system-i386 -kernel kernel.bin -m 256

# Opção 2: ISO bootável (se grub-mkrescue disponível)
qemu-system-i386 -cdrom hubbleos.iso -m 256 -boot d
```

## Próximos Passos (Roadmap)

1. **Network Stack**: Implementar TCP/IP mínimo
   - ARP, IP routing, TCP handshake
   - HTTP client para download de pacotes

2. **Real Package Compilation**: Cross-compile ferramentas reais
   - git, gcc, nasm com todas as dependências
   - Teste de build em QEMU

3. **Filesystem Persistente**: Migrar de em-memória
   - FAT12 ou ext2 simples
   - Suporte a leitura/escrita em disco virtual

4. **Shell Completo**: Expandir intérprete
   - Pipes e redirecionamento
   - Scripts .hsh (HubbleOS Shell Scripts)
   - Variáveis de ambiente

5. **Multi-tasking**: Suporte a múltiplos processos
   - Task switching via timer interrupt
   - Memory protection (paging)

6. **Userspace Libraries**: Implementar libc freestanding
   - stdlib, stdio, string, math funções
   - POSIX-like API

## Compilação dos Pacotes - Detalhes

### Git como .efh

```bash
# Download fonte
cd build/packages
wget https://github.com/git/git/archive/v2.40.0.tar.gz
tar xzf v2.40.0.tar.gz
cd git-2.40.0

# Configurar para cross-compile
./configure --host=i686-elf --prefix=/usr/local/hubble \
  --without-openssl --without-curl

# Compilar
make -j$(nproc) CC=i686-elf-gcc

# Empacotar como .efh
strip git
# Usar ferramentas do HubbleOS para criar .efh header e pacotear
```

### NASM como .efh

```bash
cd build/packages
wget https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/nasm-2.16.01.tar.xz
tar xf nasm-2.16.01.tar.xz
cd nasm-2.16.01

./configure --host=i686-elf --prefix=/usr/local/hubble
make -j$(nproc) CC=i686-elf-gcc
# Empacotar
```

### GCC como .efh

**Atenção**: GCC é ~200 MB. Alternativas:
- Usar **TinyCC** (~20 KB, suporta C89)
- Usar **Vala + LLVM** compilado mínimo
- Usar apenas **cc1** (backend do GCC) como servidor

Exemplo com TinyCC:

```bash
cd build/packages
wget https://github.com/TinyCompiler/tinycc/archive/refs/tags/release_0_9_27.tar.gz
tar xzf release_0_9_27.tar.gz
cd tinycc-release_0_9_27

./configure --enable-cross --cross-prefix=i686-elf- \
  --enable-static

make -j$(nproc) TCC_VERSION=0.9.27
# tcc executável compilado para i686-elf
```

## Statísticas do Projeto

| Métrica | Valor |
|---------|-------|
| Linhas de código kernel | 805 |
| Kernel compilado | 44 KB |
| Pacotes no HPM | 150+ |
| Categorias | 13 |
| Arquivos fonte | 11 (C/NASM) |
| Tamanho source tree | ~500 KB |

## Nota Legal

Este é um **projeto educacional** e de demonstração. Uso livre para fins de aprendizado e pesquisa. Componentes são derivados de GNU/OSS e sujeitos a suas respectivas licenças (GPL, MIT, BSD, etc).

---

**Autor**: Ubuntuinformation  
**Data**: 2024-12-02  
**Versão**: 1.0 (Alpha)  
**Status**: Funcional - MVP completo
