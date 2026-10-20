#ifndef MEMORIES_DECOMP_CAMPAIGN_CREATE_PRIMARY_DISPLAY_OBJECT_H
#define MEMORIES_DECOMP_CAMPAIGN_CREATE_PRIMARY_DISPLAY_OBJECT_H

#include "../types.h"
#include "display_object.h"

/* Creates and registers the campaign scene's primary screen-space object.
 *
 * It allocates a type 2 object, configures it through DisplayObject_ConfigureSpriteAtPosition with a
 * fixed geometry and depth, sets DISPLAY_OBJECT_FLAG_SCREEN_SPACE in the flag
 * halfword at +0x08, hands it to DisplayObject_SelectOrderingTable1, and returns the pointer.
 *
 * Both callers store the result in the typed gCampaign_PrimaryDisplayObject DisplayObject owner. */
DisplayObject *Campaign_CreatePrimaryDisplayObject(void);

#endif
