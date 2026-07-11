# Gloamline (NDS) — walking-skeleton build (arc slice 3).
#
# Thin wrapper over the BlocksDS default ARM9 ROM makefile: BlocksDS
# ships a maintained default_makefiles/rom_arm9 rule set (default ARM7
# core, ndstool packing), and reusing it keeps this repo's surface to a
# variable block — the exact pattern BlocksDS templates use.
#
# Toolchain: tools/setup-nds-toolchain.sh (pinned BlocksDS + Wonderful
# GCC under /opt/wonderful). Build:  make -C games/gloamline-nds

export BLOCKSDS		?= /opt/wonderful/thirdparty/blocksds/core
export WONDERFUL_TOOLCHAIN	?= /opt/wonderful

NAME		:= gloamline-nds

GAME_TITLE	:= Gloamline
GAME_SUBTITLE	:= hold the fence line
GAME_AUTHOR	:= gba-homebrew Track B

SOURCEDIRS	:= source

# Perf-stress build (CI-only, NEVER shipped): `make GL_STRESS=1` spawns the
# full 24-Shambler cap at frame 0 with death disarmed, so a headless run
# measures the honest worst-case frame cost via the GL_T_VLINES/GL_T_VLMAX
# telemetry. Always `make clean` between shipped and stress builds.
ifeq ($(GL_STRESS),1)
DEFINES		:= -DGL_STRESS
endif

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
