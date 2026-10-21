extern int data_0209fc48;
extern int data_020889b0[];
int func_0200ee68(void){
return data_0209fc48 == (int)&data_020889b0;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
#pragma explicit_zero_data on
int data_020889b0[26] = { 0x0000040e, 0x00130014, 0x9c006400, 0x041033ff, 0x00270000, 0x4c025814, 0x00101000, 0x040d0000, 0x001e001e, 0x28020018, 0x0e800000, 0x66004b04, 0x00001300, 0xff380064, 0x32040f07, 0x16270f00, 0x00000000, 0x02000000, 0x0032040f, 0x00160050, 0x00ffff88, 0x0f000000, 0x5a004604, 0xa0001600, 0x00000000, 0x00000000 };
#pragma explicit_zero_data off
#pragma section forcedata end
