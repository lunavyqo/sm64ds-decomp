extern void *data_020a8060;
extern int data_020a805c;
extern int data_020a8064[2];
extern int data_020a806c[2];
extern int data_020a8074;
extern int data_0209a048;
extern int func_0205da94;
extern int func_0205db2c;
extern int func_0205db24;
extern int func_02057020(void);
extern void func_02060890(void);
extern void func_0205cd34(void *o);
extern int func_0205cc80(void *thiz, void *a, void *b);
extern void func_0205c7c4(int *p, int a, int b);
extern int func_0205cb68(int *self, int a1, int a2, int a3, int a4, int a5, int a6, int a7);

#define A ((int *)0x27ffe40)
#define B ((int *)0x27ffe48)

#pragma opt_propagation off
void func_0205d96c(void *arg)
{
    int *a;
    int *b;

    data_020a8060 = arg;
    data_020a805c = func_02057020();
    data_020a8064[0] = 0;
    data_020a8064[1] = 0;
    data_020a806c[0] = 0;
    data_020a806c[1] = 0;
    func_02060890();

    a = A;
    b = B;
    func_0205cd34(&data_020a8074);
    func_0205cc80(&data_020a8074, &data_0209a048, (void *)3);
    if (a[0] == -1) return;
    if (a[0] == 0) return;
    if (b[0] == -1) return;
    if (b[0] == 0) return;
    func_0205c7c4(&data_020a8074, (int)&func_0205da94, 0x602);
    func_0205cb68(&data_020a8074, 0, b[0], b[1], a[0], a[1],
                  (int)&func_0205db2c, (int)&func_0205db24);
}

extern void func_0205ffb0(unsigned short c);
extern void func_0205ff90(unsigned short t);
extern unsigned int data_020a805c;

int func_0205da94(int a, int b) {
    switch (b) {
    case 9:
        func_0205ffb0((unsigned short)data_020a805c);
        return 0;
    case 0xa:
        func_0205ff90((unsigned short)data_020a805c);
        return 0;
    case 1:
        return 4;
    }
    return 8;
}

int func_0205db24(void) {
    return 1;
}

/* func_0205db2c at 0x0205db2c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern unsigned int data_020a8060;
extern int func_0205db78(void);
extern int func_02060918(int a0, int a1, int a2, int a3, void *a4, int a5, int a6);

int func_0205db2c(int a, int b, int c, int d) {
    func_02060918(data_020a8060, c, b, d, (void *)&func_0205db78, a, 1);
    return 6;
}

extern void func_0205cd5c(void *, int);
void func_0205db78(void *a) {
    func_0205cd5c(a, 0);
}
