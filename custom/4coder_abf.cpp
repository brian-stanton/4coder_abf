
//#if !defined(FCODER_DEFAULT_BINDINGS_CPP)
#ifndef FCODER_DEFAULT_BINDINGS_CPP
#define FCODER_DEFAULT_BINDINGS_CPP

#include "4coder_default_include.cpp"
#include "abf-custom/4coder_abf_center_view.h"
#include "abf-custom/4coder_abf_mapping.cpp"

// TODO(brian): figure out if this include is really needed
#include "generated/managed_id_metadata.cpp"

#define ABF_CUSTOM_MAPPING true
#define ABF_COLOR_TEXT 0xFF90B080
#define ABF_COLOR_HIGHLIGHT_CURSOR_LINE 0xFF3D65A6
#define ABF_COLOR_BACKGROUND 0xFF121A26
#define ABF_COLOR_MARGINS 0xFFC48000
#define ABF_COLOR_SCOPE_HIGHLIGHT_VAL 0xFF288D8F
#define ABF_COLOR_PP_INCLUDE 0xFFC2A70E
#define ABF_COLOR_LIST_HOVER 0xFF3F556E

function void abf_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id);
function void abf_render_buffer(Application_Links* app, View_ID view_id, Face_ID face_id,
    Buffer_ID buffer, Text_Layout_ID text_layout_id,
    Rect_f32 rect);
function void set_abf_color_scheme(Application_Links* app);

// TODO(brian): Put into custom command file
CUSTOM_COMMAND_SIG(abf_center_view)
CUSTOM_DOC("Cycling through three positions, the frame will first center on the line the cursor is on; next, the line the cursor is on will go to the top of the frame; lastly, the line will go to the bottom of the frame")
{

    View_ID view = get_active_view(app, Access_ReadVisible);
    Rect_f32 region = view_get_buffer_region(app, view);

    i64 pos = view_get_cursor_pos(app, view);
    Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(pos));
    f32 view_height = rect_height(region);
    Buffer_Scroll ScrollBuffer = view_get_buffer_scroll(app, view);

    // NOTE(brian): 2020/11/03 Customization
    // If we're in a center_view loop and the line we originally centered on hasn't been navigated away from; i.e. we want subsequent center_view calls to iterate through the loop
    if (CVLastLine == cursor.line) {

        ScrollBuffer.target.line_number = cursor.line;

        switch (CVCurrentState) {
        case ABF_CVC_CENTER:
        {
            // Scroll to top and update state
            ScrollBuffer.target.pixel_shift.y = -view_height * abfTopMod;
            CVCurrentState = ABF_CVC_TOP;
        } break;
        case ABF_CVC_TOP:
        {
            f32 LineHeight = get_view_line_height(app, view);
            // Scroll to bottom and update state
            ScrollBuffer.target.pixel_shift.y = -view_height + LineHeight;
            CVCurrentState = ABF_CVC_BOTTOM;
        } break;
        case ABF_CVC_BOTTOM:
        {
            // Scroll back to center and update state
            ScrollBuffer.target.pixel_shift.y = -view_height * abfCenterMod;
            CVCurrentState = ABF_CVC_CENTER;
        } break;
        }

    }
    else {
        ScrollBuffer.target.line_number = cursor.line;
        ScrollBuffer.target.pixel_shift.y = -view_height * abfCenterMod;

        CVCurrentState = ABF_CVC_CENTER;
    }

    view_set_buffer_scroll(app, view, ScrollBuffer, SetBufferScroll_SnapCursorIntoView);
    no_mark_snap_to_cursor(app, view);
    CVLastLine = cursor.line;
}

void
custom_layer_init(Application_Links *app) {
    Thread_Context *tctx = get_thread_context(app);

    // NOTE(brian): Init CV
    CVLastLine = 0;
    CVCurrentState = ABF_CVC_OFF;

    default_framework_init(app);
    set_abf_color_scheme(app);

    set_all_default_hooks(app);
	
    // NOTE(brian): setting background color
    set_custom_hook(app, HookID_RenderCaller, abf_render_caller);
	
    mapping_init(tctx, &framework_mapping);

    #if OS_MAC
    setup_mac_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    #elif ABF_CUSTOM_MAPPING
    setup_abf_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    #else
    setup_default_mapping(&framework_mapping, mapid_global, mapid_file, mapid_code);
    #endif
}

