#include "drivers.h"

/* Minimal ATA driver stubs (for IDE disks) */
void ata_init(void) {
    /* Detect ATA controllers at ports 0x1F0 and 0x170 */
}

int ata_read(uint32_t lba, uint8_t *buf, int sectors) {
    /* Read from disk - simplified, just return 0 (success) for now */
    return 0;
}

int ata_write(uint32_t lba, const uint8_t *buf, int sectors) {
    return 0;
}

/* Minimal virtio-net driver stubs (for QEMU virtio network) */
void virtio_net_init(void) {
    /* Enumerate PCI and find virtio network device */
}

int virtio_net_send(const uint8_t *data, uint32_t len) {
    /* Send packet - stub */
    return len;
}

int virtio_net_recv(uint8_t *buf, uint32_t max_len) {
    /* Receive packet - stub, return 0 if none */
    return 0;
}

/* Minimal PCI enumeration */
void pci_enumerate(void) {
    /* Scan PCI bus for devices */
}
