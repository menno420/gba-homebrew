#---------------------------------------------------------------------------------------------------------------------
# SHOAL (game-lab Track B, breadth program game #8) — flock-herding action puzzler (profiling-gated).
# Game-specific identity only; all build glue is shared:
# games/common/butano-game.mak.
#
# Text-only presentation (Butano common sprite fonts over a deep-water
# backdrop): no local graphics or include dirs, so both paths are pinned to
# the shared Butano common assets explicitly.
#---------------------------------------------------------------------------------------------------------------------
ROMTITLE    	:=  SHOAL
ROMCODE     	:=  GLSH
AUDIO       	:=  audio
INCLUDES    	:=  ../common/include ../../third_party/butano/common/include
GRAPHICS    	:=  ../../third_party/butano/common/graphics

# Butano re-invokes make from the build dir, so resolve the shared fragment
# to an absolute path once and export it (same trick butano.mak plays with
# LIBBUTANOABS).
ifndef GAMESCOMMONABS
	export GAMESCOMMONABS	:=	$(realpath ../common)
endif

include $(GAMESCOMMONABS)/butano-game.mak
