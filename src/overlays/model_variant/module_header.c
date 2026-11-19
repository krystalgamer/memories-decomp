#include "../../types.h"

/*
 * The image's MODEL.MRG build-list word (notes/model-effects.md). The overlay
 * build supplies both macros from config/<region>/overlays/model_module_words.csv
 * and the address of the layout's module_header segment.
 */
u32 MODEL_MODULE_WORD_LABEL = MODEL_MODULE_WORD;
