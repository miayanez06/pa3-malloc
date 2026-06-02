#include "vmlib.h"

int main() {
    vminit(2000); // comment this out if using vmload()

    /* alternatively, load a heap image */
    // vmload("tests/img/many_free.img");

    void *p1 = vmalloc(20);
    void *p2 = vmalloc(40);

    vminfo();

    vmfree(p1);

    vminfo();

    vmfree(p2);

    vminfo();

    vmdestroy();

    return 0;
}
