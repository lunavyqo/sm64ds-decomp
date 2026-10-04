typedef struct C C;
extern unsigned char data_0209d45c[];
extern void UnloadOverlay(int id);
extern int overlay_100;
extern int overlay_102;

void func_ov075_02117bc4(C *c) {
    *data_0209d45c &= ~0xe;
    *(int*)((char*)c + 0x264) = 0x14;
    UnloadOverlay((int)&overlay_100);
    UnloadOverlay((int)&overlay_102);
}
