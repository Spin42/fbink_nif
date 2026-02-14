/**
 * fbink_nif.c - Elixir NIF bindings for FBInk
 *
 * Full-featured NIF wrapper for the FBInk library (FrameBuffer eInker).
 * Provides access to all public FBInk API functions from Elixir.
 *
 * Copyright (c) 2026 Marc Lainez
 * SPDX-License-Identifier: MIT
 */

#include <erl_nif.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fbink.h"

// ============================================================================
// Atoms (cached on load)
// ============================================================================

static ERL_NIF_TERM atom_ok;
static ERL_NIF_TERM atom_error;
static ERL_NIF_TERM atom_nil;
static ERL_NIF_TERM atom_true;
static ERL_NIF_TERM atom_false;
static ERL_NIF_TERM atom_undefined;

// Struct field atoms - FBInkConfig
static ERL_NIF_TERM atom_row;
static ERL_NIF_TERM atom_col;
static ERL_NIF_TERM atom_fontmult;
static ERL_NIF_TERM atom_fontname;
static ERL_NIF_TERM atom_is_inverted;
static ERL_NIF_TERM atom_is_flashing;
static ERL_NIF_TERM atom_is_cleared;
static ERL_NIF_TERM atom_is_centered;
static ERL_NIF_TERM atom_hoffset;
static ERL_NIF_TERM atom_voffset;
static ERL_NIF_TERM atom_is_halfway;
static ERL_NIF_TERM atom_is_padded;
static ERL_NIF_TERM atom_is_rpadded;
static ERL_NIF_TERM atom_fg_color;
static ERL_NIF_TERM atom_bg_color;
static ERL_NIF_TERM atom_is_overlay;
static ERL_NIF_TERM atom_is_bgless;
static ERL_NIF_TERM atom_is_fgless;
static ERL_NIF_TERM atom_no_viewport;
static ERL_NIF_TERM atom_is_verbose;
static ERL_NIF_TERM atom_is_quiet;
static ERL_NIF_TERM atom_ignore_alpha;
static ERL_NIF_TERM atom_halign;
static ERL_NIF_TERM atom_valign;
static ERL_NIF_TERM atom_scaled_width;
static ERL_NIF_TERM atom_scaled_height;
static ERL_NIF_TERM atom_wfm_mode;
static ERL_NIF_TERM atom_dithering_mode;
static ERL_NIF_TERM atom_sw_dithering;
static ERL_NIF_TERM atom_cfa_mode;
static ERL_NIF_TERM atom_is_nightmode;
static ERL_NIF_TERM atom_no_refresh;
static ERL_NIF_TERM atom_no_merge;
static ERL_NIF_TERM atom_is_animated;
static ERL_NIF_TERM atom_saturation_boost;
static ERL_NIF_TERM atom_to_syslog;

// FBInkOTConfig atoms
static ERL_NIF_TERM atom_margins;
static ERL_NIF_TERM atom_top;
static ERL_NIF_TERM atom_bottom;
static ERL_NIF_TERM atom_left;
static ERL_NIF_TERM atom_right;
static ERL_NIF_TERM atom_style;
static ERL_NIF_TERM atom_size_pt;
static ERL_NIF_TERM atom_size_px;
static ERL_NIF_TERM atom_padding;
static ERL_NIF_TERM atom_is_formatted;
static ERL_NIF_TERM atom_compute_only;
static ERL_NIF_TERM atom_no_truncation;

// FBInkState atoms
static ERL_NIF_TERM atom_user_hz;
static ERL_NIF_TERM atom_font_name;
static ERL_NIF_TERM atom_view_width;
static ERL_NIF_TERM atom_view_height;
static ERL_NIF_TERM atom_screen_width;
static ERL_NIF_TERM atom_screen_height;
static ERL_NIF_TERM atom_scanline_stride;
static ERL_NIF_TERM atom_bpp;
static ERL_NIF_TERM atom_inverted_grayscale;
static ERL_NIF_TERM atom_device_name;
static ERL_NIF_TERM atom_device_codename;
static ERL_NIF_TERM atom_device_platform;
static ERL_NIF_TERM atom_device_id;
static ERL_NIF_TERM atom_pen_fg_color;
static ERL_NIF_TERM atom_pen_bg_color;
static ERL_NIF_TERM atom_screen_dpi;
static ERL_NIF_TERM atom_font_w;
static ERL_NIF_TERM atom_font_h;
static ERL_NIF_TERM atom_max_cols;
static ERL_NIF_TERM atom_max_rows;
static ERL_NIF_TERM atom_view_hori_origin;
static ERL_NIF_TERM atom_view_vert_origin;
static ERL_NIF_TERM atom_view_vert_offset;
static ERL_NIF_TERM atom_fontsize_mult;
static ERL_NIF_TERM atom_glyph_width;
static ERL_NIF_TERM atom_glyph_height;
static ERL_NIF_TERM atom_is_perfect_fit;
static ERL_NIF_TERM atom_is_mtk;
static ERL_NIF_TERM atom_is_sunxi;
static ERL_NIF_TERM atom_sunxi_has_fbdamage;
static ERL_NIF_TERM atom_sunxi_force_rota;
static ERL_NIF_TERM atom_is_kindle_legacy;
static ERL_NIF_TERM atom_is_kobo_non_mt;
static ERL_NIF_TERM atom_unreliable_wait_for;
static ERL_NIF_TERM atom_can_wake_epdc;
static ERL_NIF_TERM atom_ntx_boot_rota;
static ERL_NIF_TERM atom_ntx_rota_quirk;
static ERL_NIF_TERM atom_rotation_map;
static ERL_NIF_TERM atom_touch_swap_axes;
static ERL_NIF_TERM atom_touch_mirror_x;
static ERL_NIF_TERM atom_touch_mirror_y;
static ERL_NIF_TERM atom_is_ntx_quirky_landscape;
static ERL_NIF_TERM atom_current_rota;
static ERL_NIF_TERM atom_can_rotate;
static ERL_NIF_TERM atom_can_hw_invert;
static ERL_NIF_TERM atom_has_eclipse_wfm;
static ERL_NIF_TERM atom_has_color_panel;
static ERL_NIF_TERM atom_pixel_format;
static ERL_NIF_TERM atom_can_wait_for_submission;

// FBInkRect atoms
static ERL_NIF_TERM atom_width;
static ERL_NIF_TERM atom_height;

// FBInkOTFit atoms
static ERL_NIF_TERM atom_computed_lines;
static ERL_NIF_TERM atom_rendered_lines;
static ERL_NIF_TERM atom_bbox;
static ERL_NIF_TERM atom_truncated;

// FBInkDump atoms
static ERL_NIF_TERM atom_data;
static ERL_NIF_TERM atom_stride;
static ERL_NIF_TERM atom_size;
static ERL_NIF_TERM atom_area;
static ERL_NIF_TERM atom_clip;
static ERL_NIF_TERM atom_rota;
static ERL_NIF_TERM atom_is_full;

// Input atoms
static ERL_NIF_TERM atom_type;
static ERL_NIF_TERM atom_fd;
static ERL_NIF_TERM atom_matched;
static ERL_NIF_TERM atom_name;
static ERL_NIF_TERM atom_path;

// ============================================================================
// Resource type for FBInkDump (opaque, heap-managed)
// ============================================================================

static ErlNifResourceType *dump_resource_type = NULL;

typedef struct {
    FBInkDump dump;
} DumpResource;

static void dump_resource_dtor(ErlNifEnv *env, void *obj) {
    (void)env;
    DumpResource *res = (DumpResource *)obj;
    fbink_free_dump_data(&res->dump);
}

// ============================================================================
// Helper: make atom
// ============================================================================

static ERL_NIF_TERM make_atom(ErlNifEnv *env, const char *name) {
    ERL_NIF_TERM ret;
    if (enif_make_existing_atom(env, name, &ret, ERL_NIF_LATIN1)) {
        return ret;
    }
    return enif_make_atom(env, name);
}

// ============================================================================
// Helper: get bool from map
// ============================================================================

static bool get_bool(ErlNifEnv *env, ERL_NIF_TERM map, ERL_NIF_TERM key, bool default_val) {
    ERL_NIF_TERM val;
    if (!enif_get_map_value(env, map, key, &val)) return default_val;
    // Accept atom true/false or integer 1/0
    char buf[8];
    if (enif_get_atom(env, val, buf, sizeof(buf), ERL_NIF_LATIN1)) {
        return strcmp(buf, "true") == 0;
    }
    int ival;
    if (enif_get_int(env, val, &ival)) return ival != 0;
    return default_val;
}

// ============================================================================
// Helper: get int from map
// ============================================================================

