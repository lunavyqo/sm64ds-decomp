extern void func_020308d0(int n);
extern unsigned char data_0208a0e0[];

void SetNumPlayers(int n) {
    data_0208a0e0[0] = (unsigned char)n;
    func_020308d0(n);
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
#pragma explicit_zero_data on
unsigned char data_0208a0e0[4] = { 0x00, 0x00, 0x00, 0x00 };
#pragma explicit_zero_data off
#pragma section forcedata end
