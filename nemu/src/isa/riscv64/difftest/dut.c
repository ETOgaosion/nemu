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

#include "../local-include/reg.h"
#include <cpu/difftest.h>
#include <isa.h>

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
    bool ret = true;
    for (int i = 0; i < 32; i++) {
        if (ref_r->gpr[i] != gpr(i)) {
            Log("[DIFFTEST ERROR]: register %s value different, dut value: 0x%lx, ref value: 0x%lx", reg_name(i), gpr(i), ref_r->gpr[i]);
            ret = false;
        }
    }
    if (pc != cpu.last_pc) {
        Log("[DIFFTEST ERROR]: pc value different, dut value: 0x%lx, ref value: 0x%lx", cpu.last_pc, pc);
        ret = false;
    }
    return ret;
}

void isa_difftest_attach() {}
