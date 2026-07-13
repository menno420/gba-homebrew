#---------------------------------------------------------------------------------------------------------------------
# CINDERVAULT (game-lab Track B, breadth program game #6) — turn-based dungeon-dive roguelike.
# Game-specific identity only; all build glue is shared:
# games/common/butano-game.mak.
#
# Text-only presentation (Butano common sprite fonts: fixed 8x8 for the map
# glyph rows, variable 8x8 for HUD/title): no local graphics or include
# dirs, so both paths are pinned to the shared Butano common assets
# explicitly.
#---------------------------------------------------------------------------------------------------------------------
ROMTITLE    	:=  CINDERVAULT
ROMCODE     	:=  GLCV
INCLUDES    	:=  ../../third_party/butano/common/include
GRAPHICS    	:=  ../../third_party/butano/common/graphics

# Butano re-invokes make from the build dir, so resolve the shared fragment
# to an absolute path once and export it (same trick butano.mak plays with
# LIBBUTANOABS).
ifndef GAMESCOMMONABS
	export GAMESCOMMONABS	:=	$(realpath ../common)
endif

include $(GAMESCOMMONABS)/butano-game.mak
