/* recovered: the ROM reaches func_0205d644 with r0 and r1 still holding this
 * function's own two arguments (push {r4, lr} / bl func_0205d644 / movs r4, r0),
 * so both parameters are real and are forwarded. Every caller in the image
 * already passes two: func_020182bc, func_02018434, func_020189f0. */
extern int func_0205d644(void *out_file_id, const char *path);
extern void func_02018e68(void*);
extern void Crash(void);
extern int data_0208ecd8[];
int func_02018e3c(void *out_file_id, const char *path){
  int r = func_0205d644(out_file_id, path);
  if(!r){
    func_02018e68(data_0208ecd8);
    Crash();
  }
  return r;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
int data_0208ecd8[7] = { 0x3130352d, 0x403a3b3e, 0x180cff27, 0x17121d17, 0xff1b0e1e, 0x000105fe, 0x4d0a4d01 };
#pragma section forcedata end
