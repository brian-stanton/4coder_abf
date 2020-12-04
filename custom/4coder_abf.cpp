
#include "4coder_abf.h"

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

CUSTOM_COMMAND_SIG(abf_page_up)
CUSTOM_DOC("")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Rect_f32 region = view_get_buffer_region(app, view);

    // NOTE(brian): With Access_Visible set, the value Buffer will only have a buffer id if the view attached to the buffer is visible... probably unnecessary here
    // TODO(brian): look into the access flags (Access_Flag)
    Buffer_ID Buffer = view_get_buffer(app, view, Access_Visible);

    if (Buffer) {
        abf_page(app, view, region, Buffer, true);
    }
}

CUSTOM_COMMAND_SIG(abf_page_down)
CUSTOM_DOC("")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Rect_f32 region = view_get_buffer_region(app, view);

    // NOTE(brian): With Access_Visible set, the value Buffer will only have a buffer id if the view attached to the buffer is visible... probably unnecessary here
    // TODO(brian): look into the access flags (Access_Flag)
    Buffer_ID Buffer = view_get_buffer(app, view, Access_Visible);

    if (Buffer) {
        abf_page(app, view, region, Buffer, false);
    }
}

void
custom_layer_init(Application_Links *app) {
    Thread_Context *tctx = get_thread_context(app);

    //abf_ca_scope = create_user_managed_scope(app);// managed_id_get(app, abfManagedScopeGroupName, abfManagedScopeCAName);
    //global_frame_arena = make_arena(get_base_allocator_system());

    // NOTE(brian): 2020/11/25
    system_set_fullscreen(true);

    default_framework_init(app);
    set_abf_color_scheme(app);

    set_all_default_hooks(app);
	
#if ABF_CUSTOMIZATIONS
    // NOTE(brian): Init CV
    CVLastLine = 0;
    CVCurrentState = ABF_CVC_OFF;

    abf_init(app);
#endif

    // NOTE(brian): abf custom rendering
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
    default_color_table.arrays[defcolor_bar] = make_colors(arena, ABF_COLOR_TRIM); //0xFF888888); // Filebar background color
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
    default_color_table.arrays[defcolor_text_default] = make_colors(arena, ABF_COLOR_TEXT);
    default_color_table.arrays[defcolor_comment] = make_colors(arena, 0xFF2090F0);
    default_color_table.arrays[defcolor_comment_pop] = make_colors(arena, 0xFF00A000, 0xFFA00000);
    default_color_table.arrays[defcolor_keyword] = make_colors(arena, ABF_COLOR_TEXT);// 0xFF249cff);// 0xFFD08F20); // if, return, typedef etc; might want to change this given the color of the PP and include colors
    default_color_table.arrays[abf_color_keyword_subset] = make_colors(arena, ABF_COLOR_TYPE);
    default_color_table.arrays[abf_primitive_highlight_type] = make_colors(arena, ABF_COLOR_TYPE);// 0xFF30E3E3);// 0xffaaaaef);
    default_color_table.arrays[abf_color_var_declaration] = make_colors(arena, ABF_COLOR_VAR_DECLARATION);

    default_color_table.arrays[abf_color_parens] = make_colors(arena, ABF_COLOR_PARENS);
    
    default_color_table.arrays[abf_primitive_highlight_function] = make_colors(arena, 0xFFFF9B05);// 0xffbbbbbb);
    default_color_table.arrays[abf_primitive_highlight_macro] = make_colors(arena, 0xFFA280E0);// 0xFF6516F7);// 0xFFA46391);
    default_color_table.arrays[abf_primitive_highlight_4coder_command] = make_colors(arena, 0xffffffff);

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
    
    default_color_table.arrays[defcolor_text_cycle] = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    default_color_table.arrays[defcolor_back_cycle] = make_colors(arena,
                                                                    // 1
                                                                    0xFF2C4D5C, // greyish teal
                                                                    
                                                                    // 2
                                                                    0xFF1c6336, // matte emerald

                                                                    // 3
                                                                    0xFF005B87, // cool misty blue

                                                                    // 4
                                                                    0xFF326352 // seafoam
                                                                );

    default_color_table.arrays[defcolor_line_numbers_back] = make_colors(arena, 0xFF101010);
    default_color_table.arrays[defcolor_line_numbers_text] = make_colors(arena, 0xFF404040);

    active_color_table = default_color_table;
}