static int get_int(ErlNifEnv *env, ERL_NIF_TERM map, ERL_NIF_TERM key, int default_val) {
    ERL_NIF_TERM val;
    if (!enif_get_map_value(env, map, key, &val)) return default_val;
    int ival;
    if (enif_get_int(env, val, &ival)) return ival;
    return default_val;
}

// ============================================================================
// Helper: get unsigned int from map
// ============================================================================

static unsigned int get_uint(ErlNifEnv *env, ERL_NIF_TERM map, ERL_NIF_TERM key, unsigned int default_val) {
    ERL_NIF_TERM val;
    if (!enif_get_map_value(env, map, key, &val)) return default_val;
    unsigned int uval;
    if (enif_get_uint(env, val, &uval)) return uval;
    // Try signed int for backward compat
    int ival;
    if (enif_get_int(env, val, &ival) && ival >= 0) return (unsigned int)ival;
    return default_val;
}

// ============================================================================
// Helper: get double from map
// ============================================================================

static double get_double(ErlNifEnv *env, ERL_NIF_TERM map, ERL_NIF_TERM key, double default_val) {
    ERL_NIF_TERM val;
    if (!enif_get_map_value(env, map, key, &val)) return default_val;
    double dval;
    if (enif_get_double(env, val, &dval)) return dval;
    // Also accept integers as doubles
    int ival;
    if (enif_get_int(env, val, &ival)) return (double)ival;
    return default_val;
}

// ============================================================================
// Convert Elixir map -> FBInkConfig
// ============================================================================

static void map_to_fbink_config(ErlNifEnv *env, ERL_NIF_TERM map, FBInkConfig *cfg) {
    memset(cfg, 0, sizeof(FBInkConfig));

    cfg->row            = (short int)get_int(env, map, atom_row, 0);
    cfg->col            = (short int)get_int(env, map, atom_col, 0);
    cfg->fontmult       = (uint8_t)get_uint(env, map, atom_fontmult, 0);
    cfg->fontname       = (FONT_INDEX_T)get_uint(env, map, atom_fontname, 0);
    cfg->is_inverted    = get_bool(env, map, atom_is_inverted, false);
    cfg->is_flashing    = get_bool(env, map, atom_is_flashing, false);
    cfg->is_cleared     = get_bool(env, map, atom_is_cleared, false);
    cfg->is_centered    = get_bool(env, map, atom_is_centered, false);
    cfg->hoffset        = (short int)get_int(env, map, atom_hoffset, 0);
    cfg->voffset        = (short int)get_int(env, map, atom_voffset, 0);
    cfg->is_halfway     = get_bool(env, map, atom_is_halfway, false);
    cfg->is_padded      = get_bool(env, map, atom_is_padded, false);
    cfg->is_rpadded     = get_bool(env, map, atom_is_rpadded, false);
    cfg->fg_color       = (FG_COLOR_INDEX_T)get_uint(env, map, atom_fg_color, 0);
    cfg->bg_color       = (BG_COLOR_INDEX_T)get_uint(env, map, atom_bg_color, 0);
    cfg->is_overlay     = get_bool(env, map, atom_is_overlay, false);
    cfg->is_bgless      = get_bool(env, map, atom_is_bgless, false);
    cfg->is_fgless      = get_bool(env, map, atom_is_fgless, false);
    cfg->no_viewport    = get_bool(env, map, atom_no_viewport, false);
    cfg->is_verbose     = get_bool(env, map, atom_is_verbose, false);
    cfg->is_quiet       = get_bool(env, map, atom_is_quiet, false);
    cfg->ignore_alpha   = get_bool(env, map, atom_ignore_alpha, false);
    cfg->halign         = (ALIGN_INDEX_T)get_uint(env, map, atom_halign, 0);
    cfg->valign         = (ALIGN_INDEX_T)get_uint(env, map, atom_valign, 0);
    cfg->scaled_width   = (short int)get_int(env, map, atom_scaled_width, 0);
    cfg->scaled_height  = (short int)get_int(env, map, atom_scaled_height, 0);
    cfg->wfm_mode       = (WFM_MODE_INDEX_T)get_uint(env, map, atom_wfm_mode, 0);
    cfg->dithering_mode = (HW_DITHER_INDEX_T)get_uint(env, map, atom_dithering_mode, 0);
    cfg->sw_dithering   = get_bool(env, map, atom_sw_dithering, false);
    cfg->cfa_mode       = (CFA_MODE_INDEX_T)get_uint(env, map, atom_cfa_mode, 0);
    cfg->is_nightmode   = get_bool(env, map, atom_is_nightmode, false);
    cfg->no_refresh     = get_bool(env, map, atom_no_refresh, false);
    cfg->no_merge       = get_bool(env, map, atom_no_merge, false);
    cfg->is_animated    = get_bool(env, map, atom_is_animated, false);
    cfg->saturation_boost = (uint8_t)get_uint(env, map, atom_saturation_boost, 0);
    cfg->to_syslog      = get_bool(env, map, atom_to_syslog, false);
}

// ============================================================================
// Convert Elixir map -> FBInkOTConfig
// ============================================================================

static void map_to_fbink_ot_config(ErlNifEnv *env, ERL_NIF_TERM map, FBInkOTConfig *cfg) {
    memset(cfg, 0, sizeof(FBInkOTConfig));

    // Margins sub-map
    ERL_NIF_TERM margins_val;
    if (enif_get_map_value(env, map, atom_margins, &margins_val)) {
        cfg->margins.top    = (short int)get_int(env, margins_val, atom_top, 0);
        cfg->margins.bottom = (short int)get_int(env, margins_val, atom_bottom, 0);
        cfg->margins.left   = (short int)get_int(env, margins_val, atom_left, 0);
        cfg->margins.right  = (short int)get_int(env, margins_val, atom_right, 0);
    }

    cfg->style         = (FONT_STYLE_T)get_int(env, map, atom_style, FNT_REGULAR);
    cfg->size_pt       = (float)get_double(env, map, atom_size_pt, 0.0);
    cfg->size_px       = (unsigned short int)get_uint(env, map, atom_size_px, 0);
    cfg->is_centered   = get_bool(env, map, atom_is_centered, false);
    cfg->padding       = (PADDING_INDEX_T)get_uint(env, map, atom_padding, 0);
    cfg->is_formatted  = get_bool(env, map, atom_is_formatted, false);
    cfg->compute_only  = get_bool(env, map, atom_compute_only, false);
    cfg->no_truncation = get_bool(env, map, atom_no_truncation, false);
}

// ============================================================================
// Convert Elixir map -> FBInkRect
// ============================================================================

static void map_to_fbink_rect(ErlNifEnv *env, ERL_NIF_TERM map, FBInkRect *rect) {
    memset(rect, 0, sizeof(FBInkRect));
    rect->left   = (unsigned short int)get_uint(env, map, atom_left, 0);
    rect->top    = (unsigned short int)get_uint(env, map, atom_top, 0);
    rect->width  = (unsigned short int)get_uint(env, map, atom_width, 0);
    rect->height = (unsigned short int)get_uint(env, map, atom_height, 0);
}

// ============================================================================
// Convert FBInkRect -> Elixir map
// ============================================================================

static ERL_NIF_TERM fbink_rect_to_map(ErlNifEnv *env, const FBInkRect *rect) {
    ERL_NIF_TERM map = enif_make_new_map(env);
    enif_make_map_put(env, map, atom_left,   enif_make_uint(env, rect->left), &map);
    enif_make_map_put(env, map, atom_top,    enif_make_uint(env, rect->top), &map);
    enif_make_map_put(env, map, atom_width,  enif_make_uint(env, rect->width), &map);
    enif_make_map_put(env, map, atom_height, enif_make_uint(env, rect->height), &map);
    return map;
}

// ============================================================================
// Convert FBInkState -> Elixir map
// ============================================================================

