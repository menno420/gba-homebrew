// Gloamline — arc slice 2 feasibility probe (NOT the game yet).
//
// The smallest ORIGINAL program that proves the two things the walking
// skeleton stands on: (1) the BlocksDS/libnds toolchain in this repo
// produces a bootable .nds, and (2) BOTH screens are initialized and
// rendering independently (top = future yard, bottom = future watch-map).
// A per-frame counter ticks on the bottom screen so a headless emulator
// can also prove the ROM is *running*, not just showing a static frame
// (tools/nds-headless-check.py asserts consecutive shots differ).
//
// 100% original content. Everything here is superseded by the slice-3
// walking skeleton; keep this file boring on purpose.

#include <nds.h>

#include <stdio.h>

int main(void)
{
    // Dual-screen console setup: one text console per engine.
    PrintConsole top;
    PrintConsole bottom;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                true /* main engine */, true);
    consoleInit(&bottom, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                false /* sub engine */, true);

    consoleSelect(&top);
    printf("\n\n  GLOAMLINE\n");
    printf("  ---------\n\n");
    printf("  the last lamplighter\n");
    printf("  holds the fence line.\n\n");
    printf("  top screen: the yard\n");
    printf("  (slice 2 feasibility\n");
    printf("   probe - no game yet)\n");

    consoleSelect(&bottom);
    printf("\n\n  WATCH-MAP (bottom)\n\n");
    printf("  dual-screen init: OK\n");
    printf("  night 0  zombies 0\n\n");

    unsigned int frame = 0;
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();

        frame++;

        // Tick a live counter on the watch-map: proves the main loop
        // runs and gives headless checks a frame-to-frame difference.
        consoleSelect(&bottom);
        printf("\x1b[10;2Hdawn in %u\x1b[K", frame);

        // Exit on START — but only once the pad has been seen idle, so an
        // emulator whose KEYINPUT register reads all-pressed (active-low
        // zero) before its first input latch cannot end the probe at boot.
        static bool pad_seen_idle = false;
        if (keysHeld() == 0)
            pad_seen_idle = true;
        if (pad_seen_idle && (keysDown() & KEY_START))
            break;
    }

    return 0;
}