function void
abf_render_caller(Application_Links* app, Frame_Info frame_info, View_ID view_id) {
    ProfileScope(app, "abf render caller");
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);

    f32 MarginWeight = 2.f;

    Rect_f32 view_rect = view_get_screen_rect(app, view_id);
    Rect_f32 region = rect_inner(view_rect, MarginWeight);
    
#if ABF_CUSTOMIZATIONS    
    // NOTE(brian): Draw background, margins
    draw_rectangle(app, region, 0.f, ABF_COLOR_BACKGROUND);
    if (is_active_view) {
        draw_margin(app, view_rect, region, ABF_COLOR_TRIM);
    }
    else {
        draw_margin(app, view_rect, region, ABF_COLOR_BACKGROUND);
    }
#else
    Rect_f32 region = draw_background_and_margin(app, view_id, is_active_view);
#endif
    
    Rect_f32 prev_clip = draw_set_clip(app, region);

    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;

    // NOTE(4cA): file bar
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

    // NOTE(4cA): query bars
    region = default_draw_query_bars(app, region, view_id, face_id);

    // NOTE(4cA): FPS hud
    if (show_fps_hud) {
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }

    // NOTE(4cA): layout line numbers
    Rect_f32 line_number_rect = {};
    if (global_config.show_line_number_margins) {
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }

    // NOTE(4cA): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

    // NOTE(4cA): draw line numbers
    if (global_config.show_line_number_margins) {
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }

    // NOTE(4cA): draw the buffer
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

    // NOTE(4cA): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    // NOTE(brian): reducing the 0.9 to 0.5 the cursor shape became sharper(less rounded)
    f32 cursor_roundness = (metrics.normal_advance * 0.5f) * 0.5f;// 0.9f;
    f32 mark_thickness = 2.f;

    // NOTE(4cA): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if (token_array.tokens != 0) {
#if ABF_SYNTAX_HIGHLIGHT
        abf_draw_cpp_token_colors(app, text_layout_id, &token_array, buffer);
#else
        draw_cpp_token_colors(app, text_layout_id, &token_array);
#endif

        // NOTE(4cA): Scan for TODOs and NOTEs
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

    // NOTE(4cA): Scope highlight
    if (global_config.use_scope_highlight) {
        Color_Array colors = finalize_color_array(defcolor_back_cycle);

#if ABF_CA
        // If the Context Alignment buffer hasn't been initialized
        abf_draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
#else
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
#endif

    }

    if (global_config.use_error_highlight || global_config.use_jump_highlight) {
        // NOTE(4cA): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if (global_config.use_error_highlight) {
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                fcolor_id(defcolor_highlight_junk));
        }

        // NOTE(4cA): Search highlight
        if (global_config.use_jump_highlight) {
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if (jump_buffer != compilation_buffer) {
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                    fcolor_id(defcolor_highlight_white));
            }
        }
    }

    // NOTE(4cA): Color parens
    if (global_config.use_paren_helper) {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(4cA): Line highlight
    if (global_config.highlight_line_at_cursor && is_active_view) {
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);

        draw_line_highlight(app, text_layout_id, line_number,
            fcolor_id(defcolor_highlight_cursor_line));
    }

    // NOTE(4cA): Whitespace highlight
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

#if ABF_CA
    // NOTE(brian): Draw enclosure headers here
    abf_draw_ca(app, abfCAContext);
#endif

    // NOTE(4cA): Cursor
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

    // NOTE(4cA): Fade ranges
    paint_fade_ranges(app, text_layout_id, buffer, view_id);

    // NOTE(4cA): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);

    draw_set_clip(app, prev_clip);
}

