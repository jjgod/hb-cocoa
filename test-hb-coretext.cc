/* Example code of using harfbuzz together with Core Text APIs. */

#include <hb-coretext.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
	fprintf(stderr, "usage: %s <PostScript-Name> <text> [shaper]\n",
		argv[0]);
        return 1;
    }

    CFStringRef font_name =
        CFStringCreateWithCString(NULL, argv[1], kCFStringEncodingUTF8);
    if (!font_name || !CFStringGetLength(font_name)) {
        fprintf(stderr, "Given font name must be valid.\n");
        if (font_name)
            CFRelease(font_name);
        return 1;
    }

    CGFontRef cg_font = CGFontCreateWithFontName(font_name);
    CFRelease(font_name);
    font_name = NULL;

    if (!cg_font) {
        fprintf(stderr, "Cannot create font with name '%s'.\n", argv[1]);
        return 1;
    }

    hb_face_t* face = hb_coretext_face_create(cg_font);
    CGFontRelease(cg_font);
    unsigned int upem = hb_face_get_upem(face);

    hb_font_t *font = hb_font_create(face);
    hb_face_destroy(face);
    hb_font_set_scale(font, upem, upem);

    hb_coretext_font_set_funcs (font);

    hb_buffer_t *buffer = hb_buffer_create();

    hb_buffer_add_utf8(buffer, argv[2], -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    const char *shaper = argc > 3 ? argv[3] : "ot";
    const char* shapers[] = { shaper, NULL };
    hb_shape_full(font, buffer, NULL, 0, shapers);

    unsigned int count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *positions =
        hb_buffer_get_glyph_positions(buffer, NULL);

    for (unsigned int i = 0; i < count; i++)
    {
        hb_glyph_info_t *info = &infos[i];
        hb_glyph_position_t *pos = &positions[i];

        printf("cluster %d glyph 0x%x at (%d,%d)+(%d,%d)\n",
            info->cluster,
            info->codepoint,
            pos->x_offset,
            pos->x_offset,
            pos->x_advance,
            pos->y_advance);
    }

    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
    return 0;
}
