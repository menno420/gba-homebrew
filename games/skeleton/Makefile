#---------------------------------------------------------------------------------------------------------------------
# GAME LAB — TRACK B walking skeleton (original Butano project).
# Derived from Butano's project template (zlib license); Butano itself is a
# pinned clone at third_party/butano (tools/setup-toolchain.sh).
#
# Build:
#   tools/setup-toolchain.sh                       (once; idempotent)
#   export DEVKITPRO=/opt/devkitpro DEVKITARM=$DEVKITPRO/devkitARM
#   export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
#   make -C games/skeleton -j$(nproc)
# Output: games/skeleton/skeleton.gba
#---------------------------------------------------------------------------------------------------------------------
TARGET      	:=  $(notdir $(CURDIR))
BUILD       	:=  build
LIBBUTANO   	:=  ../../third_party/butano/butano
PYTHON      	:=  python3
SOURCES     	:=  src
INCLUDES    	:=  include ../../third_party/butano/common/include
DATA        	:=
GRAPHICS    	:=  graphics ../../third_party/butano/common/graphics
AUDIO       	:=
AUDIOBACKEND	:=  maxmod
AUDIOTOOL		:=
DMGAUDIO    	:=
DMGAUDIOBACKEND	:=  default
ROMTITLE    	:=  GAMELAB TRKB
ROMCODE     	:=  GLTB
USERFLAGS   	:=
USERCXXFLAGS	:=
USERASFLAGS 	:=
USERLDFLAGS 	:=
USERLIBDIRS 	:=
USERLIBS    	:=
DEFAULTLIBS 	:=
STACKTRACE		:=
USERBUILD   	:=
EXTTOOL     	:=

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