function void
abf_draw_scope_highlight(Application_Links* app, Buffer_ID buffer, Text_Layout_ID text_layout_id,
    i64 pos, ARGB_Color* colors, i32 color_count) {
    abf_draw_enclosures(app, text_layout_id, buffer,
        pos, FindNest_Scope, RangeHighlightKind_LineHighlight,
        colors, color_count, 0, 0);
}

function void
abf_draw_enclosures(Application_Links* app, Text_Layout_ID text_layout_id, Buffer_ID buffer,
    i64 pos, u32 flags, Range_Highlight_Kind kind,
    ARGB_Color* back_colors, i32 back_count,
    ARGB_Color* fore_colors, i32 fore_count) {
    Scratch_Block scratch(app);
    Range_i64_Array ranges = get_enclosure_ranges(app, scratch, buffer, pos, flags);

    ///
    // Ok... we're here because the program has been told to draw enclosures
    // this means we're going to loop through the enclosures in our scope;
    // therefore, grab the global values first, before the loop, then, in the loop
    // get the remaining data needed for filling the Context Alignment memory
    //
    // NOTE(brian): Global Context Alignment values needed for drawing
    abfCAContext->delta = V2f32(0.f, 1.f);
    abfCAContext->face = get_face_id(app, buffer);
    abfCAContext->AppTextLayoutRegion = text_layout_region(app, text_layout_id);
    abfCAContext->metrics = get_face_metrics(app, abfCAContext->face);
    abfCAContext->NumEnclosures = (ranges.count >= ABF_MAX_CA_ENCLOSURES) ?
        ABF_MAX_CA_ENCLOSURES : (ranges.count % ABF_MAX_CA_ENCLOSURES);
    abfCAContext->LayoutFunc = buffer_get_layout(app, buffer);
    ///

    /*
        NOTE(brian): For every scope enclosure (block), do this
    */
    i32 color_index = 0;
    for (i32 i = ranges.count - 1; i >= 0; i -= 1) {
        Range_i64 range = ranges.ranges[i];

        if (kind == RangeHighlightKind_LineHighlight) {
            Range_i64 r[2] = {};
            if (i > 0) {
                Range_i64 inner_range = ranges.ranges[i - 1];
                Range_i64 lines = get_line_range_from_pos_range(app, buffer, range);
                Range_i64 inner_lines = get_line_range_from_pos_range(app, buffer, inner_range);
                inner_lines.min = clamp_bot(lines.min, inner_lines.min);
                inner_lines.max = clamp_top(inner_lines.max, lines.max);
                inner_lines.min -= 1;
                inner_lines.max += 1;
                if (lines.min <= inner_lines.min) {
                    r[0] = Ii64(lines.min, inner_lines.min);
                }
                if (inner_lines.max <= lines.max) {
                    r[1] = Ii64(inner_lines.max, lines.max);
                }
            }
            else {
                r[0] = get_line_range_from_pos_range(app, buffer, range);
            }

            // For both ranges in r:Range_i64
            for (i32 j = 0; j < 2; j += 1) {
                if (r[j].min == 0) {
                    continue;
                }
                Range_i64 line_range = r[j];
                if (back_colors != 0) {
                    i32 back_index = color_index % back_count;
                    draw_line_highlight(app, text_layout_id, line_range, back_colors[back_index]);
                }
                if (fore_colors != 0) {
                    i32 fore_index = color_index % fore_count;
                    Range_i64 pos_range = get_pos_range_from_line_range(app, buffer, line_range);
                    paint_text_color(app, text_layout_id, pos_range, fore_colors[fore_index]);
                }
            }

            // If there is still room in the Context Alignment memory
            // Fill the corresponding scope metadata
            if (i < ABF_MAX_CA_ENCLOSURES) {
                f32 abfWidth = abfCAContext->AppTextLayoutRegion.x1 - abfCAContext->AppTextLayoutRegion.x0;
                Layout_Item_List LayoutList = abfCAContext->LayoutFunc(app, scratch, buffer, range, abfCAContext->face, abfWidth);
                abfCAContext->Enclosures[i].YPositionToStartDrawing = 2.f * LayoutList.first->items[0].padded_y1;

                Range_i64 abfCALineRange = (r[0].min == 0) ? r[1] : r[0];
                abfCAContext->Enclosures[i].Range = abfCALineRange;

                abf_get_header_string_u8(app, buffer,
                                            &abfCAContext->Enclosures[i],
                                            abfCAContext->face,
                                            abfCALineRange,
                                            abfWidth);

                f32 EnclosureIndentOffset = 2.f * abfCAContext->metrics.normal_advance;
                EnclosureIndentOffset += (f32)i * 4.f * abfCAContext->metrics.normal_advance;
                abfCAContext->Enclosures[i].EnclosureIndentOffset = EnclosureIndentOffset;

            }
        }
        else {
            if (back_colors != 0) {
                i32 back_index = color_index % back_count;
                draw_character_block(app, text_layout_id, range.min, 0.f, back_colors[back_index]);
                draw_character_block(app, text_layout_id, range.max - 1, 0.f, back_colors[back_index]);
            }
            if (fore_colors != 0) {
                i32 fore_index = color_index % fore_count;
                paint_text_color_pos(app, text_layout_id, range.min, fore_colors[fore_index]);
                paint_text_color_pos(app, text_layout_id, range.max - 1, fore_colors[fore_index]);
            }
        }
        color_index += 1;
    }
}

