#include "vm.h"
#include "vmlib.h"

/**
 * Allocate a memory block with the given minimum size on the simulated heap.
 * If allocation succeeds, return a pointer to the start of the new block's
 * payload. If allocation fails due to insufficient free space, return NULL.
 *
 * Read the section for vmalloc in the PA writeup for how to implement this
 * function.
 
 */


void *vmalloc(size_t size) {

    if (size == 0) {
        return NULL;
    }

    size_t needed =
        ROUND_UP(size + sizeof(struct block_header),
                 BLKALIGN);

    struct block_header *curr = heapstart;
    struct block_header *best = NULL;

    while (curr->size_status != VM_ENDMARK) {

        size_t curr_size = BLKSZ(curr);

        int busy = curr->size_status & VM_BUSY;

        if (!busy && curr_size >= needed) {

            if (best == NULL ||
                curr_size < BLKSZ(best)) {

                best = curr;
            }
        }

        curr = (struct block_header *)
               ((char *)curr + curr_size);
    }

    if (best == NULL) {
        return NULL;
    }

    size_t best_size = BLKSZ(best);
    size_t remaining = best_size - needed;

    /*
     * Split block if remaining space is large enough
     * for a new free block.
     */
    if (remaining >= 16) {

        struct block_header *newblk =
            (struct block_header *)
            ((char *)best + needed);

        newblk->size_status = remaining;
        newblk->size_status |= VM_PREVBUSY;

        struct block_footer *footer =
            (struct block_footer *)
            ((char *)newblk
             + remaining
             - sizeof(struct block_footer));

        footer->size = remaining;

        size_t prevbit =
            best->size_status & VM_PREVBUSY;

        best->size_status =
            needed | prevbit | VM_BUSY;

    } else {

        best->size_status |= VM_BUSY;
    }

    struct block_header *next =
        (struct block_header *)
        ((char *)best + BLKSZ(best));

    if (next->size_status != VM_ENDMARK) {
        next->size_status |= VM_PREVBUSY;
    }

    return (void *)
           ((char *)best +
            sizeof(struct block_header));
}
