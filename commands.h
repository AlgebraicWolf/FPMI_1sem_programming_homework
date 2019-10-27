/*DEF_CMD(PUSH, 1, 1, {})
DEF_CMD(POP, 2, 1, {})
DEF_CMD(ADD, 3, 0, {})
DEF_CMD(SUB, 4, 0, {})
DEF_CMD(MUL, 5, 0, {})
DEF_CMD(DIV, 6, 0, {})
DEF_CMD(END, 7, 0, {})
DEF_CMD(NOP, 0, 0, {})*/

DEF_CMD(push, 1,
        CMD_OVRLD(1, isdigit(*sarg)  || (*sarg == '-'), NUMBER, {
            arg = *((int *)(bin + 1)) * precision;
            push(&stk, arg);
            bin += sizeof(int);
        })
        CMD_OVRLD(11, isalpha(*sarg), REGISTER, {
            arg = *((int *)(bin + 1));
            if(arg >= 4) {
                printf(ANSI_COLOR_RED "Invalid register number %d. Terminating..." ANSI_COLOR_RESET, arg);
                exit(-1);
            }
            push(&stk, registers[arg]);
            bin += sizeof(int);
        }))


DEF_CMD(pop, 1,
        CMD_OVRLD(2, isalpha(*sarg), REGISTER, {
            arg = *((int *)(bin + 1));
            if(arg >= 4) {
                printf(ANSI_COLOR_RED "Invalid register number %d. Terminating..." ANSI_COLOR_RESET, arg);
                exit(-1);
            }
            registers[arg] = pop(&stk);
            bin += sizeof(int);
        })
        CMD_OVRLD(52, (*sarg == '[') && isdigit(*(sarg + 1)), RAM_IMMED, {

        })
        CMD_OVRLD(53, (*sarg == '[') && isalpha(*(sarg + 1)), RAM_REG, {

        })
        CMD_OVRLD(54, (*sarg == '[') && isalpha(*(sarg + 1)) && ((strchr(sarg, '-') != nullptr) || (strchr(sarg, '+') != nullptr)), RAM_REG_IMMED, {

        }))

DEF_CMD(add, 0,
        CMD_OVRLD(3, true, NONE, {
            push(&stk, pop(&stk) + pop(&stk));
        }))

DEF_CMD(sub, 0,
        CMD_OVRLD(4, true, NONE, {
            push(&stk, pop(&stk) - pop(&stk));
        }))

DEF_CMD(mul, 0,
        CMD_OVRLD(5, true, NONE, {
            push(&stk, pop(&stk) * pop(&stk) / 100);
        }))

DEF_CMD(div, 0,
        CMD_OVRLD(6, true, NONE, {
            int a = pop(&stk);
            int b = pop(&stk);

            if (b == 0) {
                printf(ANSI_COLOR_RED "Zero division error. Terminating...\n" ANSI_COLOR_RESET);
                exit(-1);
            }
            push(&stk, (int)((precision * a) / b));
        }))


DEF_CMD(end, 0,
        CMD_OVRLD(7, true, NONE, {
            bin = binStart + len - 1;
        }))

DEF_CMD(in, 0,
        CMD_OVRLD(8, true, NONE, {
            push(&stk, get_int(&stk) * precision);
}))

DEF_CMD(out, 0,
        CMD_OVRLD(9, true, NONE, {
            printf("%.2lf\n", (double) peak_n(&stk, 1) / precision);
        }))

DEF_CMD(nop, 0,
        CMD_OVRLD(0, true, NONE, {}))

DEF_CMD(call, 1,
        CMD_OVRLD(10, true, LABEL, {
            arg = *((int *)(bin + 1));
            if ((arg >= len) || (arg < 0)) {
                printf(ANSI_COLOR_RED "Calling function outside the program. Terminating..." ANSI_COLOR_RESET);
                exit(-1);
            }
            push(&stk, bin - binStart + 1);
            bin = binStart + arg - 1;
        })
        CMD_OVRLD(12, true, NUMBER, {
            arg = *((int *)(bin + 1));
            if ((arg >= len) || (arg < 0)) {
                printf(ANSI_COLOR_RED "Calling function outside the program. Terminating..." ANSI_COLOR_RESET);
                exit(-1);
            }
            push(&stk, bin - binStart + 1);
            bin = binStart + arg - 1;
        }))

DEF_CMD(ret, 0,
        CMD_OVRLD(13, true, NONE, {
            arg = pop(&stk);
            if((arg >= len) || (arg < 0)) {
                printf(ANSI_COLOR_RED "Returning outside the program. Terminating..." ANSI_COLOR_RESET);
                exit(-1);
            }
            bin = binStart + arg + sizeof(int) - 1;
        }))

DEF_CMD(sqrt, 0,
        CMD_OVRLD(14, true, NONE, {
            push(&stk, (int)round(sqrt((double) pop(&stk) / precision) * precision));
        }))


DEF_CMD(jmp, 1,
        CMD_OVRLD(20, isalpha(*sarg), LABEL, {
            arg = *((int *)(bin + 1));
            if ((arg >= len) || (arg < 0)) {
                printf(ANSI_COLOR_RED "Jumping outside the program. Terminating..." ANSI_COLOR_RESET);
                exit(-1);
            }
            bin = binStart + arg - 1;
        })
        CMD_OVRLD(21, isdigit(*sarg) || (*sarg == '-'), NUMBER, {
            arg = *((int *)(bin + 1));
            if ((arg >= len) || (arg < 0)) {
                printf(ANSI_COLOR_RED "Jumping outside the program. Terminating..." ANSI_COLOR_RESET);
                exit(-1);
            }
            bin = binStart + arg - 1;
        }))

#define DEF_JMP(name, opcode, cond) \
DEF_CMD(name, 1, \
        CMD_OVRLD(opcode, isalpha(*sarg), LABEL, { \
            if(peak_n(&stk, 1) cond peak_n(&stk, 2)) { \
                arg = *((int *)(bin + 1)); \
                if ((arg >= len) || (arg < 0)) { \
                    printf(ANSI_COLOR_RED "Jumping outside the program. Terminating..." ANSI_COLOR_RESET); \
                    exit(-1); \
                } \
                bin = binStart + arg - 1; \
            } \
            else bin += sizeof(int); \
        }) \
        CMD_OVRLD(opcode + 1, isdigit(*sarg) || (*sarg == '-'), NUMBER, { \
        if(peak_n(&stk, 1) cond peak_n(&stk, 2)) { \
                arg = *((int *)(bin + 1)); \
                if ((arg >= len) || (arg < 0)) { \
                    printf(ANSI_COLOR_RED "Jumping outside the program. Terminating..." ANSI_COLOR_RESET); \
                    exit(-1); \
                } \
                bin = binStart + arg - 1; \
            } \
            else bin += sizeof(int); \
        }))

DEF_JMP(ja, 22,>)
DEF_JMP(jae, 24, >=)
DEF_JMP(jb, 26, <)
DEF_JMP(jbe, 28, <=)
DEF_JMP(je, 30, ==)
DEF_JMP(jne, 32, !=)

#undef DEF_JMP

// Check non zero division
// nand2tetris.org
// Физтеховский курс по FPGA
// Вставить sleep при обращении к RAM
// Мб сделать кэш
