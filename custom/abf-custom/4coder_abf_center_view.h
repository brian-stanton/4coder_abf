
//#ifndef FCODER_ABF_CENTER_VIEW_H
#define FCODER_ABF_CENTER_VIEW_H

#include "..\4coder_default_include.cpp"

// NOTE(brian): CVC is center view context
#define ABF_CVC_OFF 0x04
#define ABF_CVC_CENTER 0x00
#define ABF_CVC_TOP 0x01
#define ABF_CVC_BOTTOM 0x02

// TODO(brian): figure out if it's a better idea to define this struct as a type
struct abf_center_view_context {
	i64 CursorLine; // line number

	// NOTE(brian): CurrentState holds one of four values (each representing a distinct state): OFF, CENTER, TOP, BOTTOM
	// that say where we are in the cycle of Center->Top->Bottom. When <center_view> is invoked (and we aren't already in a center_view-loop; i.e. the state is equal to Off)
	// the buffer region's view that we are in centers on the line the cursor is on; if <center_view> is invoked again, we move
	// to the next state in the center_view-loop and the line we centered on the first time <center_view> was used is
	// oriented to the TOP; further, if <center_view> is invoked again (without the cursor changing lines during the loop)
	// the line we originally centered on is oriented to the BOTTOM; if <center_view> is invoked once more the original line
	// that was centered on is centered again and the loop continues. This feature is the same as the Emacs center feature
	i8 CurrentState;

	Buffer_Scroll ScrollBuffer;

};

const f32 abfTopMod = 0.0f;
const f32 abfCenterMod = 0.5f;
const f32 abfBottomMod = 1.0f;

static i64 CVLastLine;
static i8 CVCurrentState;

//#endif FCODER_ABF_CENTER_VIEW_H