// NOTE: This function is a modification of 'draw_cpp_token_colors' from '4coder_draw.cpp'
function void
abf_draw_cpp_token_colors(Application_Links* app, Text_Layout_ID text_layout_id, Token_Array* array, Buffer_ID buffer) {

    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    i64 first_index = token_index_from_pos(array, visible_range.first);
    Token_Iterator_Array it = token_iterator_index(0, array, first_index);

    /* NOTE: Start modification. */
    Scratch_Block scratch(app);

#if 1
    code_index_lock();
#endif

    Temp_Memory notes_temp = begin_temp(scratch);
    primitive_highlight_hashes_notes_t hashes_notes = primitive_highlight_create_big_note_array(app, scratch);

    ProfileBlockNamed(app, "token loop", token_loop);
    /* NOTE: End modification. */

    for (; ; ) {

        Token* token = token_it_read(&it);

        if (token->pos >= visible_range.one_past_last) {
            break;
        }

        /* NOTE: Start modification. */
        FColor color = fcolor_id(defcolor_text_default);
        b32 colored = false;

        if (token->kind == TokenBaseKind_Identifier) {

            Temp_Memory temp = begin_temp(scratch);
            String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
            Code_Index_Note* note = primitive_highlight_get_note(app, &hashes_notes, lexeme);
            end_temp(temp);

            if (note) {
                switch (note->note_kind) {
                case CodeIndexNote_Type: {
                    color = fcolor_id(abf_primitive_highlight_type);
                } break;

                case CodeIndexNote_Function: {
                    color = fcolor_id(abf_primitive_highlight_function);
                } break;

                case CodeIndexNote_Macro: {
                    color = fcolor_id(abf_primitive_highlight_macro);
                } break;

                case CodeIndexNote_4coderCommand: {
                    /* NOTE: 4coder doesn't create those notes as of 4.1.6. */
                    color = fcolor_id(abf_primitive_highlight_4coder_command);
                } break;
                }

                colored = true;

#if 0
                if (note->note_kind == CodeIndexNote_Type) {

                    Token_Iterator_Array dot_arrow_it = it;

                    if (token_it_dec_non_whitespace(&dot_arrow_it)) {

                        Token* dot_arrow = token_it_read(&dot_arrow_it);

                        if (dot_arrow->kind == TokenBaseKind_Operator && (dot_arrow->sub_kind == TokenCppKind_Dot || dot_arrow->sub_kind == TokenCppKind_Arrow)) {
                            colored = false;
                        }
                    }
                }
#endif
            }
        }

        if (!colored) {
            // TODO(brian): move this preproc def into header file for 4coder_abf.cpp and include at the top of this file
#if ABF_SYNTAX_HIGHLIGHT
            color = abf_get_token_color_cpp(*token);
#else
            color = get_token_color_cpp(*token);
#endif
        }
        /* NOTE: End modification. */

        ARGB_Color argb = fcolor_resolve(color);
        paint_text_color(app, text_layout_id, Ii64_size(token->pos, token->size), argb);

        if (!token_it_inc_all(&it)) {
            break;
        }
    }

#if 1
    code_index_unlock();
#endif

    /* NOTE: Start modification. */
    ProfileCloseNow(token_loop);
    end_temp(notes_temp);
    /* NOTE: End modification. */
}

