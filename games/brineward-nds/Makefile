# Brineward (NDS) — walking-skeleton build (arc slice 2).
#
# Thin wrapper over the BlocksDS default ARM9 ROM makefile, exactly like
# games/gloamline-nds: BlocksDS ships a maintained
# default_makefiles/rom_arm9 rule set (default ARM7 core, ndstool
# packing), and reusing it keeps this game's surface to a variable block.
#
# Toolchain: tools/setup-nds-toolchain.sh (pinned BlocksDS + Wonderful
# GCC under /opt/wonderful). Build:  make -C games/brineward-nds

export BLOCKSDS		?= /opt/wonderful/thirdparty/blocksds/core
export WONDERFUL_TOOLCHAIN	?= /opt/wonderful

NAME		:= brineward-nds

GAME_TITLE	:= Brineward
GAME_SUBTITLE	:= sink or be sunk
GAME_AUTHOR	:= gba-homebrew Track B

SOURCEDIRS	:= source

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
