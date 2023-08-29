#include <fs.h>
#include <klib.h>
#include <am.h>
#include <device.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
    char *name;
    size_t size;
    size_t disk_offset;
    size_t open_offset;
    ReadFn read_func;
    WriteFn write_func;
    bool char_dev;
} Finfo;

enum {
    FD_STDIN,
    FD_STDOUT,
    FD_STDERR,
    FD_FB
};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin"},
    [FD_STDOUT] = {"stdout", 0, 0, 0, 0, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, 0, 0, serial_write},
#include "files.h"
    {"/dev/null", 0, 0, 0, null_read, null_write},
    {"/dev/zero", 0, 0, 0, zero_read, zero_write},
    {"/dev/events", 0, 0, 0, events_read, 0, true},
    {"/dev/fb", 0, 0, 0, 0, fb_write},
    {"/proc/dispinfo", 0, 0, 0, dispinfo_read, 0, true},
};

static const int file_table_size = sizeof(file_table)/sizeof(Finfo);

void init_fs() {
    // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
    for (int i = FD_FB; i < file_table_size; i++) {
        if (strcmp(file_table[i].name, pathname) == 0) {
            file_table[i].open_offset = 0;
            return i;
        }
    }
    return -1;
}


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t fs_read(int fd, void *buf, size_t len) {
    Assert(fd >= 0 && fd < file_table_size, "[fs read] error fd, fd: %d, buf: 0x%p, len: %ld", fd, buf, len);
    size_t ret = 0;
    switch(fd) {
        case FD_STDIN:
        case FD_STDOUT:
        case FD_STDERR:
            panic("try read std I/O, fd: %d, buf: 0x%p, len: %ld", fd, buf, len);
            break;
        default:
            if (file_table[fd].read_func) {
                ret = file_table[fd].read_func(buf, file_table[fd].open_offset, len);
            }
            else {
                if (file_table[fd].open_offset < file_table[fd].size) {
                    if (file_table[fd].open_offset + len >= file_table[fd].size) {
                        len = file_table[fd].size - file_table[fd].open_offset;
                    }
                    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
                    file_table[fd].open_offset += len;
                    ret = len;
                }
            }
            break;
    }
    return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
    Assert(fd >= 0 && fd < file_table_size, "[fs write] error fd, fd: %d, buf: 0x%p, len: %ld", fd, buf, len);
    size_t ret = 0;
    switch(fd) {
        case FD_STDIN:
            panic("try write stdin, fd: %d, buf: 0x%p, len: %ld", fd, buf, len);
            break;
        case FD_STDOUT:
        case FD_STDERR:
            if (file_table[fd].write_func) {
                ret = file_table[fd].write_func(buf, 0, len);
            }
            else {
                panic("please use write_func");
            }
            break;
        default:
            if (file_table[fd].write_func) {
                ret = file_table[fd].write_func(buf, file_table[fd].open_offset, len);
            }
            else {
                if (file_table[fd].open_offset < file_table[fd].size) {
                    if (file_table[fd].open_offset + len >= file_table[fd].size) {
                        len = file_table[fd].size - file_table[fd].open_offset;
                    }
                    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
                    file_table[fd].open_offset += len;
                    ret = len;
                }
            }
            break;
    }
    return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
    Assert(fd >= 0 && fd < file_table_size, "[fs lseek] error fd, fd: %d, offset: 0x%ld, whence: %d", fd, offset, whence);
    size_t ret = 0;
    if (file_table[fd].char_dev) {
        panic("lseek not support for char device");
        return 0;
    }
    switch(fd) {
        case FD_STDIN:
        case FD_STDOUT:
        case FD_STDERR:
            panic("try lseek std I/O, fd: %d, offset: 0x%ld, whence: %d", fd, offset, whence);
            break;
        default: {
            switch (whence)
            {
            case SEEK_SET:
                file_table[fd].open_offset = offset;
                break;
            case SEEK_CUR:
                file_table[fd].open_offset += offset;
                break;
            case SEEK_END:
                file_table[fd].open_offset = file_table[fd].size;
                break;
            
            default:
                break;
            }
            ret = file_table[fd].open_offset;
        }
            break;
    }
    return ret;
}

int fs_close(int fd) {
    Assert(fd >= 0 && fd < file_table_size, "[fs close] error fd, fd: %d", fd);
    size_t ret = 0;
    file_table[fd].open_offset = 0;
    return ret;
}

char *get_file_name(int fd) {
    return file_table[fd].name;
}