function FColor
abf_get_token_color_cpp(Token token) {
    Managed_ID color = defcolor_text_default;
    FColor ColorToBeReturned = {};
    switch (token.kind) {
    case TokenBaseKind_Preprocessor:
    {
        color = defcolor_preproc;
    }break;
    case TokenBaseKind_Keyword:
    {
        // NOTE(brian): 2020/12/3-keyword highlighting customization; primitive types to be highlighted differently
        color = abf_token_color(token);
    }break;
    case TokenBaseKind_Comment:
    {
        color = defcolor_comment;
    }break;
    case TokenBaseKind_LiteralString:
    {
        color = defcolor_str_constant;
    }break;
    case TokenBaseKind_LiteralInteger:
    {
        color = defcolor_int_constant;
    }break;
    case TokenBaseKind_LiteralFloat:
    {
        color = defcolor_float_constant;
    }break;
    // NOTE(brian): Commented out because it highlights basically everything
    /*
    case TokenBaseKind_Identifier:
    {
        color = abf_token_color(token);
    }; break;
    */
    case TokenBaseKind_ParentheticalOpen:
    case TokenBaseKind_ParentheticalClose:
    case TokenBaseKind_ScopeOpen:
    case TokenBaseKind_ScopeClose:
    case TokenBaseKind_StatementClose:
    {
        color = abf_color_parens;
    } break;

    default:
    {
        switch (token.sub_kind) {
        case TokenCppKind_LiteralTrue:
        case TokenCppKind_LiteralFalse:
        {
            color = defcolor_bool_constant;
        }break;
        case TokenCppKind_LiteralCharacter:
        case TokenCppKind_LiteralCharacterWide:
        case TokenCppKind_LiteralCharacterUTF8:
        case TokenCppKind_LiteralCharacterUTF16:
        case TokenCppKind_LiteralCharacterUTF32:
        {
            color = defcolor_char_constant;
        }break;
        case TokenCppKind_PPIncludeFile:
        {
            color = defcolor_include;
        }break;
        }
    }break;
    }
    ColorToBeReturned.id = (ID_Color)color;
    return(ColorToBeReturned);
}

function Managed_ID
abf_token_color(Token token) {
    switch (token.kind)
    {
    case TokenBaseKind_Keyword:
    {
        return abf_color_keyword(token.sub_kind);
    } break;
    case TokenBaseKind_Identifier:
    {
        return abf_color_identifier(token.sub_kind);
    } break;
    default:
    {
        return defcolor_text_default;
    }

    } // end switch
}

