extern int ACTOR_DEBUG_NAMES[];
extern int data_020914a0[];
int GetActorDebugName(unsigned int i) {
    return (i < 0x188) ? ACTOR_DEBUG_NAMES[i] : (int)&data_020914a0;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
int data_020914a0[2] = { 0x5252452a, 0x002a524f };
#pragma section forcedata end
