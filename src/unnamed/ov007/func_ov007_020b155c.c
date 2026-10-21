extern void RequestScreenFade(int a, int b, void* p);
extern void func_ov007_020c1d8c(int i);
extern char data_ov007_0210342c[];
extern char func_ov007_020b1604[];

void func_ov007_020b155c(void) {
  char* g = *(char**)data_ov007_0210342c;
  if (*(int*)(*(char**)(g+8) + 0xc) != 0) return;
  {
    int v = *(int*)(g+0x10);
    if (v == 8) {
      RequestScreenFade(1, 0, func_ov007_020b1604);
    } else {
      if ((unsigned int)(v - 0xa) <= 1) {
        RequestScreenFade(2, 0, func_ov007_020b1604);
      } else {
        RequestScreenFade(1, 0, func_ov007_020b1604);
      }
      (*(void(**)(int))(*(char**)data_ov007_0210342c + 0x2c))(0x13);
    }
  }
  func_ov007_020c1d8c(0);
}

extern void func_ov007_020af34c(void);
extern void *data_ov007_0210342c;
void func_ov007_020b1604(void) {
    char *r2 = (char *)data_ov007_0210342c;
    int r1 = *(int *)(r2 + 0x10);
    if (r1 == 9) {
        *(int *)(r2 + 0x14) = 0;
        *(int *)((char *)data_ov007_0210342c + 0x10) = 0;
        func_ov007_020af34c();
        return;
    }
    if (r1 == 8) {
        *(short *)(*(char **)(r2 + 4) + 2) = 6;
    } else {
        *(short *)(*(char **)(r2 + 8) + 2) = 7;
    }
}
