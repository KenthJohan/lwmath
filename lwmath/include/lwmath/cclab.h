#pragma once
#include <stdint.h>

/** Builds connected regions of same labels
 * Connected-component labeling is used in computer vision to detect connected regions in binary digital images, although color images and data with higher dimensionality can also be processed.
 *
 * @param input Input image
 * @param mask Which bits should be intepreted as true
 * @param component Number of hits per label
 * @param labels Output labels image
 * @param w Width of input, component, labels
 * @param h Height of input, component, labels
 * @return 
 */
void cclab_union_find(uint32_t const *input, uint32_t mask, uint32_t *component, uint32_t * labels, uint32_t w, uint32_t h);