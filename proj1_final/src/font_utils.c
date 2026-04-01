#include "font_utils.h"
#include <stdio.h>

const char *find_font_path(void) {
    static const char *candidates[] = {
        "assets/DejaVuSans.ttf",
        "assets/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        NULL
    };
    for (int i = 0; candidates[i] != NULL; ++i) {
        FILE *fp = fopen(candidates[i], "rb");
        if (fp) {
            fclose(fp);
            return candidates[i];
        }
    }

    return NULL;
}