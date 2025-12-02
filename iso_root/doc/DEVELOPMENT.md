# HubbleOS - Developer Contributions & Extensions

## Como Criar um Novo Pacote .efh

### Passo 1: Escrever a Aplicação

```c
/* hello.c - Aplicação simples para HubbleOS */
#include <stdint.h>

/* Output serial/VGA */
void putchar(char c) {
    volatile uint8_t *serial = (uint8_t*)0x3F8;  /* COM1 */
    *serial = c;
}

void puts(const char *s) {
    while (*s) putchar(*s++);
}

/* Entry point - chamado pelo loader EFH */
int main(void) {
    puts("Hello from HubbleOS!\n");
    return 0;
}
```

### Passo 2: Compilar para i686-elf

```bash
i686-elf-gcc -ffreestanding -fno-builtin -O2 -c hello.c
i686-elf-ld -Ttext 0x0 -o hello.elf hello.o
objcopy -O binary hello.elf hello.bin
```

### Passo 3: Empacotar como .efh

Use a ferramenta `efh_pack` (incluir no kernel):

```bash
efh_pack -n hello -v 1.0 -a "Seu Nome" -d "Hello world" \
  hello.bin -o hello.efh
```

Ou manualmente em C:

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct efh_header {
    char magic[4];
    uint32_t version;
    uint32_t entry_offset;
    uint32_t code_size;
    uint32_t data_offset;
    uint32_t data_size;
    char name[32];
    char author[32];
    char desc[64];
};

int main() {
    FILE *bin = fopen("hello.bin", "rb");
    fseek(bin, 0, SEEK_END);
    uint32_t size = ftell(bin);
    fseek(bin, 0, SEEK_SET);
    
    struct efh_header hdr = {
        .magic = {'E', 'F', 'H', '\0'},
        .version = 1,
        .entry_offset = 256,
        .code_size = size,
        .data_offset = 0,
        .data_size = 0,
    };
    strncpy(hdr.name, "hello", 31);
    strncpy(hdr.author, "Your Name", 31);
    strncpy(hdr.desc, "Hello world app", 63);
    
    FILE *efh = fopen("hello.efh", "wb");
    fwrite(&hdr, sizeof(hdr), 1, efh);
    
    /* Pad to 256 bytes */
    char pad[256 - sizeof(hdr)] = {0};
    fwrite(pad, sizeof(pad), 1, efh);
    
    /* Write code */
    unsigned char buf[4096];
    size_t rd;
    while ((rd = fread(buf, 1, sizeof(buf), bin)) > 0) {
        fwrite(buf, 1, rd, efh);
    }
    
    fclose(bin);
    fclose(efh);
    return 0;
}
```

### Passo 4: Copiar para packages/

```bash
cp hello.efh /workspaces/KERNEL_C-/packages/
```

### Passo 5: Instalar no HubbleOS

```
hubble> hpm install hello
Installing hello... Done!

hubble> hello
Hello from HubbleOS!
```

## Adicionando um Novo Pacote ao HPM

Edite `src/hpm.c` e adicione à array `hpm_packages[]`:

```c
{"hello", "1.0", 8192, "https://github.com/seu-usuario/hello", "util", "libc"},
```

Recompile o kernel:

```bash
make clean && make
```

## Estendendo o Kernel

### Adicionar um Novo Driver

1. Criar `src/driver_xyz.c/h`
2. Adicionar ao Makefile: `KERNEL_OBJS+=src/driver_xyz.o`
3. Chamar `xyz_init()` em `kernel_main()`

Exemplo - Driver fictício:

```c
/* src/driver_rtc.c - Real-time clock */
#include <stdint.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

uint8_t rtc_read_register(uint8_t reg) {
    outb(CMOS_ADDR, reg);
    return inb(CMOS_DATA);
}

void rtc_init(void) {
    /* Initialize RTC */
}

