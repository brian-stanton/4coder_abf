
#ifndef _4CODER_ABF_SYNTAX_HIGHLIGHTING_H
#define _4CODER_ABF_SYNTAX_HIGHLIGHTING_H

typedef struct primitive_highlight_hashes_notes_t {
    u64* hashes;
    Code_Index_Note** notes;
    i32 count;
} primitive_highlight_hashes_notes_t;

function Code_Index_Note* primitive_highlight_get_note(Application_Links* app, primitive_highlight_hashes_notes_t* hashes_notes, String_Const_u8 name);
function primitive_highlight_hashes_notes_t primitive_highlight_create_big_note_array(Application_Links* app, Arena* arena);
function void primitive_highlight_quick_sort_hashes_notes(u64* hashes, Code_Index_Note** notes, u64 start, u64 end);

#endif
