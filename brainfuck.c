#include <stdio.h>

#define opEnd         0
#define opIncDp       1
#define opDecDp       2
#define opIncVal      3
#define opDecVal      4
#define opOut          5
#define opIn           6
#define opJmpFwd      7
#define opJmpBck      8

#define sucesso         0
#define falhou         1

#define tamanhoApp    4096
#define tamanhoPilha      512
#define tamanhoDado       65535

#define STACK_PUSH(A)   (STACK[SP++] = A)
#define STACK_POP()     (STACK[--SP])
#define STACK_EMPTY()   (SP == 0)
#define STACK_FULL()    (SP == tamanhoPilha)

struct instruction_t {
    unsigned short operador;
    unsigned short operando;
};

static struct instruction_t APP[tamanhoApp];
static unsigned short STACK[tamanhoPilha];
static unsigned int SP = 0;

int compile_bf(FILE* fp) {
    unsigned short pc = 0, jmp_pc;
    int c;
    while ((c = getc(fp)) != EOF && pc < tamanhoApp) {
        switch (c) {
            case '>': APP[pc].operador = opIncDp; break;
            case '<': APP[pc].operador = opDecDp; break;
            case '+': APP[pc].operador = opIncVal; break;
            case '-': APP[pc].operador = opDecVal; break;
            case '.': APP[pc].operador = opOut; break;
            case ',': APP[pc].operador = opIn; break;
            case '[':
                APP[pc].operador = opJmpFwd;
                if (STACK_FULL()) {
                    return falhou;
                }
                STACK_PUSH(pc);
                break;
            case ']':
                if (STACK_EMPTY()) {
                    return falhou;
                }
                jmp_pc = STACK_POP();
                APP[pc].operador =  opJmpBck;
                APP[pc].operando = jmp_pc;
                APP[jmp_pc].operando = pc;
                break;
            default: pc--; break;
        }
        pc++;
    }
    if (!STACK_EMPTY() || pc == tamanhoApp) {
        return falhou;
    }
    APP[pc].operador = OP_END;
    return sucesso;
}

int execute_bf() {
    unsigned short dado[tamanhoDado], pc = 0;
    unsigned int ptr = tamanhoDado;
    while (--ptr) { dado[ptr] = 0; }
    while (APP[pc].operador != opEnd&& ptr < tamanhoDado) {
        switch (APP[pc].operador) {
            case opIncDp: ptr++; break;
            case opDecDp: ptr--; break;
            case opIncVal: dado[ptr]++; break;
            case opDecVal: dado[ptr]--; break;
            case opOut: putchar(dado[ptr]); break;
            case opIn: dado[ptr] = (unsigned int)getchar(); break;
            case opJmpFwd: if(!dado[ptr]) { pc = APP[pc].operando; } break;
            case opJmpBck: if(dado[ptr]) { pc = APP[pc].operando; } break;
            default: return falhou;
        }
        pc++;
    }
    return ptr != tamanhoDado ? sucesso : falhou;
}

int main(int argc, const char * argv[])
{
    int status;
    FILE *fp;
    if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Arquivo: %s\n", argv[0]);
        return falhou;
    }
    status = compile_bf(fp);
    fclose(fp);
    if (status == sucesso) {
        status = execute_bf();
    }
    if (status == falhou) {
        fprintf(stderr, "Erro!\n");
    }
    return status;
}
