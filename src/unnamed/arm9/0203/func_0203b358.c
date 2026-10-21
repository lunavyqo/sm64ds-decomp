struct Node { struct Node *next; char pad[8]; void *item; };

void *func_0203b358(struct Node *thiz)
{
    struct Node *n;
    if (thiz->item) return thiz->item;
    n = thiz->next;
    while (n) {
        if (n->item) return n->item;
        n = n->next;
    }
    return 0;
}


struct Node {
    char pad[4];
    void *cached;
};
extern void *func_0203b358(struct Node *thiz);
void *func_0203b394(struct Node *thiz) {
    void *p = thiz->cached;
    if (p == 0)
        p = func_0203b358(thiz);
    return p;
}
