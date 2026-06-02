#include "vm.h"
#include "vmlib.h"

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmalloc().
 * Otherwise, or if vmfree(ptr) has already been called before,
 * undefined behavior occurs.
 *
 * If ptr is NULL, no operation is performed.
 */
void vmfree(void *ptr) {
    
    if (ptr == NULL) {
        return;
    }

    struct block_header *block =
        (struct block_header *)
        ((char *)ptr - sizeof(struct block_header));

    if (!(block->size_status & VM_BUSY)) {
        return;
    }

    block->size_status &= ~VM_BUSY;

    size_t size = BLKSZ(block);

    struct block_footer *footer =
        (struct block_footer *)
        ((char *)block
         + size
         - sizeof(struct block_footer));

    footer->size = size;

    struct block_header *next =
        (struct block_header *)
        ((char *)block + size);

    if (next->size_status != VM_ENDMARK) {
        next->size_status &= ~VM_PREVBUSY;
    }
    if (next->size_status != VM_ENDMARK &&
    !(next->size_status & VM_BUSY)) {

    size_t new_size =
        BLKSZ(block) + BLKSZ(next);

    size_t prevbit =
        block->size_status & VM_PREVBUSY;

    block->size_status =
        new_size | prevbit;

    struct block_footer *new_footer =
        (struct block_footer *)
        ((char *)block
         + new_size
         - sizeof(struct block_footer));

    new_footer->size = new_size;

    struct block_header *after =
        (struct block_header *)
        ((char *)block + new_size);

    if (after->size_status != VM_ENDMARK) {
        after->size_status &= ~VM_PREVBUSY;
    }

    }
    // left coalesing code 

    if (!(block->size_status & VM_PREVBUSY)) {

    struct block_footer *prev_footer =
        (struct block_footer *)
        ((char *)block
         - sizeof(struct block_footer));

    size_t prev_size = prev_footer->size;

    struct block_header *prev =
        (struct block_header *)
        ((char *)block - prev_size);

    size_t new_size =
        BLKSZ(prev) + BLKSZ(block);

    size_t prevbit =
        prev->size_status & VM_PREVBUSY;

    prev->size_status =
        new_size | prevbit;

    struct block_footer *new_footer =
        (struct block_footer *)
        ((char *)prev
         + new_size
         - sizeof(struct block_footer));

    new_footer->size = new_size;

    struct block_header *after =
        (struct block_header *)
        ((char *)prev + new_size);

    if (after->size_status != VM_ENDMARK) {
        after->size_status &= ~VM_PREVBUSY;
    }
}
}
