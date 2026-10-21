#include "types.h"
#include "nitro/fx/fx.h"
extern void func_ov007_020c6784(void *self, int count, const int *p2, const void *p3,
                                const void *p4, const int *const *p5, int mode);

/* The portrait tables live in their own files. Their addresses are earlier in
 * .rodata and .data than this function, so one translation unit cannot own both. */
extern const int sPortraitStrokeCounts[6];
extern const int *sPortraitStrokePointCounts[6];
extern const unsigned char *const *sPortraitStrokeXCoords[6];
extern const unsigned char *const *sPortraitStrokeYCoords[6];
extern const int *const *sPortraitStrokeProgressThresholds[6];

extern char *data_ov007_0210342c;
extern char *data_ov007_02104ba0;

void func_ov007_020bbe60(int idx) {
    int drawing = idx * 3 + *(u8 *)(data_ov007_0210342c + idx + 0xfc);
    func_ov007_020c6784(*(void **)data_ov007_02104ba0, sPortraitStrokeCounts[drawing],
                        sPortraitStrokePointCounts[drawing], sPortraitStrokeXCoords[drawing],
                        sPortraitStrokeYCoords[drawing], sPortraitStrokeProgressThresholds[drawing],
                        1);
    *(int *)(data_ov007_02104ba0 + 0x38) = 0x78;
    if (idx == 0 && *(u8 *)(data_ov007_0210342c + idx + 0xfc) == 2) {
        int *o = (int *)data_ov007_02104ba0;
        int mid = FX_MUL((o[0x38 / 4] << 12), 0x1800);
        o[0x38 / 4] = mid >> 12;
    }
    ((u8 *)(data_ov007_0210342c + 0xfc))[idx]++;
    {
        u8 *p = (u8 *)(data_ov007_0210342c + 0xfc);
        if (p[idx] >= 3)
            p[idx] = 0;
    }
}
