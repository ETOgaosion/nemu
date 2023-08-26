/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/host.h>
#include <memory/vaddr.h>
#include <device/map.h>

#define IO_SPACE_MAX (2 * 1024 * 1024)

static uint8_t *io_space = NULL;
static uint8_t *p_space = NULL;

uint8_t* new_space(int size) {
  uint8_t *p = p_space;
  // page aligned;
  size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
  p_space += size;
  assert(p_space - io_space < IO_SPACE_MAX);
  return p;
}

static void check_bound(IOMap *map, paddr_t addr) {
  if (map == NULL) {
    Assert(map != NULL, "address (" FMT_PADDR ") is out of bound at pc = " FMT_WORD, addr, cpu.pc);
  } else {
    Assert(addr <= map->high && addr >= map->low,
        "address (" FMT_PADDR ") is out of bound {%s} [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
        addr, map->name, map->low, map->high, cpu.pc);
  }
}

static void invoke_callback(io_callback_t c, paddr_t offset, int len, bool is_write) {
  if (c != NULL) { c(offset, len, is_write); }
}

void init_map() {
  io_space = malloc(IO_SPACE_MAX);
  assert(io_space);
  p_space = io_space;
}

word_t map_read(Decode *s, paddr_t addr, int len, IOMap *map) {
  assert(len >= 1 && len <= 8);
  check_bound(map, addr);
  paddr_t offset = addr - map->low;
  invoke_callback(map->callback, offset, len, false); // prepare data to read
  word_t ret = host_read(map->space + offset, len);

#ifdef CONFIG_DTRACE
  if (s) {
    char *p = s->dtrace_logbuf;
    int max_log_len = sizeof(s->dtrace_logbuf) - 1;
    p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s\n", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
    p += snprintf(p, max_log_len, "access device %s at 0x%x, len: %d, data: 0x%lx", map->name, addr, len, ret);
  }
#endif
    
#ifdef CONFIG_DTRACE_COND
  if (DTRACE_COND) { dtrace_log_write("%s\n\n", s->dtrace_logbuf); }
#endif

  return ret;
}

void map_write(Decode *s, paddr_t addr, int len, word_t data, IOMap *map) {
  assert(len >= 1 && len <= 8);
  check_bound(map, addr);
  paddr_t offset = addr - map->low;
  host_write(map->space + offset, len, data);
  invoke_callback(map->callback, offset, len, true);

#ifdef CONFIG_DTRACE
  if (s) {
    char *p = s->dtrace_logbuf;
    int max_log_len = sizeof(s->dtrace_logbuf) - 1;
    p += snprintf(p, max_log_len, "[%lld] pc: 0x%lx, inst: 0x%x, instruction: %s\n", s->count, s->pc, s->isa.inst.val, s->isa.inst.name);
    p += snprintf(p, max_log_len, "access device %s at 0x%x, len: %d, data: 0x%lx", map->name, addr, len, data);
  }
#endif
    
#ifdef CONFIG_DTRACE_COND
  if (DTRACE_COND) { dtrace_log_write("%s\n\n", s->dtrace_logbuf); }
#endif
}
