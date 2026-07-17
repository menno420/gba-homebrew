# Underroot (NDS) — dual-screen burrow-colony survival (arc slice 1).
#
# Thin wrapper over the BlocksDS default ARM9 ROM makefile — the exact
# pattern games/gloamline-nds and games/brineward-nds use: BlocksDS ships a
# maintained default_makefiles/rom_arm9 rule set (default ARM7 core, ndstool
# packing), and reusing it keeps this repo's surface to a variable block.
#
# Toolchain: tools/setup-nds-toolchain.sh (pinned BlocksDS + Wonderful GCC
# under /opt/wonderful). Build:  make -C games/underroot-nds

export BLOCKSDS		?= /opt/wonderful/thirdparty/blocksds/core
export WONDERFUL_TOOLCHAIN	?= /opt/wonderful

NAME		:= underroot-nds

GAME_TITLE	:= Underroot
GAME_SUBTITLE	:= dig deep, store enough
GAME_AUTHOR	:= gba-homebrew Track B

SOURCEDIRS	:= source

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