function void
set_abf_color_scheme(Application_Links* app) {
    if (global_theme_arena.base_allocator == 0) {
        global_theme_arena = make_arena_system();
    }

    Arena* arena = &global_theme_arena;

    //default_color_table = make_color_table(app, arena);

    default_color_table.arrays[0] = make_colors(arena, ABF_COLOR_BACKGROUND); // No idea
    default_color_table.arrays[defcolor_bar] = make_colors(arena, 0xFF888888); // Filebar background color
    default_color_table.arrays[defcolor_base] = make_colors(arena, 0xFF000000); // Filebar text color... probably some other things too
    default_color_table.arrays[defcolor_pop1] = make_colors(arena, 0xFF3C57DC);
    default_color_table.arrays[defcolor_pop2] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_back] = make_colors(arena, 0xFF0C0C0C);

    default_color_table.arrays[defcolor_margin] = make_colors(arena, 0xFF181818);
    default_color_table.arrays[defcolor_margin_hover] = make_colors(arena, 0xFF252525);
    default_color_table.arrays[defcolor_margin_active] = make_colors(arena, 0xFFbd6800);// NOTE(brian): not gonna show up because of the draw_margin call in 4coder_abf.cpp
    default_color_table.arrays[defcolor_list_item] = make_colors(arena, ABF_COLOR_BACKGROUND, ABF_COLOR_BACKGROUND & 0xFF0F0F0F);// 0xFF0C0C0C);// (Lighter, Darker)
    default_color_table.arrays[defcolor_list_item_hover] = make_colors(arena, ABF_COLOR_BACKGROUND, ABF_COLOR_LIST_HOVER);
    default_color_table.arrays[defcolor_list_item_active] = make_colors(arena, ABF_COLOR_HIGHLIGHT_CURSOR_LINE, ABF_COLOR_HIGHLIGHT_CURSOR_LINE);// 0xFF323232);

    default_color_table.arrays[defcolor_cursor] = make_colors(arena, 0xFF00EE00, 0xFFEE7700);
    default_color_table.arrays[defcolor_at_cursor] = make_colors(arena, 0xFF0C0C0C);
    default_color_table.arrays[defcolor_highlight_cursor_line] = make_colors(arena, ABF_COLOR_HIGHLIGHT_CURSOR_LINE);
    default_color_table.arrays[defcolor_highlight] = make_colors(arena, ABF_COLOR_HIGHLIGHT_CURSOR_LINE);
    default_color_table.arrays[defcolor_at_highlight] = make_colors(arena, 0xFFFF44DD);
    default_color_table.arrays[defcolor_mark] = make_colors(arena, 0xFF494949);
    default_color_table.arrays[defcolor_text_default] = make_colors(arena, ABF_COLOR_TEXT); // 0xFF90B080
    default_color_table.arrays[defcolor_comment] = make_colors(arena, 0xFF2090F0);
    default_color_table.arrays[defcolor_comment_pop] = make_colors(arena, 0xFF00A000, 0xFFA00000);
    default_color_table.arrays[defcolor_keyword] = make_colors(arena, 0xFF249cff);// 0xFFD08F20); // if, return, typedef etc; might want to change this given the color of the PP and include colors

    default_color_table.arrays[defcolor_str_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_char_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_int_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_float_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_bool_constant] = make_colors(arena, 0xFF50FF30);

    default_color_table.arrays[defcolor_preproc] = make_colors(arena, ABF_COLOR_PP_INCLUDE);
    default_color_table.arrays[defcolor_include] = make_colors(arena, ABF_COLOR_PP_INCLUDE);
    default_color_table.arrays[defcolor_special_character] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_ghost_character] = make_colors(arena, 0xFF4E5E46);
    default_color_table.arrays[defcolor_highlight_junk] = make_colors(arena, 0xFF3A0000);
    default_color_table.arrays[defcolor_highlight_white] = make_colors(arena, 0xFF003A3A);
    default_color_table.arrays[defcolor_paste] = make_colors(arena, 0xFFDDEE00);
    default_color_table.arrays[defcolor_undo] = make_colors(arena, 0xFF00DDEE);
    default_color_table.arrays[defcolor_back_cycle] = make_colors(arena, 0xFF130707, 0xFF071307, 0xFF070713, 0xFF131307);
    default_color_table.arrays[defcolor_text_cycle] = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    default_color_table.arrays[defcolor_line_numbers_back] = make_colors(arena, 0xFF101010);
    default_color_table.arrays[defcolor_line_numbers_text] = make_colors(arena, 0xFF404040);

    active_color_table = default_color_table;
}


/*
function void
abf_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id){
    ProfileScope(app, "default render caller");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    
    Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
    Rect_f32 prev_clip = draw_set_clip(app, region);
    
    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;
    
    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }
    
    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
    
    Buffer_Point_Delta_Result delta = delta_apply(app, view_id,
                                                  frame_info.animation_dt, scroll);
    if (!block_match_struct(&scroll.position, &delta.point)){
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }
    if (delta.still_animating){
        animate_in_n_milliseconds(app, 0);
    }
    
    // NOTE(allen): query bars
    region = default_draw_query_bars(app, region, view_id, face_id);
    
    // NOTE(allen): FPS hud
    if (show_fps_hud){
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }
    
    // NOTE(allen): layout line numbers
    Rect_f32 line_number_rect = {};
    if (global_config.show_line_number_margins){
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }
    
    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);
    
    // NOTE(allen): draw line numbers
    if (global_config.show_line_number_margins){
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }
    
    // NOTE(allen): draw the buffer
    default_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);
    
    // NOTE(brian): testing custom draw here
    // TODO(brian): write something here to show that custom hook has loaded
    //draw_rectangle_fcolor(app, Rf32(0.0f, 0.0f, 100.0f, 100.0f), 0.0f, f_pink);

    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
    
}
*/

