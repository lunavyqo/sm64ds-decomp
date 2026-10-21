extern int func_0205ff20(void);
extern void func_0205f8b0(int param);
extern void func_0205f68c(int a, int b, int c, int d);
extern volatile int data_020a8108;
extern int data_020a810c;

int func_0205fc44(unsigned int a, unsigned int b)
{
    volatile int *p;

    if (!func_0205ff20()) return 1;
    data_020a8108 = 0;
    func_0205f8b0(0x3006000);
    p = &data_020a8108;
    while (!*p) { }
    *p = 0;
    data_020a810c = 0;
    func_0205f68c(0, 2, 0, 0);
    func_0205f8b0((a & 0xff) | 0x2006100);
    func_0205f8b0((b & 0xffff) | 0x1010000);
    return 0;
}


extern void func_0205feac(int a);

typedef struct {
    short *ptr;
    int pad;
} PtrEntry;
extern PtrEntry data_020a813c[];

typedef struct {
    unsigned short flag;
    short pad;
    int pad2;
} FlagEntry;
extern FlagEntry data_020a8138[];

extern int data_020a8108;
extern int data_020a810c;

typedef struct {
    char pad[0xc];
    int *fc;
} StructH;
extern StructH data_020a8114;

void func_0205fcfc(int a0, unsigned int msg, int flag)
{
    unsigned short cmd;
    unsigned short val;

    if (flag != 0) {
        func_0205feac(2);
        return;
    }

    cmd = (msg & 0x7f00) >> 8;
    val = (msg & 0xff) << 16 >> 16;

    if (cmd >= 0x70 && cmd <= 0x73) {
        int idx = cmd - 0x70;
        unsigned short pv = val & 0xff;
        short *p = data_020a813c[idx].ptr;
        if (p != 0) {
            *p = pv;
        }
        data_020a8138[idx].flag = 1;
        val = 0;
    } else if (cmd == 0x60) {
        data_020a8108 = 1;
    } else if (cmd == 0x62) {
        data_020a810c = 1;
    } else if (cmd == 0x67) {
        int *q = data_020a8114.fc;
        if (q != 0) {
            *q = val;
        }
        val = 0;
    }

    func_0205feac(val);
}
