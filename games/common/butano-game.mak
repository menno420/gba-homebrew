#---------------------------------------------------------------------------------------------------------------------
# games/common/butano-game.mak — shared Butano project glue (GENERIC, game-agnostic).
#
# A game's Makefile sets its identity (ROMTITLE/ROMCODE) and any overrides,
# then includes this fragment. Everything here transfers unchanged to any
# concept (Lumen Drift / Clockwork Courier / Shoal / ...):
#
#     ROMTITLE := MY GAME
#     ROMCODE  := GLXX
#     include ../common/butano-game.mak
#
# Build (from the repo root):
#     tools/setup-toolchain.sh                       (once; idempotent)
#     export DEVKITPRO=/opt/devkitpro
#     export DEVKITARM=$DEVKITPRO/devkitARM
#     export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
#     make -C games/<game> -j$(nproc)
# Output: games/<game>/<game>.gba
#---------------------------------------------------------------------------------------------------------------------
TARGET      	?=  $(notdir $(CURDIR))
BUILD       	?=  build
LIBBUTANO   	?=  ../../third_party/butano/butano
PYTHON      	?=  python3
SOURCES     	?=  src
INCLUDES    	?=  include ../common/include ../../third_party/butano/common/include
DATA        	?=
GRAPHICS    	?=  graphics ../../third_party/butano/common/graphics
AUDIO       	?=
AUDIOBACKEND	?=  maxmod
AUDIOTOOL		?=
DMGAUDIO    	?=
DMGAUDIOBACKEND	?=  default
ROMTITLE    	?=  GAME LAB
ROMCODE     	?=  GLTB
USERFLAGS   	?=
USERCXXFLAGS	?=
USERASFLAGS 	?=
USERLDFLAGS 	?=
USERLIBDIRS 	?=
USERLIBS    	?=
DEFAULTLIBS 	?=
STACKTRACE		?=
USERBUILD   	?=
EXTTOOL     	?=

#---------------------------------------------------------------------------------------------------------------------
# Export absolute butano path:
#---------------------------------------------------------------------------------------------------------------------
ifndef LIBBUTANOABS
	export LIBBUTANOABS	:=	$(realpath $(LIBBUTANO))
endif

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak
