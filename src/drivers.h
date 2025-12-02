#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdint.h>

/* Simple disk driver (ATA) */
typedef struct {
    uint16_t base;
    int exists;
} ata_device_t;

void ata_init(void);
int ata_read(uint32_t lba, uint8_t *buf, int sectors);
int ata_write(uint32_t lba, const uint8_t *buf, int sectors);

/* Virtio network driver (minimal) */
void virtio_net_init(void);
int virtio_net_send(const uint8_t *data, uint32_t len);
int virtio_net_recv(uint8_t *buf, uint32_t max_len);

/* PCI enumeration */
void pci_enumerate(void);

#endif
