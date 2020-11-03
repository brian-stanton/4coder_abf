/*
4coder_default_map.cpp - Instantiate default bindings.
*/

// TOP

#include "..\4coder_default_include.cpp"

function void
setup_abf_mapping(Mapping* mapping, i64 global_id, i64 file_id, i64 code_id) {
    MappingScope();
    SelectMapping(mapping);

    /*
        Global scope/context bindings
    */
    SelectMap(global_id);
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
    Bind(keyboard_macro_start_recording, KeyCode_U, KeyCode_Control);
    Bind(keyboard_macro_finish_recording, KeyCode_U, KeyCode_Control, KeyCode_Shift);
    Bind(keyboard_macro_replay, KeyCode_U, KeyCode_Alt);
    Bind(change_active_panel, KeyCode_S, KeyCode_Alt); // new
    // good

    // Bind(change_active_panel_backwards, KeyCode_Comma, KeyCode_Control, KeyCode_Shift); // fix

    Bind(interactive_new, KeyCode_N, KeyCode_Control);
    Bind(interactive_open_or_new, KeyCode_O, KeyCode_Control);
    Bind(open_in_other, KeyCode_O, KeyCode_Alt);
    Bind(interactive_kill_buffer, KeyCode_F5); // new
    Bind(interactive_switch_buffer, KeyCode_B, KeyCode_Control); // new
    Bind(project_go_to_root_directory, KeyCode_R, KeyCode_Alt); // new
    Bind(save_all_dirty_buffers, KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(change_to_build_panel, KeyCode_F8); // new
    Bind(close_build_panel, KeyCode_F6); // new
    // good

    // TODO(brian): gotta figure out what jumps are and how they work
    /*
    Bind(goto_next_jump, KeyCode_N, KeyCode_Alt); // fix
    Bind(goto_prev_jump, KeyCode_N, KeyCode_Alt, KeyCode_Shift); // fix
    Bind(goto_first_jump, KeyCode_M, KeyCode_Alt, KeyCode_Shift); // fix
    */

    Bind(build_in_build_panel, KeyCode_M, KeyCode_Alt);
    // good

    // Bind(toggle_filebar, KeyCode_B, KeyCode_Alt); // fix

    Bind(execute_any_cli, KeyCode_Z, KeyCode_Alt);
    Bind(execute_previous_cli, KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
    Bind(command_lister, KeyCode_X, KeyCode_Alt);
    // good

    // Bind(project_command_lister, KeyCode_X, KeyCode_Alt, KeyCode_Shift); // fix

    Bind(list_all_functions_current_buffer_lister, KeyCode_B, KeyCode_Control, KeyCode_Shift); // new
    Bind(exit_4coder, KeyCode_F4, KeyCode_Alt);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(project_fkey_command, KeyCode_F3);
    Bind(project_fkey_command, KeyCode_F4);

    // NOTE(brian): 2020/11/03-Reserving F5 for killing buffers ("interactive_kill_buffer")
    // Bind(project_fkey_command, KeyCode_F5);

    // NOTE(brian): 2020/11/03-Reserving F6 for "close_build_panel"
    // Bind(project_fkey_command, KeyCode_F6);

    Bind(project_fkey_command, KeyCode_F7);

    // NOTE(brian): 2020/11/03-Reserving F8 for "change_to_build_panel"
    // Bind(project_fkey_command, KeyCode_F8);

    Bind(project_fkey_command, KeyCode_F9);
    Bind(project_fkey_command, KeyCode_F10);
    Bind(project_fkey_command, KeyCode_F11);
    Bind(project_fkey_command, KeyCode_F12);
    Bind(project_fkey_command, KeyCode_F13);
    Bind(project_fkey_command, KeyCode_F14);
    Bind(project_fkey_command, KeyCode_F15);
    Bind(project_fkey_command, KeyCode_F16);
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
    // good

    /*
        File scope/context bindings
    */
    SelectMap(file_id);
    ParentMap(global_id);
    BindTextInput(write_text_input);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);
    Bind(delete_char, KeyCode_Delete);
    Bind(backspace_char, KeyCode_Backspace);
    // good

    /*
        New navigation bindings
        2020/11/03 - 4:11am
    */
    // TODO(brian): Compile the default bindings from before for analysis of how they can best be rebound... a diagram of the keyboard and keys that is color coded for each function's purpose (text navigation, file navigation, deleting, etc.)
    Bind(move_up, KeyCode_K, KeyCode_Control);
    Bind(move_down, KeyCode_J, KeyCode_Control);
    Bind(move_left, KeyCode_H, KeyCode_Control);
    Bind(move_right, KeyCode_L, KeyCode_Control);
    Bind(seek_end_of_line, KeyCode_E, KeyCode_Control);
    Bind(seek_beginning_of_line, KeyCode_A, KeyCode_Control);
    Bind(page_up, KeyCode_V, KeyCode_Alt);
    Bind(page_down, KeyCode_V, KeyCode_Control);
    Bind(goto_beginning_of_file, KeyCode_PageUp);
    Bind(goto_end_of_file, KeyCode_PageDown);
    Bind(move_up_to_blank_line_end, KeyCode_K, KeyCode_Alt);
    Bind(move_down_to_blank_line_end, KeyCode_J, KeyCode_Alt);
    Bind(move_left_whitespace_boundary, KeyCode_H, KeyCode_Alt);
    Bind(move_right_whitespace_boundary, KeyCode_L, KeyCode_Alt);
    // All new

    // Bind(move_line_up, KeyCode_Up, KeyCode_Alt); // fix
    // Bind(move_line_down, KeyCode_Down, KeyCode_Alt); // fix

    Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Control);
    Bind(delete_alpha_numeric_boundary, KeyCode_Delete, KeyCode_Control);
    Bind(snipe_backward_whitespace_or_token_boundary, KeyCode_Backspace, KeyCode_Alt);
    Bind(snipe_forward_whitespace_or_token_boundary, KeyCode_Delete, KeyCode_Alt);
    Bind(set_mark, KeyCode_Space, KeyCode_Control);
    // good

    // Bind(replace_in_range, KeyCode_A, KeyCode_Control); // fix

    Bind(copy, KeyCode_C, KeyCode_Control);
    Bind(delete_range, KeyCode_D, KeyCode_Control);
    Bind(delete_line, KeyCode_D, KeyCode_Alt); // new
    Bind(center_view, KeyCode_I, KeyCode_Control);
    // good

    // Bind(left_adjust_view, KeyCode_E, KeyCode_Control, KeyCode_Shift); // fix
    
    Bind(search, KeyCode_F, KeyCode_Control);
    Bind(list_all_locations, KeyCode_F, KeyCode_Control, KeyCode_Shift);
    Bind(list_all_substring_locations_case_insensitive, KeyCode_F, KeyCode_Alt);
    Bind(goto_line, KeyCode_G, KeyCode_Control);
    // good

    // Bind(list_all_locations_of_selection, KeyCode_G, KeyCode_Control, KeyCode_Shift); // fix
    
    Bind(snippet_lister, KeyCode_BackwardSlash, KeyCode_Control); // new
    Bind(kill_buffer, KeyCode_K, KeyCode_Control, KeyCode_Shift);
    // good

    // NOTE(brian): 2020/11/03-Ctrl+L is now mapped to moving the cursor left; make sure to change the mapping if you include this
    // Bind(duplicate_line, KeyCode_L, KeyCode_Control); // fix

    Bind(cursor_mark_swap, KeyCode_M, KeyCode_Control);
    Bind(reopen, KeyCode_R, KeyCode_Control, KeyCode_Shift); // new
    Bind(query_replace, KeyCode_Q, KeyCode_Control);
    Bind(query_replace_identifier, KeyCode_Q, KeyCode_Control, KeyCode_Shift);
    Bind(query_replace_selection, KeyCode_Q, KeyCode_Alt); 
    // good

    // Bind(reverse_search, KeyCode_R, KeyCode_Control); // fix
    
    Bind(save, KeyCode_S, KeyCode_Control);
    Bind(save_all_dirty_buffers, KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(search_identifier, KeyCode_T, KeyCode_Control);
    Bind(list_all_locations_of_identifier, KeyCode_T, KeyCode_Control, KeyCode_Shift);
    Bind(paste_and_indent, KeyCode_Y, KeyCode_Control); // new
    Bind(paste_next_and_indent, KeyCode_Y, KeyCode_Control, KeyCode_Shift); // new
    Bind(cut, KeyCode_X, KeyCode_Control);
    Bind(redo, KeyCode_ForwardSlash, KeyCode_Control, KeyCode_Shift); // new
    Bind(undo, KeyCode_ForwardSlash, KeyCode_Control); // new
    Bind(view_buffer_other_panel, KeyCode_1, KeyCode_Control);
    Bind(swap_panels, KeyCode_2, KeyCode_Control);
    // good

    // Bind(if_read_only_goto_position, KeyCode_Return); // fix
    // Bind(if_read_only_goto_position_same_panel, KeyCode_Return, KeyCode_Shift); // fix

    // TODO(brian): REALLY need to figure out what jumps are
    // Bind(view_jump_list_with_lister, KeyCode_Period, KeyCode_Control, KeyCode_Shift); // fix

    /*
        Code scope/context keybindings
        // TODO(brian): figure out what 4coder calls these "contexts/scopes"
    */
    SelectMap(code_id);
    ParentMap(file_id);
    BindTextInput(write_text_and_auto_indent);
    Bind(move_left_alpha_numeric_boundary, KeyCode_H, KeyCode_Alt); // new
    Bind(move_right_alpha_numeric_boundary, KeyCode_L, KeyCode_Alt); // new
    // good

    // NOTE(brian): doesn't really hurt to keep these in here so I'm not taking them out
    Bind(move_left_alpha_numeric_or_camel_boundary, KeyCode_Left, KeyCode_Alt); // fix
    Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt); // fix
    // good for now

    Bind(comment_line_toggle, KeyCode_Semicolon, KeyCode_Control);
    Bind(word_complete, KeyCode_Tab);
    Bind(auto_indent_range, KeyCode_Tab, KeyCode_Control);
    Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
    Bind(word_complete_drop_down, KeyCode_Tab, KeyCode_Shift, KeyCode_Control);
    Bind(write_block, KeyCode_R, KeyCode_Alt);
    Bind(write_todo, KeyCode_T, KeyCode_Alt);
    Bind(write_note, KeyCode_N, KeyCode_Alt);
    // good

    // TODO(brian): Determine if these functions are worth having keybindings for long term
    // Bind(list_all_locations_of_type_definition, KeyCode_D, KeyCode_Alt); // fix
    // Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift); // fix

    Bind(open_long_braces, KeyCode_LeftBracket, KeyCode_Control);
    Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(open_long_braces_break, KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
    Bind(select_surrounding_scope, KeyCode_LeftBracket, KeyCode_Alt);
    Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Alt);
    Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Alt);
    Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
    Bind(place_in_scope, KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(delete_current_scope, KeyCode_Minus, KeyCode_Alt);
    Bind(if0_off, KeyCode_I, KeyCode_Alt);
    Bind(open_file_in_quotes, KeyCode_1, KeyCode_Alt);
    Bind(open_matching_file_cpp, KeyCode_2, KeyCode_Alt);
    Bind(write_zero_struct, KeyCode_0, KeyCode_Control);
    // good
}

// BOTTOM

