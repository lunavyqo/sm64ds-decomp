// @symbol daObjEwmIceBlock_c_classInit
/* The big ice block that moves.
 * Leftover: classInit. return new matches this body but also emits homeless
 * _ZN10dBgActor_cD2Ev, which this separate TU cannot deadstrip. */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern void _ZN7PathPtrC1Ev(void *);

int *daObjEwmIceBlock_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjEwmIceBlock_c[2];
        _ZN7PathPtrC1Ev((char *)p + 0x320);
    }
    return p;
}
