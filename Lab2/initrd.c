#include "initrd.h"
#include "utils.h"

#ifdef __QEMU__
#define CPIO_ADDR 0x8000000
#else
#define CPIO_ADDR 0x21000000
#endif

void* cpio_base = (void*)(CPIO_ADDR);

int pad4(int x){
    return (4-(x&3))&3;
}

char* next(const char* now){
    if(strncmp(now+sizeof(struct cpio_newc_header), "TRAILER!!!", 10) == 0)
        return 0;
    struct cpio_newc_header* header = (struct cpio_newc_header*)now;
    int namesz = HtoI(header->c_namesize, 8);
    int datasz = HtoI(header->c_filesize, 8);
    int headsz = sizeof(struct cpio_newc_header)+namesz;
    headsz += pad4(headsz);
    datasz += pad4(datasz);
    return (char*)(now+headsz+datasz);
}

void initrd_list(){
    char* now = cpio_base;
    const char* nxt = 0;
    while((nxt = next(now)) != 0){
        struct cpio_newc_header* header = (struct cpio_newc_header*)now;
        if(strncmp(header->c_magic, "070701", 6)){
            uart_puts("ERRO: File System CORRUPTED!\n");
            break;
        }
        int namesz = HtoI(header->c_namesize, 8);
        char* tmp = now+sizeof(struct cpio_newc_header);
        for(int i = 0;i < namesz;i++){
            uart_putc(*tmp);
            tmp++;
        }
        uart_putc('\n');
        now = nxt;
    }
}

void test(){
    char* tmp = cpio_base;
    char c;
    while((c = uart_getc()) != '0'){
        uart_putc(*tmp);
        tmp++;
    }
}
