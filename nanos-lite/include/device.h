#pragma once

size_t null_read(void *buf, size_t offset, size_t len);
size_t null_write(const void *buf, size_t offset, size_t len);
size_t zero_read(void *buf, size_t offset, size_t len);
size_t zero_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);