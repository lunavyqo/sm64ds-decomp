/* storage 0x0238E0E4, runtime 0x027C005C, 0x3C bytes including pool.
 * Classify the average of sixteen unsigned byte samples.
 * The data block's original structure and field names remain unrecovered.
 */
extern unsigned char dat_27D6A84[];
extern int fn_0238E0BC(unsigned int average);

int fn_0238E0E4(void)
{
    int i;
    unsigned int sum;
    sum = 0;
    i = 0;
    for (; i < 16; ++i)
        sum += (dat_27D6A84 + i)[0x1614];
    return fn_0238E0BC(sum >> 4);
}
