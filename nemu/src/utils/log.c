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

#include <common.h>

extern uint64_t g_nr_guest_inst;
FILE *log_fp = NULL;
FILE *itrace_log_fp = NULL;
FILE *mtrace_log_fp = NULL;
FILE *ftrace_log_fp = NULL;

void init_log(const char *log_file, const char *itrace_log_file, const char *mtrace_log_file, const char *ftrace_log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
  itrace_log_fp = stdout;
  if (itrace_log_file != NULL) {
    FILE *fp = fopen(itrace_log_file, "w");
    Assert(fp, "Can not open '%s'", itrace_log_file);
    itrace_log_fp = fp;
  }
  Log("Itrace log is written to %s", itrace_log_file ? itrace_log_file : "stdout");
  mtrace_log_fp = stdout;
  if (mtrace_log_file != NULL) {
    FILE *fp = fopen(mtrace_log_file, "w");
    Assert(fp, "Can not open '%s'", mtrace_log_file);
    mtrace_log_fp = fp;
  }
  Log("Mtrace log is written to %s", mtrace_log_file ? mtrace_log_file : "stdout");
  ftrace_log_fp = stdout;
  if (ftrace_log_file != NULL) {
    FILE *fp = fopen(ftrace_log_file, "w");
    Assert(fp, "Can not open '%s'", ftrace_log_file);
    ftrace_log_fp = fp;
  }
  Log("Ftrace log is written to %s", ftrace_log_file ? ftrace_log_file : "stdout");
}

bool log_enable() {
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}