uint8_t rtc_second(void) {
    return rtc_read_register(0x00);
}
```

### Adicionar um Novo Comando Shell

Em `src/kernel.c`, no `read_line()` loop:

```c
} else if (strncmp(line, "date", 4) == 0) {
    puts("Current time: ");
    itoa(rtc_second(), (char*)VGA, 10);
    puts(" seconds\n");
} else if (strncmp(line, "YOUR_CMD", 8) == 0) {
    /* Seu comando aqui */
```

### Adicionar uma Nova Syscall Simples

Criar um handler em `src/kernel.c`:

```c
int syscall_handler(int num, void *arg1, void *arg2) {
    switch (num) {
        case 1: /* sys_exit */
            while (1) __asm__ volatile ("hlt");
            break;
        case 2: /* sys_write */
            puts((const char*)arg1);
            break;
        default:
            return -1;
    }
    return 0;
}
```

## Compilando Pacotes Reais - Guia Detalhado

### Exemplo: Compilar TinyCC para HubbleOS

TinyCC é um compilador C muito pequeno (~20 KB) ideal para HubbleOS.

```bash
#!/bin/bash

cd build/packages

# Download
wget https://bellard.org/tcc/tcc-0.9.27.tar.bz2
tar xjf tcc-0.9.27.tar.bz2
cd tcc-0.9.27

# Configure para cross-compile
./configure \
  --cross-prefix=i686-elf- \
  --cc=i686-elf-gcc \
  --extra-cflags="-ffreestanding -O2" \
  --enable-static

# Patch (se necessário) para remover dependencies de libc
# ... (editar config.h)

# Compilar
make -j$(nproc)

# Testar
strip tcc

# Verificar tamanho
ls -lh tcc  # Deve ter ~2-5 MB

# Empacotar como .efh
# ... usar o script efh_pack.c
```

### Exemplo: Compilar BusyBox Mínimo

BusyBox é um multitool com centenas de comandos Unix.

```bash
#!/bin/bash

cd build/packages

# Download
wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar xjf busybox-1.36.1.tar.bz2
cd busybox-1.36.1

# Configure mínimo
make defconfig
make menuconfig  # Remover features grandes (vi, modprobe, etc)

# Compilar
make \
  CC=i686-elf-gcc \
  CFLAGS="-ffreestanding -O2 -static" \
  LDFLAGS="-static" \
  -j$(nproc)

# Resultado
strip busybox
ls -lh busybox  # Típicamente 1-3 MB com muitos applets
```

## Performance & Otimizações

### Reduzir Tamanho do Kernel

1. **Remover debug symbols**:
   ```bash
   strip kernel.bin
   ```

2. **Otimizar compilação**:
   ```makefile
   CFLAGS=-m32 -ffreestanding -O3 -ffunction-sections -fdata-sections
   LDFLAGS=-Wl,--gc-sections
   ```

3. **Remover drivers não-usados**:
   - Comentar em `kernel_main()` inits não-necessários

### Aumentar Velocidade de Boot

1. **Reduzir tamanho de HPM**: manter apenas pacotes essenciais
2. **Lazy-load drivers**: inicializar drivers sob demanda

## Testing & Debugging

### Executar em QEMU com Debug

```bash
# Terminal 1: QEMU com gdbstub
qemu-system-i386 -kernel kernel.bin -m 256 -s -S

# Terminal 2: GDB
i686-elf-gdb kernel.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
(gdb) next
```

### Logging Simples

Adicionar macro de debug:

```c
#define DEBUG 1
#define dbg_print(fmt, ...) \
    if (DEBUG) { printf(fmt, ##__VA_ARGS__); }
```

### Verificar Cobertura de Código

```bash
gcc -fprofile-arcs -ftest-coverage ...
# Após run em QEMU, gerar relatório
```

## Roadmap - Próximas Features

- [ ] **Preemptive Multitasking**: Task scheduler com timers
- [ ] **Virtual Memory**: Paging + page tables
- [ ] **POSIX APIs**: fork(), exec(), waitpid()
- [ ] **File System**: FAT12 ou ext2 simples
- [ ] **Network Stack**: IP/TCP/UDP básicos
- [ ] **Module System**: Carregamento dinâmico de drivers
- [ ] **Permission System**: UID/GID simples
- [ ] **IPC**: Pipes, sockets, message queues
- [ ] **Graphics Mode**: Framebuffer VGA/VESA

## Reporting Issues & Contributing

Formato de issue:

```
Title: [KERNEL/DRIVER/PACKAGE] Brief description

Description:
- What happened?
- Expected vs actual
- Reproducible steps

Environment:
- Architecture: x86_64 / i386
- QEMU version / Real hardware
- Kernel version

Logs/Traces:
- Boot output
- Error messages
```

## Licença & Attribution

Este projeto usa componentes de múltiplos projetos open-source:
- **GNU binutils/gcc**: GPL3
- **NASM**: Simplified BSD
- **Linux kernel** (conceitos): GPL2
- **OSDev tutorials**: CC-BY-SA

Para contribuir, mantenha compatibilidade de licença.

---

**Last Updated**: 2024-12-02
**Maintainer**: Ubuntuinformation
**Contact**: [GitHub Issues]