function Managed_ID
abf_color_keyword(Token_Base_Kind sub_kind) {
    /*
        NOTE(brian): Tokens from the buffer are assigned a "sub_kind", a sub type. Thankfully,
                some of the subtypes relate to c++ tokens and are already assigned by the core
                system (TODO(brian): find out where... i think maybe lexer_cpp.cpp)....
                I found out about the subtypes through some basic debugging:

                I put an int variable declaration at the top of the buffer, turned on a breakpoint
                at the switch statement from abf_get_token_color and looked at the token being handled,
                knew it was the integer (based on its "kind" attribute being set as 4 (TokenBaseKind_Keyword
                from 4coder_token.h), then saw that the sub_kind was set to 96...after looking more at the
                codebase, I found that the 96 matched the macro TokenCppKind_Int and assumed all c++ related
                tokens must be assigned a subtype from the TokenCppKind declaration (see lexer_cpp.h)....
                From there i just found the token sub kinds i wanted and put them in the switch statement below
    */

    switch (sub_kind)
    {
    case TokenCppKind_Void:
    case TokenCppKind_Bool:
    case TokenCppKind_Char:
    case TokenCppKind_Int:
    case TokenCppKind_Float:
    case TokenCppKind_Double:
    case TokenCppKind_Long:
    case TokenCppKind_Short:
    case TokenCppKind_Unsigned:
    case TokenCppKind_Signed:
    case TokenCppKind_Const:
    {
        return abf_color_keyword_subset;
    } break;

    default:
    {
        return defcolor_keyword;
    }break;

    } // end switch
}


function Managed_ID
abf_color_identifier(Token_Base_Kind sub_kind) {
    switch (sub_kind)
    {
    case TokenCppKind_Identifier:
    {
        // NOTE(brian): the intention of this is for variable names in their declaration to be highlighted with this color
        return abf_color_var_declaration;
    } break;

    default:
    {
        return defcolor_text_default;
    } break;

    } // end switch
}

/**
    Used by abf_page_up and abf_page_down commands (declared at top)
*/
function void
abf_page(Application_Links* app, View_ID view, Rect_f32 region, Buffer_ID Buffer, b32 PageUp) {

    i64 pos = view_get_cursor_pos(app, view);
    Buffer_Cursor cursor = view_compute_cursor(app, view, seek_pos(pos));
    f32 ViewHeight = rect_height(region);
    Buffer_Scroll ScrollBuffer = view_get_buffer_scroll(app, view);

    f32 LineHeight = get_view_line_height(app, view);
    i64 LinesInView = (i64)(ViewHeight / LineHeight);

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view);
    scroll.target.line_number = (PageUp) ? (cursor.line - LinesInView) : (cursor.line + LinesInView);
    scroll.target.pixel_shift.y = -ViewHeight * abfCenterMod;

    // If the line number exists, set the cursor and scroll
    // TODO(brian): gotta implement condition check for a line going over the max... just need to first figure out what the max is...
    if (scroll.target.line_number > 0) {

        Buffer_Seek seek = {};
        seek.type = buffer_seek_line_col;
        seek.line = scroll.target.line_number;
        seek.col = cursor.col;
        view_set_cursor(app, view, seek);

        view_set_buffer_scroll(app, view, scroll, SetBufferScroll_SnapCursorIntoView);
        no_mark_snap_to_cursor(app, view);
    }
}

function void
abf_init(Application_Links* app) {

#if ABF_CA
    // TODO(brian): remove when you've figured the CA bullshit out
    String_Const_u8 location = {};

    /**
        Context Alignment Initializations
    */
    /*
    String_Const_u8 abfManagedScopeGroupName = string_u8_litexpr(ABF_MANAGED_SCOPE_GROUP_NAME);

    String_Const_u8 abfManagedScopeCAName = string_u8_litexpr("ContextAlignment");
    */

    Base_Allocator* alloc = managed_scope_allocator(app, abf_ca_scope);
    u64 SizeReturned = 0;
    // String_Const_u8 location = string_u8_litexpr(ABF_MANAGED_SCOPE_GROUP_NAME); // no idea what this value is for
    location.size = sizeof(abf_ca_context);
    alloc->reserve((void*) abfCAContext, sizeof(abf_ca_context), &SizeReturned, location);

    // init to 0
    abfCAContext->face = 0;
    abfCAContext->LayoutFunc = 0;
    abfCAContext->NumEnclosures = 0;

    abfCAContext->isInitialized = true;
#endif
    return;
}
