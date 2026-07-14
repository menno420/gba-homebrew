#---------------------------------------------------------------------------------------------------------------------
# DEEPCAST (game-lab Track B, breadth program game #4) — fishing / line-tension-management arcade.
# Game-specific identity only; all build glue is shared:
# games/common/butano-game.mak.
#
# Presentation (growth cut 5, the art pass): local graphics/ holds the
# original procedurally-generated lake/fish/rod BMPs (see
# graphics/generate_assets.py); HUD/title text still uses Butano's common
# variable 8x8 sprite font (the one tools/headless-screenshot.py
# --assert-text can read).
#---------------------------------------------------------------------------------------------------------------------
ROMTITLE    	:=  DEEPCAST
ROMCODE     	:=  GLDC
AUDIO       	:=  audio
INCLUDES    	:=  ../common/include ../../third_party/butano/common/include
GRAPHICS    	:=  graphics ../../third_party/butano/common/graphics

# Butano re-invokes make from the build dir, so resolve the shared fragment
# to an absolute path once and export it (same trick butano.mak plays with
# LIBBUTANOABS).
ifndef GAMESCOMMONABS
	export GAMESCOMMONABS	:=	$(realpath ../common)
endif

include $(GAMESCOMMONABS)/butano-game.mak
