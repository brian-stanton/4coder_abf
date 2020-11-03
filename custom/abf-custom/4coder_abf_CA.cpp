/// 4coder_abf_CA.cpp
/// 
/// CA: context alignment
///

#include "4coder_abf_CA.h"

function
void abf_draw_CA(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id, Range_i64 lines, i64 pos, i32 enclosure_index, u32 flags) {

	Scratch_Block scratch(app);

	File_Attributes attributes = buffer_get_file_attributes(app, buffer);
	Face_ID face = get_face_id(app, buffer);
	Face_Metrics metrics = get_face_metrics(app, face);
	Glyph_Flag rotate90 = GlyphFlag_Rotate90;
	// See 4coder_fancy.cpp:draw_fancey_string__inner
	// line: 596 to see the original usage of this code
	//
	// determine "base_line" attribute
	//f32 base_line = 0.f;
	//if (face != 0) {
	//	base_line = Max(base_line, metrics.ascent);
	//}
	/************************************************/

	Rect_f32 AppTextLayoutRegion = text_layout_region(app, text_layout_id);
	f32 abf_width = AppTextLayoutRegion.x1 - AppTextLayoutRegion.x0;
	Layout_Function* LayoutFunc = buffer_get_layout(app, buffer);
	//Layout_Item_List LayoutList = LayoutFunc(app, scratch, buffer, { 0, buffer_get_line_count(app, buffer) }, face, abf_width);
	Layout_Item_List LayoutList = LayoutFunc(app, scratch, buffer, lines, face, abf_width);
	f32 LayoutItemY1 = LayoutList.first->items[0].padded_y1;

	// TODO(brian): conditionally set the "line" parameter in case the opening '{'
	//				is on "lines.min"... currently, if the opening '{' is on
	//				"lines.min", we'll get the index of the line before the scope header
	//
	//				e.g. 
	i64 BufferIndexOfScopeHeader = buffer_pos_from_relative_character(app,
																		buffer,
																		abf_width,
																		face,
																		lines.min,
																		0);
	i64 BufferIndexOfPrevLine = buffer_pos_from_relative_character(app,
																	buffer,
																	abf_width,
																	face,
																	lines.min - 1,
																	0);

	V2_i64 indicies = { BufferIndexOfScopeHeader, BufferIndexOfPrevLine };

	// TODO(brian): find a way to get the f32 height of the scope
	// 2020/08/20: DONE
	//f32 ScopeHeight = (f32)((lines.max - lines.min)) * metrics.line_height;

	// TODO(brian): Consider pulling this vvv out into it's own function
	// OR
	// *Code block comment header* would be good here
	// See "Project Ideas" page in OneNote notebook

	// NOTE(brian): for some reason we need to assign the values to the u8 string struct in this function (not read header line)
	// TODO(brian): Assign FinalHeaderString values using pass via reference
	String_Const_u8 FinalHeaderString = {};
	LexedHeader f = abf_read_header_line(app, buffer, indicies, indicies.lineIndex);
	FinalHeaderString.str = f.chars;
	FinalHeaderString.size = f.size;

	// Read the header line
	// see: buffer_read_range https://4coder.net/docs/custom_api_index.html#buffer_read_range
	
	/* delta: for example usage see 4coder_draw.cpp:63
	   I take this example to mean "advance text downwards"
	*/
	Vec2_f32 delta = V2f32(0.f, 1.f);

	/***************************************************************************/
	// Figure out the x offset for the scope header text that we are drawing
	//
	//
	// TODO(brian): !!! FIND OUT HOW TO GET THE NUMBER OF CHARS AN INDENT USES
	f32 EnclosureIndentOffset = 2.f * metrics.normal_advance;
	if (enclosure_index > 0) {
		// the number of characters in a tab (4) multiplied by the index 
		// of which enclosure we are in multiplied by the width of a character
		// gives us the final x offset for the string we are drawing
		EnclosureIndentOffset += (f32)enclosure_index * 4.f * metrics.normal_advance;
	}
	/***************************************************************************/

	/***************************************************************************/
	// Figure out the y offset for the scope header text that we are drawing
	//
	//
	/*
	i64 EnclosureFirstChar = buffer_pos_at_relative_xy(app,
														buffer,
														abf_width,
														face,
														lines.min,
														{ 0.f, 0.f });
														*/
	Rect_f32 EnclosureBox = view_padded_box_of_pos(app,
													get_active_view(app, 0),
													lines.min,
													pos);
	f32 YPositionToStartDrawing = 2.f * LayoutItemY1;// EnclosureBox.y0;
	 //*/
	/***************************************************************************/

	Vec2_f32 point = AppTextLayoutRegion.p0 + V2f32(EnclosureIndentOffset, YPositionToStartDrawing);
	//Vec2_f32 point = AppTextLayoutRegion.p0 + V2f32(EnclosureIndentOffset, 2.f);

	draw_string_oriented(app, face, /* color */ 0xffff0000 /* red */,
		FinalHeaderString, /*point*/ point,
		/*flags |*/ 0x00000000 | /*(rotate90 << 16) | (rotate90 << 8) |*/ rotate90,
		delta);
}

LexedHeader abf_read_header_line(Application_Links* app, Buffer_ID buffer, V2_i64 indicies, i64 selectedIndex) {
	LexedHeader gold = {};
	u8 ScopeHeader[ABF_MAX_CA_SIZE];
	if (buffer_read_range(app, buffer, Range_i64{ selectedIndex, selectedIndex + ABF_MAX_CA_SIZE }, ScopeHeader)) {
		gold.size = 0;
		for (i32 i = 0; i < ABF_MAX_CA_SIZE; i++) {
			u8 c = ScopeHeader[i];
			if (c == 13 /* '\r' */ || c == 10 /* '\n' */) {
				continue;
			}
			else if (ScopeHeader[i] == 123 /* '{' */) {

				if (i == 0) {
					return abf_read_header_line(app, buffer, indicies, indicies.prevLineIndex);
				}

				gold.chars[gold.size++] = c;
				break;
			}
			else {
				gold.chars[gold.size++] = c;
			}
		}
	}
	else {
		// TODO(brian): handle buffer read failure
	}
	return gold;
}

void abf_print_message(Application_Links* app, void* data, i32 length) {
	String_Const_u8 s = {};
	s.size = length;
	s.data = data;
	print_message(app, s);
}