// SPDX-License-Identifier: MIT
// Business logic of application and UI event handlers.
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include "keybind.h"
#include "pixmap.h"

// Configuration parameters
#define VIEWER_CFG_SCALE          "scale"
#define VIEWER_CFG_ANTIALIASING   "antialiasing"
#define VIEWER_CFG_FIXED          "fixed"
#define VIEWER_CFG_TRANSPARENCY   "transparency"
#define VIEWER_CFG_BACKGROUND     "background"
#define VIEWER_CFG_SLIDESHOW      "slideshow"
#define VIEWER_CFG_SLIDESHOW_TIME "slideshow_time"

/**
 * Create global viewer context.
 */
void viewer_create(void);

/**
 * Initialize global viewer context.
 */
void viewer_init(void);

/**
 * Destroy global viewer context.
 */
void viewer_destroy(void);

/**
 * Reset state: reload image file, set initial scale etc.
 */
void viewer_reload(void);

/**
 * Redraw handler.
 * @param window pixel map of window
 */
void viewer_on_redraw(struct pixmap* window);

/**
 * Window resize handler.
 */
void viewer_on_resize(void);

/**
 * Key press handler.
 * @param key code of key pressed
 * @param mods key modifiers (ctrl/alt/shift)
 */
void viewer_on_keyboard(xkb_keysym_t key, uint8_t mods);

/**
 * Image drag handler.
 * @param dx,dy delta to move viewpoint
 */
void viewer_on_drag(int dx, int dy);
