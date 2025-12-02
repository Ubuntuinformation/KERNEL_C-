# HubbleOS v2.0 - Quick Start

## Build & Test em 30 segundos

### 1️⃣ Compilar
```bash
cd /workspaces/KERNEL_C-
make clean && make
```

### 2️⃣ Verificar
```bash
ls -lh kernel.bin
# Deve mostrar: -rwxrwxrwx 1 ... 56K ... kernel.bin
```

### 3️⃣ Teste Rápido
```bash
bash test_commands.sh
```

---

## Comandos Principais

### 📂 Arquivos
```
ls                    # Listar
touch file.txt        # Criar
cat file.txt          # Ver
rm file.txt           # Deletar
mv old.txt new.txt    # Renomear
cp original copy      # Copiar
chmod 755 file        # Permissões
```

### 📁 Diretórios
```
pwd                   # Diretório atual
cd mydir              # Entrar
mkdir mydir           # Criar
rmdir mydir           # Deletar
cd ..                 # Voltar
```

### 📦 Pacotes
```
hpm list              # Ver todos os 150+ pacotes
hpm cat dev           # Ver categoria dev
hpm search gcc        # Buscar gcc
hpm info gcc          # Info do gcc
hpm download gcc      # Baixar gcc
hpm convert gcc       # Converter para .efh
hpm install gcc       # Instalar
hpm installed         # Ver instalados
hpm cache             # Ver downloads
```

### ⚙️ Sistema
```
echo Hello            # Imprimir
help                  # Ajuda
clear                 # Limpar tela
pwd                   # Dir. atual
reboot                # Reiniciar
exit                  # Desligar
```

---

## Workflow Completo: GCC

```bash
hubble> hpm info gcc
# Ver informações

hubble> hpm download gcc
# Downloading gcc... Done! (258 bytes)

hubble> hpm convert gcc
# Converting gcc to .efh... Done!

hubble> ls *.efh
# [FILE] gcc.efh (258 bytes)

hubble> hpm install gcc
# Installing gcc... Done!

hubble> hpm installed
# Installed Packages:
#   gcc 12.2.0
```

---

## Em QEMU (opcional)

```bash
make run
# Abre QEMU com o kernel

# No shell do QEMU:
hubble> help
hubble> hpm list
hubble> exit
```

---

## 📚 Documentação

| Arquivo | Conteúdo |
|---------|----------|
| `COMMANDS_REFERENCE.md` | Guia completo de todos os comandos |
| `DOWNLOAD_CONVERSION_FLOW.md` | Como funciona download e .efh |
| `COMPLETE_GUIDE.md` | Setup completo |
| `PACKAGES_CATALOG.md` | Lista dos 150+ pacotes |
| `DEVELOPMENT.md` | Como contribuir |
| `FINAL_SUMMARY.md` | Visão geral do projeto |

---

## ✅ Feito!

- ✅ 1,470 linhas de código
- ✅ 56 KB kernel compilado
- ✅ 30+ comandos
- ✅ 150+ pacotes
- ✅ Sistema completo de download/conversão .efh
- ✅ Parser de argumentos
- ✅ Virtual filesystem com diretórios
- ✅ 8 documentações

**Tudo pronto para usar!** 🚀
