extern int data_0209fc48;
extern int data_020889b0[];
int func_0200ee68(void){
return data_0209fc48 == (int)&data_020889b0;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
#pragma explicit_zero_data on
int data_020889b0[26] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
#pragma explicit_zero_data off
#pragma section forcedata end
