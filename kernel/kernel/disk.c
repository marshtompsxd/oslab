#include "x86.h"
#include "device.h"

void waitDisk(void) {
	while((inByte(0x1F7) & 0xC0) != 0x40); 
}

void readSect(void *dst, int offset) {
	int i;
	waitDisk();
	
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}

void readBytes(uint8_t* addr, int diskoff, int cont)
{
	uint8_t* addr_end = addr + cont;
	int section;
	
	// addr step back
	addr -= diskoff%SECTSIZE;
	section = (diskoff/SECTSIZE);
	for(; addr < addr_end; addr+=SECTSIZE,section++)
		readSect(addr, section);
}

void writeSect(void *src, int offset) {
    int i;
    waitDisk();

    outByte(0x1F2, 1);
    outByte(0x1F3, offset);
    outByte(0x1F4, offset >> 8);
    outByte(0x1F5, offset >> 16);
    outByte(0x1F6, (offset >> 24) | 0xE0);
    outByte(0x1F7, 0x30);

    waitDisk();
    for (i = 0; i < SECTSIZE / 4; i ++) {
        outLong(0x1F0, ((uint32_t *)src)[i]);
    }
}

void writeBytes(uint8_t* addr, int diskoff, int cont)
{
	uint8_t* addr_end = addr + cont;
	int section;
	
	// addr step back
	assert(diskoff%SECTSIZE == 0);
	diskoff -= diskoff%SECTSIZE;
	section = (diskoff/SECTSIZE);
	for(; addr < addr_end; addr+=SECTSIZE,section++)
		writeSect(addr, section);
}