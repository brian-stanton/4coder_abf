#ifndef FCODER_ABF_CA_H
#define FCODER_ABF_CA_H

#include "..\4coder_default_include.cpp"

#define ABF_MAX_CA_SIZE 30

struct LexedHeader {
	u8 chars[ABF_MAX_CA_SIZE];
	u64 size;
};

struct V2_i64 {
	i64 lineIndex;
	i64 prevLineIndex;
};

function void abf_draw_CA(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Range_i64 lines, i64 pos, i32 enclosure_index, u32 flags);
void abf_print_message(Application_Links *app, void* s, i32 length);
LexedHeader abf_read_header_line(Application_Links* app, Buffer_ID buffer, V2_i64 indicies, i64 selectedIndex);

#endif