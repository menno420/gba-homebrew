#---------------------------------------------------------------------------------------------------------------------
# WICKROAD (game-lab Track B, breadth program game #11) — market-route trading loop.
# Game-specific identity only; all build glue is shared:
# games/common/butano-game.mak.
#
# Presentation (crossroads cut 2, the sprite art pass): local graphics/ holds
# the original procedurally-generated tile/palette BMPs (see
# graphics/generate_assets.py) for the authored regular_bg baked from sim
# state; headers/title/end text still uses Butano's common variable 8x8
# sprite font (the one tools/headless-screenshot.py --assert-text can read),
# so the shared common graphics dir stays on the path too.
#---------------------------------------------------------------------------------------------------------------------
ROMTITLE    	:=  WICKROAD
ROMCODE     	:=  GLWR
AUDIO       	:=  audio
INCLUDES    	:=  ../../third_party/butano/common/include
GRAPHICS    	:=  graphics ../../third_party/butano/common/graphics

# Growth cut 2 measured fact: with the pact line on screen, a dawn
# redraw's regeneration peak (old line sprites not yet released + the
# new generation) exhausts Butano's default 128 sprite-tiles items —
# the v0.3 P2 first-probe froze on a "No more sprite tiles items
# available" assert at the day-5 dawn. Text-only game, plenty of IWRAM
# headroom: double the item pool.
USERFLAGS   	:=  -DBN_CFG_SPRITE_TILES_MAX_ITEMS=256

# Butano re-invokes make from the build dir, so resolve the shared fragment
# to an absolute path once and export it (same trick butano.mak plays with
# LIBBUTANOABS).
ifndef GAMESCOMMONABS
	export GAMESCOMMONABS	:=	$(realpath ../common)
endif

include $(GAMESCOMMONABS)/butano-game.mak