static ERL_NIF_TERM fbink_state_to_map(ErlNifEnv *env, const FBInkState *state) {
    ERL_NIF_TERM map = enif_make_new_map(env);

    enif_make_map_put(env, map, atom_user_hz,
        enif_make_long(env, state->user_hz), &map);
    enif_make_map_put(env, map, atom_font_name,
        state->font_name ? enif_make_string(env, state->font_name, ERL_NIF_LATIN1) : atom_nil, &map);
    enif_make_map_put(env, map, atom_view_width,
        enif_make_uint(env, state->view_width), &map);
    enif_make_map_put(env, map, atom_view_height,
        enif_make_uint(env, state->view_height), &map);
    enif_make_map_put(env, map, atom_screen_width,
        enif_make_uint(env, state->screen_width), &map);
    enif_make_map_put(env, map, atom_screen_height,
        enif_make_uint(env, state->screen_height), &map);
    enif_make_map_put(env, map, atom_scanline_stride,
        enif_make_uint(env, state->scanline_stride), &map);
    enif_make_map_put(env, map, atom_bpp,
        enif_make_uint(env, state->bpp), &map);
    enif_make_map_put(env, map, atom_inverted_grayscale,
        state->inverted_grayscale ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_device_name,
        enif_make_string(env, state->device_name, ERL_NIF_LATIN1), &map);
    enif_make_map_put(env, map, atom_device_codename,
        enif_make_string(env, state->device_codename, ERL_NIF_LATIN1), &map);
    enif_make_map_put(env, map, atom_device_platform,
        enif_make_string(env, state->device_platform, ERL_NIF_LATIN1), &map);
    enif_make_map_put(env, map, atom_device_id,
        enif_make_uint(env, state->device_id), &map);
    enif_make_map_put(env, map, atom_pen_fg_color,
        enif_make_uint(env, state->pen_fg_color), &map);
    enif_make_map_put(env, map, atom_pen_bg_color,
        enif_make_uint(env, state->pen_bg_color), &map);
    enif_make_map_put(env, map, atom_screen_dpi,
        enif_make_uint(env, state->screen_dpi), &map);
    enif_make_map_put(env, map, atom_font_w,
        enif_make_uint(env, state->font_w), &map);
    enif_make_map_put(env, map, atom_font_h,
        enif_make_uint(env, state->font_h), &map);
    enif_make_map_put(env, map, atom_max_cols,
        enif_make_uint(env, state->max_cols), &map);
    enif_make_map_put(env, map, atom_max_rows,
        enif_make_uint(env, state->max_rows), &map);
    enif_make_map_put(env, map, atom_view_hori_origin,
        enif_make_uint(env, state->view_hori_origin), &map);
    enif_make_map_put(env, map, atom_view_vert_origin,
        enif_make_uint(env, state->view_vert_origin), &map);
    enif_make_map_put(env, map, atom_view_vert_offset,
        enif_make_uint(env, state->view_vert_offset), &map);
    enif_make_map_put(env, map, atom_fontsize_mult,
        enif_make_uint(env, state->fontsize_mult), &map);
    enif_make_map_put(env, map, atom_glyph_width,
        enif_make_uint(env, state->glyph_width), &map);
    enif_make_map_put(env, map, atom_glyph_height,
        enif_make_uint(env, state->glyph_height), &map);
    enif_make_map_put(env, map, atom_is_perfect_fit,
        state->is_perfect_fit ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_is_mtk,
        state->is_mtk ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_is_sunxi,
        state->is_sunxi ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_sunxi_has_fbdamage,
        state->sunxi_has_fbdamage ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_sunxi_force_rota,
        enif_make_int(env, state->sunxi_force_rota), &map);
    enif_make_map_put(env, map, atom_is_kindle_legacy,
        state->is_kindle_legacy ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_is_kobo_non_mt,
        state->is_kobo_non_mt ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_unreliable_wait_for,
        state->unreliable_wait_for ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_can_wake_epdc,
        state->can_wake_epdc ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_ntx_boot_rota,
        enif_make_uint(env, state->ntx_boot_rota), &map);
    enif_make_map_put(env, map, atom_ntx_rota_quirk,
        enif_make_uint(env, state->ntx_rota_quirk), &map);

    // rotation_map as a list of 4 elements
    ERL_NIF_TERM rota_arr[4];
    for (int i = 0; i < 4; i++) {
        rota_arr[i] = enif_make_uint(env, state->rotation_map[i]);
    }
    enif_make_map_put(env, map, atom_rotation_map,
        enif_make_list_from_array(env, rota_arr, 4), &map);

    enif_make_map_put(env, map, atom_touch_swap_axes,
        state->touch_swap_axes ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_touch_mirror_x,
        state->touch_mirror_x ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_touch_mirror_y,
        state->touch_mirror_y ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_is_ntx_quirky_landscape,
        state->is_ntx_quirky_landscape ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_current_rota,
        enif_make_uint(env, state->current_rota), &map);
    enif_make_map_put(env, map, atom_can_rotate,
        state->can_rotate ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_can_hw_invert,
        state->can_hw_invert ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_has_eclipse_wfm,
        state->has_eclipse_wfm ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_has_color_panel,
        state->has_color_panel ? atom_true : atom_false, &map);
    enif_make_map_put(env, map, atom_pixel_format,
        enif_make_uint(env, state->pixel_format), &map);
    enif_make_map_put(env, map, atom_can_wait_for_submission,
        state->can_wait_for_submission ? atom_true : atom_false, &map);

    return map;
}

// ============================================================================
// Convert FBInkOTFit -> Elixir map
// ============================================================================

static ERL_NIF_TERM fbink_ot_fit_to_map(ErlNifEnv *env, const FBInkOTFit *fit) {
    ERL_NIF_TERM map = enif_make_new_map(env);
    enif_make_map_put(env, map, atom_computed_lines,
        enif_make_uint(env, fit->computed_lines), &map);
    enif_make_map_put(env, map, atom_rendered_lines,
        enif_make_uint(env, fit->rendered_lines), &map);

    ERL_NIF_TERM bbox = enif_make_new_map(env);
    enif_make_map_put(env, bbox, atom_width,
        enif_make_uint(env, fit->bbox.width), &bbox);
    enif_make_map_put(env, bbox, atom_height,
        enif_make_uint(env, fit->bbox.height), &bbox);
    enif_make_map_put(env, map, atom_bbox, bbox, &map);

    enif_make_map_put(env, map, atom_truncated,
        fit->truncated ? atom_true : atom_false, &map);
    return map;
}

// ============================================================================
// Helper: return {:ok, value} or {:error, reason}
// ============================================================================

static ERL_NIF_TERM make_ok(ErlNifEnv *env, ERL_NIF_TERM value) {
    return enif_make_tuple2(env, atom_ok, value);
}

static ERL_NIF_TERM make_error_int(ErlNifEnv *env, int code) {
    return enif_make_tuple2(env, atom_error, enif_make_int(env, code));
}

static ERL_NIF_TERM make_error_string(ErlNifEnv *env, const char *reason) {
    return enif_make_tuple2(env, atom_error, make_atom(env, reason));
}

static ERL_NIF_TERM make_ok_or_error(ErlNifEnv *env, int rv) {
    if (rv >= 0) {
        return enif_make_tuple2(env, atom_ok, enif_make_int(env, rv));
    }
    return make_error_int(env, rv);
}

// ============================================================================
// NIF: fbink_version/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_version(ErlNifEnv *env, int argc,
                                       const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    const char *ver = fbink_version();
    return enif_make_string(env, ver, ERL_NIF_LATIN1);
}

// ============================================================================
// NIF: fbink_target/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_target(ErlNifEnv *env, int argc,
                                      const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    return enif_make_uint(env, (unsigned int)fbink_target());
}

// ============================================================================
// NIF: fbink_features/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_features(ErlNifEnv *env, int argc,
                                        const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    return enif_make_uint(env, fbink_features());
}

// ============================================================================
// NIF: fbink_open/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_open(ErlNifEnv *env, int argc,
                                    const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    int fd = fbink_open();
    if (fd < 0) {
        return make_error_int(env, fd);
    }
    return make_ok(env, enif_make_int(env, fd));
}

// ============================================================================
// NIF: fbink_close/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_close(ErlNifEnv *env, int argc,
                                     const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    int rv = fbink_close(fbfd);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_init/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_init(ErlNifEnv *env, int argc,
                                    const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    int rv = fbink_init(fbfd, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_reinit/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_reinit(ErlNifEnv *env, int argc,
                                      const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    int rv = fbink_reinit(fbfd, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_get_state/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_get_state(ErlNifEnv *env, int argc,
                                         const ERL_NIF_TERM argv[]) {
    (void)argc;
    FBInkConfig cfg;
    map_to_fbink_config(env, argv[0], &cfg);

    FBInkState state;
    memset(&state, 0, sizeof(state));
    fbink_get_state(&cfg, &state);

    return fbink_state_to_map(env, &state);
}

// ============================================================================
// NIF: fbink_state_dump/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_state_dump(ErlNifEnv *env, int argc,
                                          const ERL_NIF_TERM argv[]) {
    (void)argc;
    FBInkConfig cfg;
    map_to_fbink_config(env, argv[0], &cfg);

    fbink_state_dump(&cfg);
    return atom_ok;
}

// ============================================================================
// NIF: fbink_get_last_rect/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_get_last_rect(ErlNifEnv *env, int argc,
                                              const ERL_NIF_TERM argv[]) {
    (void)argc;
    int rotated;
    if (!enif_get_int(env, argv[0], &rotated))
        return enif_make_badarg(env);

    FBInkRect rect = fbink_get_last_rect(rotated != 0);
    return fbink_rect_to_map(env, &rect);
}

// ============================================================================
// NIF: fbink_get_last_marker/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_get_last_marker(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    return enif_make_uint(env, fbink_get_last_marker());
}

// ============================================================================
// NIF: fbink_update_verbosity/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_update_verbosity(ErlNifEnv *env, int argc,
                                                 const ERL_NIF_TERM argv[]) {
    (void)argc;
    FBInkConfig cfg;
    map_to_fbink_config(env, argv[0], &cfg);
    fbink_update_verbosity(&cfg);
    return atom_ok;
}

