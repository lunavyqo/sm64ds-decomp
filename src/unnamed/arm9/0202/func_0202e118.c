extern signed char data_0209f2f8;
extern void* data_02092584[];
extern int data_02092654[];
void* func_0202e118(void){
  signed char i = data_0209f2f8;
  return (unsigned)i < 0x34 ? data_02092584[i] : (void*)&data_02092654;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
int data_02092654[3] = { 0x544f4e2a, 0x41545320, 0x002a4547 };
#pragma section forcedata end
