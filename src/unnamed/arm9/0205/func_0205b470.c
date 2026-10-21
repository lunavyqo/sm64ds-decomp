struct E {
    void (*fn)(int);
    int arg;
    unsigned char tag;
};

extern struct E data_020a7f60[];

void func_0205b470(int arg)
{
    struct E *e = &data_020a7f60[arg & 0xff];
    if (((arg >> 8) & 0xff) != e->tag)
        return;
    if (e->fn == 0)
        return;
    e->fn(e->arg);
}

struct E7f60 {
    void (*fn)(int);
    int arg;
    unsigned char tag;
};

extern struct E7f60 data_020a7f60[];

unsigned char func_0205b4d0(int index, void (*fn)(int), int arg) {
    struct E7f60 *e = &data_020a7f60[index];
    unsigned char *t = (unsigned char *)((char *)e + 8);
    data_020a7f60[index].fn = fn;
    e->arg = arg;
    (*t)++;
    return e->tag;
}

typedef struct UnkStruct_020a7f60 {
    char pad0[8];
    unsigned char unk8;
    char pad9[3];
} UnkStruct_020a7f60;

extern UnkStruct_020a7f60 data_020a7f60[];

void func_0205b504(int idx) {
    *(unsigned char *)((long long)((int)&data_020a7f60[idx] + 8)) += 1;
}

struct E7f60 {
    int a;
    int b;
    unsigned char c;
};
extern struct E7f60 data_020a7f60[8];

void func_0205b524(void) {
    int i;
    for (i = 0; i < 8; i++) {
        data_020a7f60[i].a = 0;
        data_020a7f60[i].b = 0;
        data_020a7f60[i].c = 0;
    }
}