// ============================================================================
// NIF: fbink_update_pen_colors/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_update_pen_colors(ErlNifEnv *env, int argc,
                                                  const ERL_NIF_TERM argv[]) {
    (void)argc;
    FBInkConfig cfg;
    map_to_fbink_config(env, argv[0], &cfg);
    int rv = fbink_update_pen_colors(&cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_set_fg_pen_gray/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_set_fg_pen_gray(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int y;
    int quantize, update;
    if (!enif_get_uint(env, argv[0], &y) ||
        !enif_get_int(env, argv[1], &quantize) ||
        !enif_get_int(env, argv[2], &update))
        return enif_make_badarg(env);

    int rv = fbink_set_fg_pen_gray((uint8_t)y, quantize != 0, update != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_set_bg_pen_gray/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_set_bg_pen_gray(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int y;
    int quantize, update;
    if (!enif_get_uint(env, argv[0], &y) ||
        !enif_get_int(env, argv[1], &quantize) ||
        !enif_get_int(env, argv[2], &update))
        return enif_make_badarg(env);

    int rv = fbink_set_bg_pen_gray((uint8_t)y, quantize != 0, update != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_set_fg_pen_rgba/6
// ============================================================================

static ERL_NIF_TERM nif_fbink_set_fg_pen_rgba(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int r, g, b, a;
    int quantize, update;
    if (!enif_get_uint(env, argv[0], &r) ||
        !enif_get_uint(env, argv[1], &g) ||
        !enif_get_uint(env, argv[2], &b) ||
        !enif_get_uint(env, argv[3], &a) ||
        !enif_get_int(env, argv[4], &quantize) ||
        !enif_get_int(env, argv[5], &update))
        return enif_make_badarg(env);

    int rv = fbink_set_fg_pen_rgba((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a,
                                   quantize != 0, update != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_set_bg_pen_rgba/6
// ============================================================================

static ERL_NIF_TERM nif_fbink_set_bg_pen_rgba(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int r, g, b, a;
    int quantize, update;
    if (!enif_get_uint(env, argv[0], &r) ||
        !enif_get_uint(env, argv[1], &g) ||
        !enif_get_uint(env, argv[2], &b) ||
        !enif_get_uint(env, argv[3], &a) ||
        !enif_get_int(env, argv[4], &quantize) ||
        !enif_get_int(env, argv[5], &update))
        return enif_make_badarg(env);

    int rv = fbink_set_bg_pen_rgba((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a,
                                   quantize != 0, update != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_print/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_print(ErlNifEnv *env, int argc,
                                     const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    ErlNifBinary bin;
    if (!enif_inspect_iolist_as_binary(env, argv[1], &bin))
        return enif_make_badarg(env);

    // Null-terminate the string
    char *str = enif_alloc(bin.size + 1);
    if (!str) return make_error_string(env, "enomem");
    memcpy(str, bin.data, bin.size);
    str[bin.size] = '\0';

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[2], &cfg);

    int rv = fbink_print(fbfd, str, &cfg);
    enif_free(str);

    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_add_ot_font/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_add_ot_font(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    char filename[4096];
    if (enif_get_string(env, argv[0], filename, sizeof(filename), ERL_NIF_LATIN1) <= 0) {
        ErlNifBinary bin;
        if (!enif_inspect_iolist_as_binary(env, argv[0], &bin) || bin.size >= sizeof(filename))
            return enif_make_badarg(env);
        memcpy(filename, bin.data, bin.size);
        filename[bin.size] = '\0';
    }

    int style;
    if (!enif_get_int(env, argv[1], &style))
        return enif_make_badarg(env);

    int rv = fbink_add_ot_font(filename, (FONT_STYLE_T)style);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_free_ot_fonts/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_free_ot_fonts(ErlNifEnv *env, int argc,
                                              const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    int rv = fbink_free_ot_fonts();
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_print_ot/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_print_ot(ErlNifEnv *env, int argc,
                                         const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    ErlNifBinary bin;
    if (!enif_inspect_iolist_as_binary(env, argv[1], &bin))
        return enif_make_badarg(env);

    char *str = enif_alloc(bin.size + 1);
    if (!str) return make_error_string(env, "enomem");
    memcpy(str, bin.data, bin.size);
    str[bin.size] = '\0';

    FBInkOTConfig ot_cfg;
    map_to_fbink_ot_config(env, argv[2], &ot_cfg);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[3], &cfg);

    FBInkOTFit fit;
    memset(&fit, 0, sizeof(fit));

    int rv = fbink_print_ot(fbfd, str, &ot_cfg, &cfg, &fit);
    enif_free(str);

    if (rv < 0) {
        return make_error_int(env, rv);
    }

    ERL_NIF_TERM fit_map = fbink_ot_fit_to_map(env, &fit);
    return enif_make_tuple3(env, atom_ok, enif_make_int(env, rv), fit_map);
}

// ============================================================================
// NIF: fbink_print_progress_bar/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_print_progress_bar(ErlNifEnv *env, int argc,
                                                   const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int percentage;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &percentage))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[2], &cfg);

    int rv = fbink_print_progress_bar(fbfd, (uint8_t)percentage, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_print_activity_bar/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_print_activity_bar(ErlNifEnv *env, int argc,
                                                   const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int progress;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &progress))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[2], &cfg);

    int rv = fbink_print_activity_bar(fbfd, (uint8_t)progress, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_print_image/5
// ============================================================================

static ERL_NIF_TERM nif_fbink_print_image(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    char filename[4096];
    if (enif_get_string(env, argv[1], filename, sizeof(filename), ERL_NIF_LATIN1) <= 0) {
        ErlNifBinary bin;
        if (!enif_inspect_iolist_as_binary(env, argv[1], &bin) || bin.size >= sizeof(filename))
            return enif_make_badarg(env);
        memcpy(filename, bin.data, bin.size);
        filename[bin.size] = '\0';
    }

    int x_off, y_off;
    if (!enif_get_int(env, argv[2], &x_off) ||
        !enif_get_int(env, argv[3], &y_off))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[4], &cfg);

    int rv = fbink_print_image(fbfd, filename, (short int)x_off, (short int)y_off, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_print_raw_data/7
// ============================================================================

static ERL_NIF_TERM nif_fbink_print_raw_data(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    ErlNifBinary bin;
    if (!enif_inspect_binary(env, argv[1], &bin))
        return enif_make_badarg(env);

    int w, h;
    if (!enif_get_int(env, argv[2], &w) ||
        !enif_get_int(env, argv[3], &h))
        return enif_make_badarg(env);

    int x_off, y_off;
    if (!enif_get_int(env, argv[4], &x_off) ||
        !enif_get_int(env, argv[5], &y_off))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[6], &cfg);

    int rv = fbink_print_raw_data(fbfd, bin.data, w, h, bin.size,
                                  (short int)x_off, (short int)y_off, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_cls/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_cls(ErlNifEnv *env, int argc,
                                   const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    // argv[2] is optional rect (nil or map) and argv[3] is no_rota bool
    FBInkRect rect;
    FBInkRect *rect_ptr = NULL;
    if (!enif_is_atom(env, argv[2])) {
        map_to_fbink_rect(env, argv[2], &rect);
        rect_ptr = &rect;
    }

    int no_rota;
    if (!enif_get_int(env, argv[3], &no_rota))
        return enif_make_badarg(env);

    int rv = fbink_cls(fbfd, &cfg, rect_ptr, no_rota != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_grid_clear/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_grid_clear(ErlNifEnv *env, int argc,
                                          const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int cols, rows;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &cols) ||
        !enif_get_uint(env, argv[2], &rows))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[3], &cfg);

    int rv = fbink_grid_clear(fbfd, (unsigned short int)cols,
                              (unsigned short int)rows, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_refresh/6
// ============================================================================

static ERL_NIF_TERM nif_fbink_refresh(ErlNifEnv *env, int argc,
                                       const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int top, left, width, height;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &top) ||
        !enif_get_uint(env, argv[2], &left) ||
        !enif_get_uint(env, argv[3], &width) ||
        !enif_get_uint(env, argv[4], &height))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[5], &cfg);

    int rv = fbink_refresh(fbfd, top, left, width, height, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_refresh_rect/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_refresh_rect(ErlNifEnv *env, int argc,
                                             const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkRect rect;
    map_to_fbink_rect(env, argv[1], &rect);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[2], &cfg);

    int rv = fbink_refresh_rect(fbfd, &rect, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_grid_refresh/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_grid_refresh(ErlNifEnv *env, int argc,
                                             const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int cols, rows;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &cols) ||
        !enif_get_uint(env, argv[2], &rows))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[3], &cfg);

    int rv = fbink_grid_refresh(fbfd, (unsigned short int)cols,
                                (unsigned short int)rows, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_wait_for_submission/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_wait_for_submission(ErlNifEnv *env, int argc,
                                                    const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int marker;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &marker))
        return enif_make_badarg(env);

    int rv = fbink_wait_for_submission(fbfd, marker);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_wait_for_complete/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_wait_for_complete(ErlNifEnv *env, int argc,
                                                  const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int marker;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &marker))
        return enif_make_badarg(env);

    int rv = fbink_wait_for_complete(fbfd, marker);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_wait_for_any_complete/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_wait_for_any_complete(ErlNifEnv *env, int argc,
                                                      const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    int rv = fbink_wait_for_any_complete(fbfd);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_dump/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_dump(ErlNifEnv *env, int argc,
                                    const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    DumpResource *res = enif_alloc_resource(dump_resource_type, sizeof(DumpResource));
    if (!res) return make_error_string(env, "enomem");
    memset(&res->dump, 0, sizeof(FBInkDump));

    int rv = fbink_dump(fbfd, &res->dump);
    if (rv < 0) {
        enif_release_resource(res);
        return make_error_int(env, rv);
    }

    ERL_NIF_TERM res_term = enif_make_resource(env, res);
    enif_release_resource(res);
    return make_ok(env, res_term);
}

// ============================================================================
// NIF: fbink_region_dump/6
// ============================================================================

static ERL_NIF_TERM nif_fbink_region_dump(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    int x_off, y_off;
    unsigned int w, h;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &x_off) ||
        !enif_get_int(env, argv[2], &y_off) ||
        !enif_get_uint(env, argv[3], &w) ||
        !enif_get_uint(env, argv[4], &h))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[5], &cfg);

    DumpResource *res = enif_alloc_resource(dump_resource_type, sizeof(DumpResource));
    if (!res) return make_error_string(env, "enomem");
    memset(&res->dump, 0, sizeof(FBInkDump));

    int rv = fbink_region_dump(fbfd, (short int)x_off, (short int)y_off,
                               (unsigned short int)w, (unsigned short int)h,
                               &cfg, &res->dump);
    if (rv < 0) {
        enif_release_resource(res);
        return make_error_int(env, rv);
    }

    ERL_NIF_TERM res_term = enif_make_resource(env, res);
    enif_release_resource(res);
    return make_ok(env, res_term);
}

// ============================================================================
// NIF: fbink_rect_dump/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_rect_dump(ErlNifEnv *env, int argc,
                                          const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkRect rect;
    map_to_fbink_rect(env, argv[1], &rect);

    DumpResource *res = enif_alloc_resource(dump_resource_type, sizeof(DumpResource));
    if (!res) return make_error_string(env, "enomem");
    memset(&res->dump, 0, sizeof(FBInkDump));

    int rv = fbink_rect_dump(fbfd, &rect, &res->dump);
    if (rv < 0) {
        enif_release_resource(res);
        return make_error_int(env, rv);
    }

    ERL_NIF_TERM res_term = enif_make_resource(env, res);
    enif_release_resource(res);
    return make_ok(env, res_term);
}

// ============================================================================
// NIF: fbink_restore/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_restore(ErlNifEnv *env, int argc,
                                       const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    DumpResource *res;
    if (!enif_get_resource(env, argv[2], dump_resource_type, (void **)&res))
        return enif_make_badarg(env);

    int rv = fbink_restore(fbfd, &cfg, &res->dump);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_free_dump_data/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_free_dump_data(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    DumpResource *res;
    if (!enif_get_resource(env, argv[0], dump_resource_type, (void **)&res))
        return enif_make_badarg(env);

    fbink_free_dump_data(&res->dump);
    return atom_ok;
}

// ============================================================================
// NIF: fbink_get_dump_data/1  (return dump pixel data as binary to Elixir)
// ============================================================================

static ERL_NIF_TERM nif_fbink_get_dump_data(ErlNifEnv *env, int argc,
                                              const ERL_NIF_TERM argv[]) {
    (void)argc;
    DumpResource *res;
    if (!enif_get_resource(env, argv[0], dump_resource_type, (void **)&res))
        return enif_make_badarg(env);

    if (!res->dump.data || res->dump.size == 0)
        return make_error_string(env, "no_data");

    ERL_NIF_TERM map = enif_make_new_map(env);

    // Copy pixel data into an Erlang binary
    ERL_NIF_TERM data_bin;
    unsigned char *buf = enif_make_new_binary(env, res->dump.size, &data_bin);
    memcpy(buf, res->dump.data, res->dump.size);
    enif_make_map_put(env, map, atom_data, data_bin, &map);

    enif_make_map_put(env, map, atom_stride,
        enif_make_uint64(env, (ErlNifUInt64)res->dump.stride), &map);
    enif_make_map_put(env, map, atom_size,
        enif_make_uint64(env, (ErlNifUInt64)res->dump.size), &map);
    enif_make_map_put(env, map, atom_area,
        fbink_rect_to_map(env, &res->dump.area), &map);
    enif_make_map_put(env, map, atom_clip,
        fbink_rect_to_map(env, &res->dump.clip), &map);
    enif_make_map_put(env, map, atom_rota,
        enif_make_uint(env, res->dump.rota), &map);
    enif_make_map_put(env, map, atom_bpp,
        enif_make_uint(env, res->dump.bpp), &map);
    enif_make_map_put(env, map, atom_is_full,
        res->dump.is_full ? atom_true : atom_false, &map);

    return make_ok(env, map);
}

// ============================================================================
// NIF: fbink_invert_screen/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_invert_screen(ErlNifEnv *env, int argc,
                                              const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    int rv = fbink_invert_screen(fbfd, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_invert_rect/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_invert_rect(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkRect rect;
    map_to_fbink_rect(env, argv[1], &rect);

    int no_rota;
    if (!enif_get_int(env, argv[2], &no_rota))
        return enif_make_badarg(env);

    int rv = fbink_invert_rect(fbfd, &rect, no_rota != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_rota_native_to_canonical/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_rota_native_to_canonical(ErlNifEnv *env, int argc,
                                                         const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int rotate;
    if (!enif_get_uint(env, argv[0], &rotate))
        return enif_make_badarg(env);

    uint8_t canonical = fbink_rota_native_to_canonical(rotate);
    return enif_make_uint(env, canonical);
}

// ============================================================================
// NIF: fbink_rota_canonical_to_native/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_rota_canonical_to_native(ErlNifEnv *env, int argc,
                                                         const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int rotate;
    if (!enif_get_uint(env, argv[0], &rotate))
        return enif_make_badarg(env);

    uint32_t native = fbink_rota_canonical_to_native((uint8_t)rotate);
    return enif_make_uint(env, native);
}

// ============================================================================
// NIF: fbink_set_fb_info/5
// ============================================================================

static ERL_NIF_TERM nif_fbink_set_fb_info(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int rota, bpp_val, grayscale;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &rota) ||
        !enif_get_uint(env, argv[2], &bpp_val) ||
        !enif_get_uint(env, argv[3], &grayscale))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[4], &cfg);

    int rv = fbink_set_fb_info(fbfd, rota, (uint8_t)bpp_val, (uint8_t)grayscale, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_fill_rect_gray/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_fill_rect_gray(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    FBInkRect rect;
    map_to_fbink_rect(env, argv[2], &rect);

    int no_rota;
    unsigned int y;
    if (!enif_get_int(env, argv[3], &no_rota) ||
        !enif_get_uint(env, argv[4], &y))
        return enif_make_badarg(env);

    int rv = fbink_fill_rect_gray(fbfd, &cfg, &rect, no_rota != 0, (uint8_t)y);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_fill_rect_rgba/8
// ============================================================================

static ERL_NIF_TERM nif_fbink_fill_rect_rgba(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[1], &cfg);

    FBInkRect rect;
    map_to_fbink_rect(env, argv[2], &rect);

    int no_rota;
    unsigned int r, g, b, a;
    if (!enif_get_int(env, argv[3], &no_rota) ||
        !enif_get_uint(env, argv[4], &r) ||
        !enif_get_uint(env, argv[5], &g) ||
        !enif_get_uint(env, argv[6], &b) ||
        !enif_get_uint(env, argv[7], &a))
        return enif_make_badarg(env);

    int rv = fbink_fill_rect_rgba(fbfd, &cfg, &rect, no_rota != 0,
                                  (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_put_pixel_gray/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_put_pixel_gray(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int x, y, v;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &x) ||
        !enif_get_uint(env, argv[2], &y) ||
        !enif_get_uint(env, argv[3], &v))
        return enif_make_badarg(env);

    int rv = fbink_put_pixel_gray(fbfd, (uint16_t)x, (uint16_t)y, (uint8_t)v);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_put_pixel_rgba/6
// ============================================================================

static ERL_NIF_TERM nif_fbink_put_pixel_rgba(ErlNifEnv *env, int argc,
                                               const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int x, y, r, g, b, a;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &x) ||
        !enif_get_uint(env, argv[2], &y) ||
        !enif_get_uint(env, argv[3], &r) ||
        !enif_get_uint(env, argv[4], &g) ||
        !enif_get_uint(env, argv[5], &b) ||
        !enif_get_uint(env, argv[6], &a))
        return enif_make_badarg(env);

    int rv = fbink_put_pixel_rgba(fbfd, (uint16_t)x, (uint16_t)y,
                                  (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_get_pixel/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_get_pixel(ErlNifEnv *env, int argc,
                                          const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    unsigned int x, y;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_uint(env, argv[1], &x) ||
        !enif_get_uint(env, argv[2], &y))
        return enif_make_badarg(env);

    uint8_t r, g, b, a;
    int rv = fbink_get_pixel(fbfd, (uint16_t)x, (uint16_t)y, &r, &g, &b, &a);
    if (rv < 0)
        return make_error_int(env, rv);

    ERL_NIF_TERM result = enif_make_tuple4(env,
        enif_make_uint(env, r),
        enif_make_uint(env, g),
        enif_make_uint(env, b),
        enif_make_uint(env, a));
    return make_ok(env, result);
}

// ============================================================================
// NIF: fbink_wakeup_epdc/0
// ============================================================================

static ERL_NIF_TERM nif_fbink_wakeup_epdc(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    int rv = fbink_wakeup_epdc();
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_sunxi_toggle_ntx_pen_mode/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_sunxi_toggle_ntx_pen_mode(ErlNifEnv *env, int argc,
                                                           const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, toggle;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &toggle))
        return enif_make_badarg(env);

    int rv = fbink_sunxi_toggle_ntx_pen_mode(fbfd, toggle != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_sunxi_ntx_enforce_rota/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_sunxi_ntx_enforce_rota(ErlNifEnv *env, int argc,
                                                        const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, mode;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &mode))
        return enif_make_badarg(env);

    FBInkConfig cfg;
    map_to_fbink_config(env, argv[2], &cfg);

    int rv = fbink_sunxi_ntx_enforce_rota(fbfd, (SUNXI_FORCE_ROTA_INDEX_T)mode, &cfg);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_mtk_set_swipe_data/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_mtk_set_swipe_data(ErlNifEnv *env, int argc,
                                                    const ERL_NIF_TERM argv[]) {
    (void)argc;
    int direction;
    unsigned int steps;
    if (!enif_get_int(env, argv[0], &direction) ||
        !enif_get_uint(env, argv[1], &steps))
        return enif_make_badarg(env);

    int rv = fbink_mtk_set_swipe_data((MTK_SWIPE_DIRECTION_INDEX_T)direction, (uint8_t)steps);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_mtk_set_halftone/3
// ============================================================================

static ERL_NIF_TERM nif_fbink_mtk_set_halftone(ErlNifEnv *env, int argc,
                                                  const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd;
    if (!enif_get_int(env, argv[0], &fbfd))
        return enif_make_badarg(env);

    // Exclude regions: list of two rect maps (or nil)
    FBInkRect exclude_regions[2];
    memset(exclude_regions, 0, sizeof(exclude_regions));

    ERL_NIF_TERM list = argv[1];
    unsigned int list_len;
    if (enif_get_list_length(env, list, &list_len) && list_len > 0) {
        ERL_NIF_TERM head, tail;
        tail = list;
        for (unsigned int i = 0; i < 2 && i < list_len; i++) {
            if (!enif_get_list_cell(env, tail, &head, &tail)) break;
            map_to_fbink_rect(env, head, &exclude_regions[i]);
        }
    }

    int halftone_size;
    if (!enif_get_int(env, argv[2], &halftone_size))
        return enif_make_badarg(env);

    int rv = fbink_mtk_set_halftone(fbfd, exclude_regions,
                                    (MTK_HALFTONE_MODE_INDEX_T)halftone_size);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_mtk_toggle_auto_reagl/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_mtk_toggle_auto_reagl(ErlNifEnv *env, int argc,
                                                       const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, toggle;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &toggle))
        return enif_make_badarg(env);

    int rv = fbink_mtk_toggle_auto_reagl(fbfd, toggle != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_mtk_toggle_pen_mode/2
// ============================================================================

static ERL_NIF_TERM nif_fbink_mtk_toggle_pen_mode(ErlNifEnv *env, int argc,
                                                     const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, toggle;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &toggle))
        return enif_make_badarg(env);

    int rv = fbink_mtk_toggle_pen_mode(fbfd, toggle != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_input_scan/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_input_scan(ErlNifEnv *env, int argc,
                                           const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int match_types, exclude_types, settings;
    if (!enif_get_uint(env, argv[0], &match_types) ||
        !enif_get_uint(env, argv[1], &exclude_types) ||
        !enif_get_uint(env, argv[2], &settings))
        return enif_make_badarg(env);

    size_t dev_count = 0;
    FBInkInputDevice *devices = fbink_input_scan(
        (INPUT_DEVICE_TYPE_T)match_types,
        (INPUT_DEVICE_TYPE_T)exclude_types,
        (INPUT_SETTINGS_TYPE_T)settings,
        &dev_count);

    if (!devices) {
        return make_ok(env, enif_make_list(env, 0));
    }

    ERL_NIF_TERM *dev_terms = enif_alloc(sizeof(ERL_NIF_TERM) * dev_count);
    for (size_t i = 0; i < dev_count; i++) {
        ERL_NIF_TERM dev_map = enif_make_new_map(env);
        enif_make_map_put(env, dev_map, atom_type,
            enif_make_uint(env, devices[i].type), &dev_map);
        enif_make_map_put(env, dev_map, atom_fd,
            enif_make_int(env, devices[i].fd), &dev_map);
        enif_make_map_put(env, dev_map, atom_matched,
            devices[i].matched ? atom_true : atom_false, &dev_map);
        enif_make_map_put(env, dev_map, atom_name,
            enif_make_string(env, devices[i].name, ERL_NIF_LATIN1), &dev_map);
        enif_make_map_put(env, dev_map, atom_path,
            enif_make_string(env, devices[i].path, ERL_NIF_LATIN1), &dev_map);
        dev_terms[i] = dev_map;
    }

    ERL_NIF_TERM result = enif_make_list_from_array(env, dev_terms, dev_count);
    enif_free(dev_terms);
    free(devices);  // fbink_input_scan returns malloc'd array

    return make_ok(env, result);
}

// ============================================================================
// NIF: fbink_input_check/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_input_check(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    char filepath[4096];
    if (enif_get_string(env, argv[0], filepath, sizeof(filepath), ERL_NIF_LATIN1) <= 0) {
        ErlNifBinary bin;
        if (!enif_inspect_iolist_as_binary(env, argv[0], &bin) || bin.size >= sizeof(filepath))
            return enif_make_badarg(env);
        memcpy(filepath, bin.data, bin.size);
        filepath[bin.size] = '\0';
    }

    unsigned int match_types, exclude_types, settings;
    if (!enif_get_uint(env, argv[1], &match_types) ||
        !enif_get_uint(env, argv[2], &exclude_types) ||
        !enif_get_uint(env, argv[3], &settings))
        return enif_make_badarg(env);

    FBInkInputDevice *dev = fbink_input_check(
        filepath,
        (INPUT_DEVICE_TYPE_T)match_types,
        (INPUT_DEVICE_TYPE_T)exclude_types,
        (INPUT_SETTINGS_TYPE_T)settings);

    if (!dev) {
        return make_error_string(env, "not_found");
    }

    ERL_NIF_TERM dev_map = enif_make_new_map(env);
    enif_make_map_put(env, dev_map, atom_type,
        enif_make_uint(env, dev->type), &dev_map);
    enif_make_map_put(env, dev_map, atom_fd,
        enif_make_int(env, dev->fd), &dev_map);
    enif_make_map_put(env, dev_map, atom_matched,
        dev->matched ? atom_true : atom_false, &dev_map);
    enif_make_map_put(env, dev_map, atom_name,
        enif_make_string(env, dev->name, ERL_NIF_LATIN1), &dev_map);
    enif_make_map_put(env, dev_map, atom_path,
        enif_make_string(env, dev->path, ERL_NIF_LATIN1), &dev_map);

    free(dev);
    return make_ok(env, dev_map);
}

// ============================================================================
// NIF: fbink_button_scan/3 (deprecated but included for completeness)
// ============================================================================

static ERL_NIF_TERM nif_fbink_button_scan(ErlNifEnv *env, int argc,
                                            const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, press_button, nosleep;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &press_button) ||
        !enif_get_int(env, argv[2], &nosleep))
        return enif_make_badarg(env);

    int rv = fbink_button_scan(fbfd, press_button != 0, nosleep != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_wait_for_usbms_processing/2 (deprecated but included)
// ============================================================================

static ERL_NIF_TERM nif_fbink_wait_for_usbms_processing(ErlNifEnv *env, int argc,
                                                           const ERL_NIF_TERM argv[]) {
    (void)argc;
    int fbfd, force_unplug;
    if (!enif_get_int(env, argv[0], &fbfd) ||
        !enif_get_int(env, argv[1], &force_unplug))
        return enif_make_badarg(env);

    int rv = fbink_wait_for_usbms_processing(fbfd, force_unplug != 0);
    return make_ok_or_error(env, rv);
}

// ============================================================================
// NIF: fbink_is_fb_quirky/0 (deprecated)
// ============================================================================

static ERL_NIF_TERM nif_fbink_is_fb_quirky(ErlNifEnv *env, int argc,
                                             const ERL_NIF_TERM argv[]) {
    (void)argc; (void)argv;
    bool quirky = fbink_is_fb_quirky();
    return quirky ? atom_true : atom_false;
}

// ============================================================================
// NIF: fbink_pack_pixel_gray/1
// ============================================================================

static ERL_NIF_TERM nif_fbink_pack_pixel_gray(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int y;
    if (!enif_get_uint(env, argv[0], &y))
        return enif_make_badarg(env);

    uint32_t px;
    int rv = fbink_pack_pixel_gray((uint8_t)y, &px);
    if (rv < 0)
        return make_error_int(env, rv);

    return make_ok(env, enif_make_uint(env, px));
}

// ============================================================================
// NIF: fbink_pack_pixel_rgba/4
// ============================================================================

static ERL_NIF_TERM nif_fbink_pack_pixel_rgba(ErlNifEnv *env, int argc,
                                                const ERL_NIF_TERM argv[]) {
    (void)argc;
    unsigned int r, g, b, a;
    if (!enif_get_uint(env, argv[0], &r) ||
        !enif_get_uint(env, argv[1], &g) ||
        !enif_get_uint(env, argv[2], &b) ||
        !enif_get_uint(env, argv[3], &a))
        return enif_make_badarg(env);

    uint32_t px;
    int rv = fbink_pack_pixel_rgba((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a, &px);
    if (rv < 0)
        return make_error_int(env, rv);

    return make_ok(env, enif_make_uint(env, px));
}

// ============================================================================
// NIF Load callback
// ============================================================================

static int load(ErlNifEnv *env, void **priv_data, ERL_NIF_TERM load_info) {
    (void)priv_data;
    (void)load_info;

    // Create resource type for dumps
    dump_resource_type = enif_open_resource_type(env, NULL, "fbink_dump",
        dump_resource_dtor, ERL_NIF_RT_CREATE, NULL);
    if (!dump_resource_type) return -1;

    // Cache atoms
    atom_ok        = make_atom(env, "ok");
    atom_error     = make_atom(env, "error");
    atom_nil       = make_atom(env, "nil");
    atom_true      = make_atom(env, "true");
    atom_false     = make_atom(env, "false");
    atom_undefined = make_atom(env, "undefined");

    // FBInkConfig field atoms
    atom_row              = make_atom(env, "row");
    atom_col              = make_atom(env, "col");
    atom_fontmult         = make_atom(env, "fontmult");
    atom_fontname         = make_atom(env, "fontname");
    atom_is_inverted      = make_atom(env, "is_inverted");
    atom_is_flashing      = make_atom(env, "is_flashing");
    atom_is_cleared       = make_atom(env, "is_cleared");
    atom_is_centered      = make_atom(env, "is_centered");
    atom_hoffset          = make_atom(env, "hoffset");
    atom_voffset          = make_atom(env, "voffset");
    atom_is_halfway       = make_atom(env, "is_halfway");
    atom_is_padded        = make_atom(env, "is_padded");
    atom_is_rpadded       = make_atom(env, "is_rpadded");
    atom_fg_color         = make_atom(env, "fg_color");
    atom_bg_color         = make_atom(env, "bg_color");
    atom_is_overlay       = make_atom(env, "is_overlay");
    atom_is_bgless        = make_atom(env, "is_bgless");
    atom_is_fgless        = make_atom(env, "is_fgless");
    atom_no_viewport      = make_atom(env, "no_viewport");
    atom_is_verbose       = make_atom(env, "is_verbose");
    atom_is_quiet         = make_atom(env, "is_quiet");
    atom_ignore_alpha     = make_atom(env, "ignore_alpha");
    atom_halign           = make_atom(env, "halign");
    atom_valign           = make_atom(env, "valign");
    atom_scaled_width     = make_atom(env, "scaled_width");
    atom_scaled_height    = make_atom(env, "scaled_height");
    atom_wfm_mode         = make_atom(env, "wfm_mode");
    atom_dithering_mode   = make_atom(env, "dithering_mode");
    atom_sw_dithering     = make_atom(env, "sw_dithering");
    atom_cfa_mode         = make_atom(env, "cfa_mode");
    atom_is_nightmode     = make_atom(env, "is_nightmode");
    atom_no_refresh       = make_atom(env, "no_refresh");
    atom_no_merge         = make_atom(env, "no_merge");
    atom_is_animated      = make_atom(env, "is_animated");
    atom_saturation_boost = make_atom(env, "saturation_boost");
    atom_to_syslog        = make_atom(env, "to_syslog");

    // FBInkOTConfig atoms
    atom_margins       = make_atom(env, "margins");
    atom_top           = make_atom(env, "top");
    atom_bottom        = make_atom(env, "bottom");
    atom_left          = make_atom(env, "left");
    atom_right         = make_atom(env, "right");
    atom_style         = make_atom(env, "style");
    atom_size_pt       = make_atom(env, "size_pt");
    atom_size_px       = make_atom(env, "size_px");
    atom_padding       = make_atom(env, "padding");
    atom_is_formatted  = make_atom(env, "is_formatted");
    atom_compute_only  = make_atom(env, "compute_only");
    atom_no_truncation = make_atom(env, "no_truncation");

    // FBInkState atoms
    atom_user_hz                = make_atom(env, "user_hz");
    atom_font_name              = make_atom(env, "font_name");
    atom_view_width             = make_atom(env, "view_width");
    atom_view_height            = make_atom(env, "view_height");
    atom_screen_width           = make_atom(env, "screen_width");
    atom_screen_height          = make_atom(env, "screen_height");
    atom_scanline_stride        = make_atom(env, "scanline_stride");
    atom_bpp                    = make_atom(env, "bpp");
    atom_inverted_grayscale     = make_atom(env, "inverted_grayscale");
    atom_device_name            = make_atom(env, "device_name");
    atom_device_codename        = make_atom(env, "device_codename");
    atom_device_platform        = make_atom(env, "device_platform");
    atom_device_id              = make_atom(env, "device_id");
    atom_pen_fg_color           = make_atom(env, "pen_fg_color");
    atom_pen_bg_color           = make_atom(env, "pen_bg_color");
    atom_screen_dpi             = make_atom(env, "screen_dpi");
    atom_font_w                 = make_atom(env, "font_w");
    atom_font_h                 = make_atom(env, "font_h");
    atom_max_cols               = make_atom(env, "max_cols");
    atom_max_rows               = make_atom(env, "max_rows");
    atom_view_hori_origin       = make_atom(env, "view_hori_origin");
    atom_view_vert_origin       = make_atom(env, "view_vert_origin");
    atom_view_vert_offset       = make_atom(env, "view_vert_offset");
    atom_fontsize_mult          = make_atom(env, "fontsize_mult");
    atom_glyph_width            = make_atom(env, "glyph_width");
    atom_glyph_height           = make_atom(env, "glyph_height");
    atom_is_perfect_fit         = make_atom(env, "is_perfect_fit");
    atom_is_mtk                 = make_atom(env, "is_mtk");
    atom_is_sunxi               = make_atom(env, "is_sunxi");
    atom_sunxi_has_fbdamage     = make_atom(env, "sunxi_has_fbdamage");
    atom_sunxi_force_rota       = make_atom(env, "sunxi_force_rota");
    atom_is_kindle_legacy       = make_atom(env, "is_kindle_legacy");
    atom_is_kobo_non_mt         = make_atom(env, "is_kobo_non_mt");
    atom_unreliable_wait_for    = make_atom(env, "unreliable_wait_for");
    atom_can_wake_epdc          = make_atom(env, "can_wake_epdc");
    atom_ntx_boot_rota          = make_atom(env, "ntx_boot_rota");
    atom_ntx_rota_quirk         = make_atom(env, "ntx_rota_quirk");
    atom_rotation_map           = make_atom(env, "rotation_map");
    atom_touch_swap_axes        = make_atom(env, "touch_swap_axes");
    atom_touch_mirror_x         = make_atom(env, "touch_mirror_x");
    atom_touch_mirror_y         = make_atom(env, "touch_mirror_y");
    atom_is_ntx_quirky_landscape = make_atom(env, "is_ntx_quirky_landscape");
    atom_current_rota           = make_atom(env, "current_rota");
    atom_can_rotate             = make_atom(env, "can_rotate");
    atom_can_hw_invert          = make_atom(env, "can_hw_invert");
    atom_has_eclipse_wfm        = make_atom(env, "has_eclipse_wfm");
    atom_has_color_panel        = make_atom(env, "has_color_panel");
    atom_pixel_format           = make_atom(env, "pixel_format");
    atom_can_wait_for_submission = make_atom(env, "can_wait_for_submission");

    // FBInkRect atoms
    atom_width  = make_atom(env, "width");
    atom_height = make_atom(env, "height");

    // FBInkOTFit atoms
    atom_computed_lines = make_atom(env, "computed_lines");
    atom_rendered_lines = make_atom(env, "rendered_lines");
    atom_bbox           = make_atom(env, "bbox");
    atom_truncated      = make_atom(env, "truncated");

    // FBInkDump atoms
    atom_data    = make_atom(env, "data");
    atom_stride  = make_atom(env, "stride");
    atom_size    = make_atom(env, "size");
    atom_area    = make_atom(env, "area");
    atom_clip    = make_atom(env, "clip");
    atom_rota    = make_atom(env, "rota");
    atom_is_full = make_atom(env, "is_full");

    // Input atoms
    atom_type    = make_atom(env, "type");
    atom_fd      = make_atom(env, "fd");
    atom_matched = make_atom(env, "matched");
    atom_name    = make_atom(env, "name");
    atom_path    = make_atom(env, "path");

    return 0;
}

// ============================================================================
// NIF Function Table
// ============================================================================

static ErlNifFunc nif_funcs[] = {
    // Info
    {"nif_version",                     0, nif_fbink_version,                     0},
    {"nif_target",                      0, nif_fbink_target,                      0},
    {"nif_features",                    0, nif_fbink_features,                    0},

    // Lifecycle
    {"nif_open",                        0, nif_fbink_open,                        0},
    {"nif_close",                       1, nif_fbink_close,                       0},
    {"nif_init",                        2, nif_fbink_init,                        0},
    {"nif_reinit",                      2, nif_fbink_reinit,                      0},

    // State
    {"nif_get_state",                   1, nif_fbink_get_state,                   0},
    {"nif_state_dump",                  1, nif_fbink_state_dump,                  0},
    {"nif_get_last_rect",              1, nif_fbink_get_last_rect,              0},
    {"nif_get_last_marker",            0, nif_fbink_get_last_marker,            0},
    {"nif_is_fb_quirky",               0, nif_fbink_is_fb_quirky,               0},

    // Config updates
    {"nif_update_verbosity",           1, nif_fbink_update_verbosity,           0},
    {"nif_update_pen_colors",          1, nif_fbink_update_pen_colors,          0},
    {"nif_set_fg_pen_gray",            3, nif_fbink_set_fg_pen_gray,            0},
    {"nif_set_bg_pen_gray",            3, nif_fbink_set_bg_pen_gray,            0},
    {"nif_set_fg_pen_rgba",            6, nif_fbink_set_fg_pen_rgba,            0},
    {"nif_set_bg_pen_rgba",            6, nif_fbink_set_bg_pen_rgba,            0},

    // Text printing
    {"nif_print",                      3, nif_fbink_print,                      0},
    {"nif_add_ot_font",                2, nif_fbink_add_ot_font,                0},
    {"nif_free_ot_fonts",              0, nif_fbink_free_ot_fonts,              0},
    {"nif_print_ot",                   4, nif_fbink_print_ot,                   0},

    // Progress/Activity bars
    {"nif_print_progress_bar",         3, nif_fbink_print_progress_bar,         0},
    {"nif_print_activity_bar",         3, nif_fbink_print_activity_bar,         0},

    // Image rendering
    {"nif_print_image",                5, nif_fbink_print_image,                0},
    {"nif_print_raw_data",             7, nif_fbink_print_raw_data,             0},

    // Screen clear
    {"nif_cls",                        4, nif_fbink_cls,                        0},
    {"nif_grid_clear",                 4, nif_fbink_grid_clear,                 0},

    // Refresh
    {"nif_refresh",                    6, nif_fbink_refresh,                    0},
    {"nif_refresh_rect",               3, nif_fbink_refresh_rect,               0},
    {"nif_grid_refresh",               4, nif_fbink_grid_refresh,               0},
    {"nif_wait_for_submission",        2, nif_fbink_wait_for_submission,        0},
    {"nif_wait_for_complete",          2, nif_fbink_wait_for_complete,          0},
    {"nif_wait_for_any_complete",      1, nif_fbink_wait_for_any_complete,      0},

    // Dump/Restore
    {"nif_dump",                       1, nif_fbink_dump,                       0},
    {"nif_region_dump",                6, nif_fbink_region_dump,                0},
    {"nif_rect_dump",                  2, nif_fbink_rect_dump,                  0},
    {"nif_restore",                    3, nif_fbink_restore,                    0},
    {"nif_free_dump_data",             1, nif_fbink_free_dump_data,             0},
    {"nif_get_dump_data",              1, nif_fbink_get_dump_data,              0},

    // Screen inversion
    {"nif_invert_screen",              2, nif_fbink_invert_screen,              0},
    {"nif_invert_rect",                3, nif_fbink_invert_rect,                0},

    // Rotation helpers
    {"nif_rota_native_to_canonical",   1, nif_fbink_rota_native_to_canonical,   0},
    {"nif_rota_canonical_to_native",   1, nif_fbink_rota_canonical_to_native,   0},

    // Framebuffer info
    {"nif_set_fb_info",                5, nif_fbink_set_fb_info,                0},

    // Drawing primitives
    {"nif_fill_rect_gray",            5, nif_fbink_fill_rect_gray,             0},
    {"nif_fill_rect_rgba",            8, nif_fbink_fill_rect_rgba,             0},
    {"nif_put_pixel_gray",            4, nif_fbink_put_pixel_gray,             0},
    {"nif_put_pixel_rgba",            7, nif_fbink_put_pixel_rgba,             0},
    {"nif_get_pixel",                  3, nif_fbink_get_pixel,                  0},
    {"nif_pack_pixel_gray",           1, nif_fbink_pack_pixel_gray,            0},
    {"nif_pack_pixel_rgba",           4, nif_fbink_pack_pixel_rgba,            0},

    // EPDC wakeup
    {"nif_wakeup_epdc",               0, nif_fbink_wakeup_epdc,                0},

    // Sunxi-specific
    {"nif_sunxi_toggle_ntx_pen_mode", 2, nif_fbink_sunxi_toggle_ntx_pen_mode, 0},
    {"nif_sunxi_ntx_enforce_rota",    3, nif_fbink_sunxi_ntx_enforce_rota,    0},

    // MTK-specific
    {"nif_mtk_set_swipe_data",        2, nif_fbink_mtk_set_swipe_data,        0},
    {"nif_mtk_set_halftone",          3, nif_fbink_mtk_set_halftone,          0},
    {"nif_mtk_toggle_auto_reagl",     2, nif_fbink_mtk_toggle_auto_reagl,     0},
    {"nif_mtk_toggle_pen_mode",       2, nif_fbink_mtk_toggle_pen_mode,       0},

    // Input scanning
    {"nif_input_scan",                3, nif_fbink_input_scan,                 0},
    {"nif_input_check",               4, nif_fbink_input_check,                0},

    // Button scan (deprecated)
    {"nif_button_scan",               3, nif_fbink_button_scan,                0},
    {"nif_wait_for_usbms_processing", 2, nif_fbink_wait_for_usbms_processing, 0},
};

ERL_NIF_INIT(Elixir.FBInk.NIF, nif_funcs, load, NULL, NULL, NULL)
