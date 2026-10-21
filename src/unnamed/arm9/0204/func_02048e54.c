extern int data_02099fb0;
void _ZN5Sound6Player19SetPlayableSeqCountEii(int a, int b);

void func_02048e54(void) {
    data_02099fb0 = 4;
    _ZN5Sound6Player19SetPlayableSeqCountEii(9, 4);
}


typedef int s32;

extern void _ZN5Sound6Player19SetPlayableSeqCountEii(s32 playerID, s32 maxSeq);
extern s32 data_02099fb0;

void func_02048e74(void)
{
    data_02099fb0 = 4;
    _ZN5Sound6Player19SetPlayableSeqCountEii(9, 4);
}


extern int _ZN5Sound6Player19SetPlayableSeqCountEii(int a, int b);
extern int data_02099fb0;

int func_02048e94(void) {
    data_02099fb0 = 4;
    return _ZN5Sound6Player19SetPlayableSeqCountEii(9, 4);
}
