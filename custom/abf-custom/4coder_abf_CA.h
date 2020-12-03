#ifndef _4CODER_ABF_CA_H
#define _4CODER_ABF_CA_H

#define ABF_MAX_CA_SIZE 30 // size of the longest string allowed in a ca enclosure item
#define ABF_MAX_CA_ENCLOSURES 10

struct abf_lexed_header {
	u8 chars[ABF_MAX_CA_SIZE];
	u64 size;
};

struct V2_i64 {
	i64 lineIndex;
	i64 prevLineIndex;
};

struct abf_ca_enclosure {	
	/**
		NOTE(brian): Given the use of this struct in an array in the
		abf_ca_context struct, the EnclosureRank is actually implicit
		in the enclosure struct's position/index in the array.

		NOTE(brian): This design approach DEPENDS ON all accesses to
		the struct going through the global interface abfCAContext.Enclosures
		- an array of abf_ca_enclosure objects - by referencing the
		enclosure via its index/key to the array (aka its rank)
	*/
	// i8 EnclosureRank; // 0 is highest
	Range_i64 Range;
	String_Const_u8 EnclosureHeader;
	f32 EnclosureIndentOffset;
	f32 YPositionToStartDrawing;
};

struct abf_ca_context {
	b32 isInitialized = false;
	Face_ID face;
	Rect_f32 AppTextLayoutRegion;
	Face_Metrics metrics;
	Vec2_f32 delta;
	Layout_Function* LayoutFunc;

	int NumEnclosures;
	abf_ca_enclosure Enclosures[ABF_MAX_CA_ENCLOSURES];
};

function void abf_draw_CA(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Range_i64 lines, i64 pos, i32 enclosure_index, u32 flags);
function abf_lexed_header abf_read_header_line(Application_Links* app, Buffer_ID buffer, V2_i64 indicies, i64 indiciesLine);
function void abf_get_header_string_u8(Application_Links* app, Buffer_ID buffer, abf_ca_enclosure* Enclosure, Face_ID face, Range_i64 lines, f32 TextRegionWidth);
function void abf_draw_ca(Application_Links* app);

void abf_print_message(Application_Links* app, void* s, i32 length);

#endif