/* The decompressed size in an LZ header word (bits 8-31; bits 0-7 are the
   type). The loaders call it on the word after the "LZ77" tag. */
unsigned func_02018ac4(const void *p) {
    return *(const unsigned *)p >> 8;
}
