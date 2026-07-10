/*
 * GAME LAB — TRACK B walking skeleton
 *
 * Minimal original Butano project: proves the pinned toolchain boots a ROM
 * that renders text sprites and animates them (a bouncing marquee on a
 * Lissajous path over a colored backdrop). All code original; font comes
 * from Butano's zlib-licensed common assets (third_party/butano/common).
 */

#include "bn_core.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_math.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "common_variable_8x8_sprite_font.h"

int main()
{
    bn::core::init();

    // Deep teal backdrop: unmistakably not a blank/black screen in the
    // headless screenshot (docs/proof/).
    bn::bg_palettes::set_transparent_color(bn::color(2, 10, 12));

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    text_generator.set_center_alignment();

    bn::vector<bn::sprite_ptr, 32> title_sprites;
    text_generator.generate(0, -56, "GAME LAB - TRACK B", title_sprites);

    bn::vector<bn::sprite_ptr, 32> subtitle_sprites;
    text_generator.generate(0, -40, "walking skeleton", subtitle_sprites);

    bn::vector<bn::sprite_ptr, 32> marquee_sprites;
    text_generator.generate(0, 24, "<BUTANO BOOTS HEADLESS>", marquee_sprites);

    // Remember each marquee sprite's rest position so the whole line can be
    // moved rigidly along the path.
    bn::vector<bn::fixed, 32> marquee_base_x;
    for(const bn::sprite_ptr& sprite : marquee_sprites)
    {
        marquee_base_x.push_back(sprite.x());
    }

    bn::fixed angle_x;
    bn::fixed angle_y;

    while(true)
    {
        // degrees_lut_* require angles inside [0, 360], so wrap both.
        angle_x += 2;

        if(angle_x >= 360)
        {
            angle_x -= 360;
        }

        angle_y += 4;

        if(angle_y >= 360)
        {
            angle_y -= 360;
        }

        bn::fixed offset_x = bn::degrees_lut_sin(angle_x) * 40;
        bn::fixed offset_y = bn::degrees_lut_cos(angle_y) * 16;

        for(int index = 0, limit = marquee_sprites.size(); index < limit; ++index)
        {
            marquee_sprites[index].set_x(marquee_base_x[index] + offset_x);
            marquee_sprites[index].set_y(24 + offset_y);
        }

        bn::core::update();
    }
}