function void
abf_render_caller(Application_Links* app, Frame_Info frame_info, View_ID view_id) {
    ProfileScope(app, "default render caller");
    //View_ID active_view = get_active_view(app, Access_Always);
    //b32 is_active_view = (active_view == view_id);

    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 region = rect_inner(view_rect, 1.f);
    //Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
    
    // NOTE(brian): Draw background
    draw_rectangle(app, region, 0.f, ABF_COLOR_BACKGROUND);
    draw_margin(app, view_rect, region, ABF_COLOR_BACKGROUND);
    
    Rect_f32 prev_clip = draw_set_clip(app, region);

    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;

    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar) {
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);

    Buffer_Point_Delta_Result delta = delta_apply(app, view_id,
        frame_info.animation_dt, scroll);
    if (!block_match_struct(&scroll.position, &delta.point)) {
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }
    if (delta.still_animating) {
        animate_in_n_milliseconds(app, 0);
    }

    // NOTE(allen): query bars
    region = default_draw_query_bars(app, region, view_id, face_id);

    // NOTE(allen): FPS hud
    if (show_fps_hud) {
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }

    // NOTE(allen): layout line numbers
    Rect_f32 line_number_rect = {};
    if (global_config.show_line_number_margins) {
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }

    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

    // NOTE(allen): draw line numbers
    if (global_config.show_line_number_margins) {
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }

    // NOTE(allen): draw the buffer
    abf_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);

    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

function void
abf_render_buffer(Application_Links* app, View_ID view_id, Face_ID face_id,
    Buffer_ID buffer, Text_Layout_ID text_layout_id,
    Rect_f32 rect) {
    ProfileScope(app, "render buffer");

    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);

    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    // NOTE(brian): reducing the 0.9 to 0.5 the cursor shape became sharper(less rounded)
    f32 cursor_roundness = (metrics.normal_advance * 0.5f) * 0.5f;// 0.9f;
    f32 mark_thickness = 2.f;

    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if (token_array.tokens != 0) {
        draw_cpp_token_colors(app, text_layout_id, &token_array);

        // NOTE(allen): Scan for TODOs and NOTEs
        if (global_config.use_comment_keyword) {
            Comment_Highlight_Pair pairs[] = {
                {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
            };
            draw_comment_highlights(app, buffer, text_layout_id,
                &token_array, pairs, ArrayCount(pairs));
        }
    }
    else {
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }

    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);

    // NOTE(allen): Scope highlight
    if (global_config.use_scope_highlight) {
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        //Color_Array colors = finalize_color_array(ABF_COLOR_SCOPE_HIGHLIGHT_VAL);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    if (global_config.use_error_highlight || global_config.use_jump_highlight) {
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if (global_config.use_error_highlight) {
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                fcolor_id(defcolor_highlight_junk));
        }

        // NOTE(allen): Search highlight
        if (global_config.use_jump_highlight) {
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if (jump_buffer != compilation_buffer) {
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                    fcolor_id(defcolor_highlight_white));
            }
        }
    }

    // NOTE(allen): Color parens
    if (global_config.use_paren_helper) {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(allen): Line highlight
    if (global_config.highlight_line_at_cursor && is_active_view) {
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);

        // NOTE(brian): Customization here
        draw_line_highlight(app, text_layout_id, line_number,
//            ABF_COLOR_HIGHLIGHT_CURSOR_LINE);
            fcolor_id(defcolor_highlight_cursor_line));
    }

    // NOTE(allen): Whitespace highlight
    b64 show_whitespace = false;
    view_get_setting(app, view_id, ViewSetting_ShowWhitespace, &show_whitespace);
    if (show_whitespace) {
        if (token_array.tokens == 0) {
            draw_whitespace_highlight(app, buffer, text_layout_id, cursor_roundness);
        }
        else {
            draw_whitespace_highlight(app, text_layout_id, &token_array, cursor_roundness);
        }
    }

    // NOTE(allen): Cursor
    switch (fcoder_mode) {
    case FCoderMode_Original:
    {
        draw_original_4coder_style_cursor_mark_highlight(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);
    }break;
    case FCoderMode_NotepadLike:
    {
        draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
    }break;
    }

    // NOTE(allen): Fade ranges
    paint_fade_ranges(app, text_layout_id, buffer, view_id);

    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);

    draw_set_clip(app, prev_clip);
}

#endif
