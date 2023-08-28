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

#include "monitor/symbol.h"
#include <isa.h>
#include <memory/paddr.h>

void init_rand();
void init_log(const char *log_file, const char *itrace_log_file, const char *dtrace_log_file, const char *mtrace_log_file, const char *ftrace_log_file, const char *etrace_log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
    Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
    IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
                            "to record the trace. This may lead to a large log file. "
                            "If it is not necessary, you can disable it in menuconfig"));
    Log("Build time: %s, %s", __TIME__, __DATE__);
    printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
    printf("For help, type \"help\"\n");
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *itrace_log_file = NULL;
static char *dtrace_log_file = NULL;
static char *mtrace_log_file = NULL;
static char *ftrace_log_file = NULL;
static char *etrace_log_file = NULL;
static char *diff_so_file = NULL;
static char *elf_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
    if (img_file == NULL) {
        Log("No image is given. Use the default build-in image.");
        return 4096; // built-in image size
    }

    FILE *fp = fopen(img_file, "rb");
    Assert(fp, "Can not open '%s'", img_file);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    Log("The image is %s, size = %ld", img_file, size);

    fseek(fp, 0, SEEK_SET);
    int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return size;
}

static int parse_args(int argc, char *argv[]) {
    const struct option table[] = {
        {"batch", no_argument, NULL, 'b'}, {"log", required_argument, NULL, 'l'}, {"itracelog", required_argument, NULL, 'i'}, {"dtracelog", required_argument, NULL, 'v'}, {"mtracelog", required_argument, NULL, 'm'}, {"ftracelog", required_argument, NULL, 'f'}, {"etracelog", required_argument, NULL, 'x'}, {"diff", required_argument, NULL, 'd'}, {"port", required_argument, NULL, 'p'}, {"elf", required_argument, NULL, 'e'}, {"source", required_argument, NULL, 's'}, {"help", no_argument, NULL, 'h'}, {0, 0, NULL, 0},
    };
    int o;
    while ((o = getopt_long(argc, argv, "bhp:l:i:v:m:f:x:d:e:s:", table, NULL)) != -1) {
        switch (o) {
        case 'b':
            sdb_set_batch_mode();
            break;
        case 'p':
            sscanf(optarg, "%d", &difftest_port);
            break;
        case 'l':
            log_file = optarg;
            break;
        case 'i':
            itrace_log_file = optarg;
            break;
        case 'v':
            dtrace_log_file = optarg;
            break;
        case 'm':
            mtrace_log_file = optarg;
            break;
        case 'f':
            ftrace_log_file = optarg;
            break;
        case 'x':
            etrace_log_file = optarg;
            break;
        case 'd':
            diff_so_file = optarg;
            break;
        case 'e':
            elf_file = optarg;
            break;
        case 's':
            img_file = optarg;
            return 0;
        default:
            printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
            printf("\t-b,--batch                run with batch mode\n");
            printf("\t-l,--log=FILE             output log to FILE\n");
            printf("\t-i,--itracelog=FILE       output itrace log to FILE\n");
            printf("\t-v,--dtracelog=FILE       output dtrace loglog to FILE\n");
            printf("\t-m,--mtracelog=FILE       output mtrace loglog to FILE\n");
            printf("\t-f,--ftracelog=FILE       output ftrace loglog to FILE\n");
            printf("\t-x,--etracelog=FILE       output etrace loglog to FILE\n");
            printf("\t-d,--diff=REF_SO          run DiffTest with reference REF_SO\n");
            printf("\t-p,--port=PORT            run DiffTest with port PORT\n");
            printf("\t-h,--help                 help\n");
            printf("\t-e,--elf                  target elf file\n");
            printf("\t-s,--source               source image\n");
            printf("\n");
            exit(0);
        }
    }
    return 0;
}

void init_monitor(int argc, char *argv[]) {
    /* Perform some global initialization. */

    /* Parse arguments. */
    parse_args(argc, argv);

    /* Set random seed. */
    init_rand();

    /* Open the log file. */
    init_log(log_file, itrace_log_file, dtrace_log_file, mtrace_log_file, ftrace_log_file, etrace_log_file);

    /* Initialize memory. */
    init_mem();

    /* Initialize devices. */
    IFDEF(CONFIG_DEVICE, init_device());

    /* Perform ISA dependent initialization. */
    init_isa();

    /* Load the image to memory. This will overwrite the built-in image. */
    long img_size = load_img();

    /* Initialize differential testing. */
    init_difftest(diff_so_file, img_size, difftest_port);

    /* Initialize the simple debugger. */
    init_sdb();

    #ifdef CONFIG_FTRACE
    init_elf(elf_file);
    #endif

#ifndef CONFIG_ISA_loongarch32r
    IFDEF(CONFIG_ITRACE, init_disasm(MUXDEF(CONFIG_ISA_x86, "i686", MUXDEF(CONFIG_ISA_mips32, "mipsel", MUXDEF(CONFIG_ISA_riscv32, "riscv32", MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"));
#endif

    /* Display welcome message. */
    welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
    extern char bin_start, bin_end;
    size_t size = &bin_end - &bin_start;
    Log("img size = %ld", size);
    memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
    return size;
}

void am_init_monitor() {
    init_rand();
    init_mem();
    init_isa();
    load_img();
    IFDEF(CONFIG_DEVICE, init_device());
    welcome();
}
#endif
