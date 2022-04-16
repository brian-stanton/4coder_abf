
// TODO(brian): Figure out if I actually need to take these out. I'm thinking the condition evaluation
//				might result in the id definitions not being included in 4coder_default_colors.h by the preprocessor
#ifndef _4CODER_ABF_COLORS
#define _4CODER_ABF_COLORS

// NOTE(brian): Custom colors for syntax highlighting; added 2020/11/12
CUSTOM_ID(colors, abf_primitive_highlight_type);
CUSTOM_ID(colors, abf_primitive_highlight_function);
CUSTOM_ID(colors, abf_primitive_highlight_macro);
CUSTOM_ID(colors, abf_primitive_highlight_4coder_command);

// NOTE(brian): More custom colors for syntax highlighting; added 2020/12/03
CUSTOM_ID(colors, abf_color_keyword_subset);
CUSTOM_ID(colors, abf_color_var_declaration);
CUSTOM_ID(colors, abf_color_parens);

// NOTE(brian): 2020/12/07
CUSTOM_ID(colors, abf_color_active);
CUSTOM_ID(colors, abf_color_background);

#endif
