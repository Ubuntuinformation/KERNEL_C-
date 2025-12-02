# HubbleOS - Complete x86 BIOS Kernel with Package Manager

HubbleOS é um sistema operacional x86 32-bit BIOS baseado em multiboot, escrito em C+NASM, com suporte a:
- **Kernel com drivers**: ATA (IDE), Virtio-Net (QEMU)
- **Filesystem mínimo**: sistema de arquivos em memória com suporte a múltiplos arquivos
- **Formato EFH**: Executable For HubbleOS - formato nativo de executáveis
- **HPM**: Hubble Package Manager - gerenciador de pacotes integrado
- **Shell interativo**: interface de linha de comando com suporte a comandos built-in
- **Rede**: suporte a drivers de rede para download de pacotes (via HTTPS/HTTP)

## Estrutura do Projeto

```
/
├── Makefile              # Build system (nasm + gcc + ld)
├── src/
│   ├── start.asm         # Entry point (multiboot header + setup)
│   ├── linker.ld         # Linker script (load at 1MB)
│   ├── kernel.c          # Main kernel + shell REPL
│   ├── drivers.h/c       # ATA + Virtio-Net drivers
│   ├── filesystem.h/c    # Minimal filesystem
│   ├── efh.h/c           # EFH format serialization
│   └── hpm.h/c           # Package manager
├── packages/
│   ├── git.efh           # Git vcs (compilado como .efh)
│   ├── curl.efh          # HTTP client (compilado como .efh)
│   ├── gcc.efh           # Compilador C (compilado como .efh)
│   ├── bash.efh          # Shell (compilado como .efh)
│   └── make.efh          # Build tool (compilado como .efh)
└── hubbleos.iso          # Imagem bootável (gerada com grub-mkrescue)

```

## Build

### Dependências

```bash
apt-get install nasm gcc-multilib grub-pc-bin xorriso qemu-system-x86
```

### Compilar

```bash
make clean
make
```

Gera `hubbleos.iso` (imagem bootável BIOS/GRUB).

## Executar

```bash
make run
```

Lança QEMU com a ISO carregada.

## Formato EFH

Cada executável é um arquivo `.efh` com formato:

```c
struct efh_header {
    char magic[4];      // "EFH\0"
    uint32_t version;   // 1
    uint32_t entry_offset;  // Offset ao código executável
    uint32_t code_size;
    uint32_t data_offset;
    uint32_t data_size;
    char name[32];
    char author[32];
    char desc[64];
    // Seguido por: código + dados
};
```

Executáveis podem ser instalados via HPM e executados no kernel.

## Gerenciador de Pacotes (HPM)

Comandos disponíveis no shell:

```
hpm list              # Lista pacotes disponíveis
hpm search <query>    # Procura por pacote
hpm install <pkg>     # Instala e compila pacote como .efh
hpm remove <pkg>      # Remove pacote instalado
```

Pacotes pré-configurados:
- **git** (2.40.0) - Sistema de controle de versão
- **curl** (8.0.1) - Cliente HTTP/HTTPS
- **gcc** (12.2.0) - Compilador C/C++
- **bash** (5.2.15) - Shell avançado
- **make** (4.3) - Build automation

## Comandos do Shell

```
help                  # Mostra ajuda
echo <texto>          # Imprime texto
ls                    # Lista arquivos no filesystem
cat <arquivo>         # Mostra conteúdo de arquivo
hpm list              # Lista pacotes disponíveis
hpm search <query>    # Procura pacotes
hpm install <pkg>     # Instala pacote
reboot                # Reinicia o sistema
exit                  # Desliga o sistema
```

## Arquitetura

- **Bootloader**: Multiboot + GRUB (boot/legacy BIOS)
- **Kernel**: Modo protegido 32-bit, execução aninhada
- **Drivers**: ATA (disco IDE) + Virtio-Net (rede QEMU)
- **Filesystem**: Em-memória com até 32 arquivos
- **Execução**: Formato EFH com jump direto para entry point
- **Rede**: Virtio para download de pacotes via HTTP/HTTPS

## Nota

Este é um projeto educacional. Alguns recursos como download real de pacotes via HTTPS, compilação on-the-fly de git/curl/gcc são stubs/simplificados. Para uso completo, seria necessário:

1. Implementar driver de rede virtio-net completo (TX/RX)
2. Stack TCP/IP mínimo (ARP, IP, TCP, HTTP/HTTPS)
3. Compilador C minimalista ou cross-compilation de ferramentas
4. Filesystem persistente (FAT12 ou simples ext2)

## Licença

Público (Educational).
