extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern int data_020a637c[];

struct Blk {
    int f0;
    int f4;
    int f8;
    int fc;
    int *f10;
};

int func_02059040(int idx, struct Blk *b, unsigned int c, int d)
{
    unsigned int saved = (unsigned int)_ZN3IRQ7DisableEv();
    int i;
    int m = d * 0xc;
    data_020a637c[idx] = (int)b;
    b->f10 = (int*)((char*)b + 0x14);
    b->f4 = d;
    for (i = 0; i < b->f4; i++) {
        int *ent = (int*)((char*)b->f10 + i * 0xc);
        ent[0] = -1;
        ent[2] = 0;
        ent[1] = ent[2];
    }
    b->f0 = -1;
    b->f8 = (int)(((char*)b->f10 + m + 0x1f) - 0) & ~0x1f;
    b->fc = c & ~0x1f;
    _ZN3IRQ7RestoreEj(saved);
    return b->f8;
}

#include "types.h"
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);
extern void *func_02059364(void *a, void *n);
extern void *func_0205929c(void *a, void *n);
struct Elem {
    void *w0;
    void *w4;
    void *w8;
}; /* 0xc */

struct G637c {
    int head; /* 0x0 */
    char pad[0xc];
    struct Elem *base; /* 0x10 */
};

extern struct G637c *data_020a637c[];

void func_020590fc(int idx, int sel, void *arg) {
    struct G637c *g;
    struct Elem *e;
    void *a;
    u32 saved = _ZN3IRQ7DisableEv();
    g = data_020a637c[idx];
    if (sel < 0)
        sel = g->head;
    a = (char *)arg - 0x20;
    e = &g->base[sel];
    e->w8 = func_02059364(e->w8, a);
    e->w4 = func_0205929c(e->w4, a);
    _ZN3IRQ7RestoreEj(saved);
}

extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern void *func_02059364(void *a, void *n);
extern void *func_0205938c(void *next, void *node);
extern int *data_020a637c[];

struct Node {
    struct Node *prev;
    struct Node *next;
    int size;
};
struct Bucket {
    int f0;
    struct Node *head;
    void *f8;
};

void *func_02059170(int id, int idx2, int reqsize) {
    int *pool;
    struct Bucket *b;
    struct Node *n;
    unsigned int state;
    int rem;

    state = _ZN3IRQ7DisableEv();
    pool = data_020a637c[id];
    if (pool == 0) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }
    if (idx2 < 0)
        idx2 = pool[0];
    b = (struct Bucket *)((char *)pool[4] + idx2 * 0xc);
    n = b->head;
    reqsize = ((int)((char *)reqsize + 0x20) + 0x1f) & ~0x1f;
    if (n != 0) {
        do {
            if (reqsize <= n->size)
                break;
            n = n->next;
        } while (n != 0);
    }
    if (n == 0) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }
    rem = n->size - reqsize;
    if ((unsigned int)rem < 0x40) {
        b->head = func_02059364(b->head, n);
    } else {
        struct Node *nn;
        n->size = reqsize;
        nn = (struct Node *)((char *)n + reqsize);
        nn->size = rem;
        nn->prev = n->prev;
        nn->next = n->next;
        if (nn->next != 0)
            nn->next->prev = nn;
        if (nn->prev != 0)
            nn->prev->next = nn;
        else
            b->head = nn;
    }
    b->f8 = func_0205938c(b->f8, n);
    _ZN3IRQ7RestoreEj(state);
    return (char *)n + 0x20;
}

typedef struct Block {
    struct Block *prev;
    struct Block *next;
    int size;
} Block;

void *func_0205929c(void *ap, void *bp) {
    Block *prev = 0;
    Block *cur = (Block *)ap;
    Block *b = (Block *)bp;

    while (cur != 0) {
        if (b <= cur)
            break;
        prev = cur;
        cur = cur->next;
    }

    b->next = cur;
    b->prev = prev;
    if (cur != 0) {
        cur->prev = b;
        if ((Block *)((char *)b + b->size) == cur) {
            int *bsize = (int *)(&b->size);
            *bsize += cur->size;
            cur = cur->next;
            b->next = cur;
            if (cur != 0)
                cur->prev = b;
        }
    }
    if (prev != 0) {
        prev->next = b;
        if ((Block *)((char *)prev + prev->size) == b) {
            int *psize = (int *)(&prev->size);
            *psize += b->size;
            prev->next = cur;
            if (cur != 0)
                cur->prev = prev;
        }
        return ap;
    }
    return b;
}

struct Node89 {
    struct Node89 *f0;
    struct Node89 *f4;
};

void *func_02059364(void *a, struct Node89 *n) {
    if (n->f4 != 0)
        n->f4->f0 = n->f0;
    if (n->f0 == 0)
        return n->f4;
    n->f0->f4 = n->f4;
    return a;
}

struct Node {
    struct Node *prev;
    struct Node *next;
};

struct Node *func_0205938c(struct Node *next, struct Node *node) {
    node->next = next;
    node->prev = 0;
    if (next)
        next->prev = node;
    return node;
}
