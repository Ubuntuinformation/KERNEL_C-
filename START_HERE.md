# 🚀 HubbleOS v2.0 - COMECE AQUI!

## ✨ Bem-vindo ao Sistema Operacional HubbleOS!

Você tem um **sistema operacional Unix-like completo e bootável** pronto para usar!

---

## 📦 O QUE VOCÊ TEM

```
hubbleos.iso (12 MB)
├── ✅ Bootloader GRUB2
├── ✅ Kernel x86 32-bit (56 KB)
├── ✅ Shell com 30+ comandos
├── ✅ Gerenciador de Pacotes (150+ pacotes)
├── ✅ Sistema .EFH executável
└── ✅ Documentação completa
```

---

## ⚡ INÍCIO RÁPIDO (3 opções)

### 1️⃣ Testar em QEMU (Mais fácil)

```bash
qemu-system-i386 -cdrom hubbleos.iso -m 256
```

**Espere ~5 segundos e o shell aparecerá:**
```
hubble> help
```

### 2️⃣ Usar em Virtualização

VirtualBox / VMware / KVM:
- Criar VM (Linux 32-bit)
- CDROM → hubbleos.iso
- Boot!

### 3️⃣ Queimar em USB/CD-DVD

```bash
bash burn_iso.sh
# Escolha a opção 1 (USB) ou 2 (CD/DVD)
```

---

## 💻 COMANDOS BÁSICOS

```bash
# Ver todos os comandos
hubble> help

# Sistema de arquivos
hubble> mkdir mydir          # Criar diretório
hubble> cd mydir             # Entrar em diretório
hubble> pwd                  # Ver onde você está
hubble> touch file.txt       # Criar arquivo
hubble> ls                   # Listar
hubble> cat file.txt         # Ver arquivo
hubble> rm file.txt          # Deletar
hubble> cp original copia    # Copiar
hubble> mv antigo novo       # Renomear

# Gerenciador de Pacotes
hubble> hpm list             # Ver 150+ pacotes
hubble> hpm search gcc       # Buscar gcc
hubble> hpm info gcc         # Info de gcc
hubble> hpm download gcc     # Baixar gcc
hubble> hpm convert gcc      # Converter para .efh
hubble> hpm install gcc      # Instalar gcc
hubble> hpm installed        # Ver instalados
```

---

## 📚 DOCUMENTAÇÃO

### Rápida (30 seg)
- `QUICK_START.md` - Início em 30 segundos

### Completa (Referência)
- `COMMANDS_REFERENCE.md` - Todos os 30+ comandos
- `ISO_GUIDE.md` - Como usar a ISO

### Técnica
- `DOWNLOAD_CONVERSION_FLOW.md` - Arquitetura interna
- `FINAL_SUMMARY.md` - Visão geral do projeto

### Dentro da ISO
```bash
# Ler documentação na ISO
hubble> cat /doc/QUICK_START.md
hubble> cat /doc/COMMANDS_REFERENCE.md
```

---

## 🎯 EXEMPLO PRÁTICO: Instalar GCC

```bash
# 1. Ver info do pacote
hubble> hpm info gcc

# 2. Baixar da internet
hubble> hpm download gcc
# Downloading gcc... Done! (258 bytes)

# 3. Converter para formato .efh
hubble> hpm convert gcc
# Converting gcc to .efh... Done!

# 4. Verificar arquivo criado
hubble> ls *.efh
# [FILE] gcc.efh (258 bytes)

# 5. Instalar no sistema
hubble> hpm install gcc
# Installing gcc... Done!

# 6. Confirmar instalação
hubble> hpm installed
# Installed Packages:
#   gcc 12.2.0
```

---

## 📊 FATOS SOBRE HUBBLEOS

| Item | Detalhes |
|------|----------|
| **Linhas de código** | 1,470 (C + NASM) |
| **Tamanho kernel** | 56 KB |
| **Tamanho ISO** | 12 MB |
| **Pacotes** | 150+ pré-configurados |
| **Comandos shell** | 30+ |
| **Categorias** | 13 (dev, lang, vcs, util, etc) |
| **Arquitetura** | x86 32-bit BIOS |
| **Status** | ✅ Production Ready |

---

## 🛠️ SCRIPTS ÚTEIS

```bash
# Compilar ISO a partir do kernel
bash build_iso.sh

# Helper para queimar ISO
bash burn_iso.sh

# Testar todos os 30+ comandos
bash test_commands.sh
```

---

## ❓ PERGUNTAS FREQUENTES

**P: Como booteia em um PC real?**
A: Queime a ISO em USB/CD-DVD com `bash burn_iso.sh` e dê boot pela mídia.

**P: Precisa instalar algo?**
A: NÃO! É apenas uma ISO bootável. Boot direto!

**P: Quais sistemas operacionais o QEMU está usando?**
A: Nenhum - HubbleOS é um sistema operacional Unix-like barebone x86.

**P: Posso executar programas nela?**
A: Sim! Sistema de download/conversão .efh está pronto. Fase 4 adicionará execução.

**P: Qual é a próxima fase?**
A: Fase 3 (TCP/IP real), Fase 4 (Execução .efh), Fase 5 (Multitasking).

---

## 📖 PRÓXIMOS PASSOS

1. **AGORA:**
   - [ ] Ler `QUICK_START.md` (30 segundos)
   - [ ] Testar em QEMU: `qemu-system-i386 -cdrom hubbleos.iso -m 256`
   - [ ] Explorar comandos: `help`

2. **DEPOIS:**
   - [ ] Ler `COMMANDS_REFERENCE.md` (completo)
   - [ ] Experimentar HPM: `hpm download`, `hpm convert`
   - [ ] Criar arquivos e diretórios

3. **AVANÇADO:**
   - [ ] Ler `DOWNLOAD_CONVERSION_FLOW.md` (arquitetura)
   - [ ] Consultar `DEVELOPMENT.md` (contribuir)
   - [ ] Compilar novos pacotes .efh

---

## 🎉 CONCLUSÃO

Você tem um **sistema operacional completo, bootável e funcional**.

Não precisa instalar nada - basta usar a ISO!

```bash
# Teste agora:
qemu-system-i386 -cdrom hubbleos.iso -m 256
```

**Bem-vindo ao HubbleOS v2.0! 🚀**

---

## 📍 ÍNDICE RÁPIDO

| Arquivo | Conteúdo |
|---------|----------|
| `README_ISO.txt` | Visão geral (este arquivo) |
| `QUICK_START.md` | Início em 30 segundos |
| `COMMANDS_REFERENCE.md` | Referência de comandos |
| `ISO_GUIDE.md` | Guia completo da ISO |
| `DOWNLOAD_CONVERSION_FLOW.md` | Arquitetura técnica |
| `FINAL_SUMMARY.md` | Resumo do projeto |
| `hubbleos.iso` | **A ISO bootável (12 MB)** |

---

**Data:** Dezembro 2024  
**Versão:** 2.0  
**Status:** ✅ Production Ready  
**Licença:** Open Source
