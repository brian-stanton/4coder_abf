#ifndef _4CODER_ABF
#define _4CODER_ABF

#include "4coder_default_include.cpp"

/**
    Header file includes
*/
#include "abf-custom/4coder_abf_center_view.h"
#include "abf-custom/4coder_abf_CA.cpp"
#include "abf-custom/4coder_abf_syntax_highlighting.cpp"

/**
    Code files that don't use any functions defined in 4coder_abf.cpp
*/
#include "abf-custom/4coder_abf_mapping.cpp"

// TODO(brian): figure out if this include is really needed
#include "generated/managed_id_metadata.cpp"

#define ABF_CUSTOM_MAPPING true
#define ABF_CUSTOMIZATIONS true

// TODO(brian): pull out preproc defines to build command line parameter
#define ABF_CA false
#define ABF_SYNTAX_HIGHLIGHT true

#define ABF_MANAGED_SCOPE_GROUP_NAME ((void*) "abf")

#define ABF_COLOR_TEXT 0xFF90B080
#define ABF_COLOR_HIGHLIGHT_CURSOR_LINE 0xFF3D65A6
#define ABF_COLOR_BACKGROUND 0xFF121A26
#define ABF_COLOR_TRIM 0xFF1497A3 // NOTE(brian): for margins and file bar
#define ABF_COLOR_SCOPE_HIGHLIGHT_VAL 0xFF288D8F
#define ABF_COLOR_PP_INCLUDE 0xFFC2A70E
#define ABF_COLOR_LIST_HOVER 0xFF3F556E
#define ABF_COLOR_TYPE 0xFF30E3E3
#define ABF_COLOR_VAR_DECLARATION 0xFF1FAB7E
#define ABF_COLOR_PARENS 0xFFFFFFFF

#define FOUR_CODER_COLOR_TEXT_DEFAULT 0xFF90B080

function void abf_render_caller(Application_Links* app, Frame_Info frame_info, View_ID view_id);
function void abf_render_buffer(Application_Links* app, View_ID view_id, Face_ID face_id,
    Buffer_ID buffer, Text_Layout_ID text_layout_id,
    Rect_f32 rect);
function void set_abf_color_scheme(Application_Links* app);
function void abf_draw_cpp_token_colors(Application_Links* app, Text_Layout_ID text_layout_id, Token_Array* array, Buffer_ID buffer);
function void abf_draw_scope_highlight(Application_Links* app, Buffer_ID buffer, Text_Layout_ID text_layout_id, i64 pos, ARGB_Color* colors, i32 color_count);
function void abf_draw_enclosures(Application_Links* app, Text_Layout_ID text_layout_id, Buffer_ID buffer, i64 pos, u32 flags, Range_Highlight_Kind kind, ARGB_Color* back_colors, i32 back_count, ARGB_Color* fore_colors, i32 fore_count);
function FColor abf_get_token_color_cpp(Token token);
function Managed_ID abf_token_color(Token token);
function Managed_ID abf_color_keyword(Token_Base_Kind sub_kind);
function Managed_ID abf_color_identifier(Token_Base_Kind sub_kind);
function void abf_draw_cpp_token_colors(Application_Links* app, Text_Layout_ID text_layout_id, Token_Array* array, Buffer_ID buffer);

function void abf_page(Application_Links* app, View_ID view, Rect_f32 region, Buffer_ID Buffer, b32 PageUp);


function void abf_init(Application_Links* app);

static Managed_ID abf_ca_scope;
abf_ca_context* abfCAContext;

#endif