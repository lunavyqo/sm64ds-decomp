/* GetLevelPart at 0x02013548
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern signed char data_02075264[];
signed char GetLevelPart(int idx) {
    return data_02075264[idx];
}

#pragma define_section forcerodata ".rodata" ".rodata" ".rodata" data_mode=far_abs
#pragma section forcerodata begin
signed char data_02075264[52] = { 0, 0, 0, 16, 0, 0, -1, 127, 0, 16, 0, 0, 0, 16, 0, 0, 0, 80, 0, 0, 0, 16, 69, 0, -122, -1, 121, 0, 20, 0, 80, 0, 80, -126, -96, 0, 2, 31, 80, 25, 1, 0, 0, 5, 0, 32, 0, 24, 0, 24, 0, -56 };
#pragma section forcerodata end
