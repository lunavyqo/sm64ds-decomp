//cpp
// minimal probe of zero-fill shapes only - not full function
typedef unsigned char u8;
extern "C" void* _ZN6Memory13operator_new2Ej(unsigned int);
extern "C" void probe(void* self) {
  char* c = (char*)self;
  u8* q;
  unsigned int u;
  *(void**)(c + 0x588) = _ZN6Memory13operator_new2Ej(0x14);
  q = *(u8**)(c + 0x588);
  u = 5;
  do { *q++ = 0; *q++ = 0; *q++ = 0; *q++ = 0; } while (--u != 0);
}
