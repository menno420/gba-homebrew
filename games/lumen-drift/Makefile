#---------------------------------------------------------------------------------------------------------------------
# LUMEN DRIFT (game-lab Track B) — one-button gravity cave drifter.
# Game-specific identity only; all build glue is shared:
# games/common/butano-game.mak.
#---------------------------------------------------------------------------------------------------------------------
ROMTITLE    	:=  LUMEN DRIFT
ROMCODE     	:=  GLLD

# Butano re-invokes make from the build dir, so resolve the shared fragment
# to an absolute path once and export it (same trick butano.mak plays with
# LIBBUTANOABS).
ifndef GAMESCOMMONABS
	export GAMESCOMMONABS	:=	$(realpath ../common)
endif

include $(GAMESCOMMONABS)/butano-game.mak
