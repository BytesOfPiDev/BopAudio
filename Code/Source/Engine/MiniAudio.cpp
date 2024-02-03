#include "AzCore/PlatformDef.h"
AZ_PUSH_DISABLE_WARNING(4701 4244 4456 4457 4245, "-Wuninitialized")

extern "C" {

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c> // Enables Vorbis decoding.

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

// The stb_vorbis implementation must come after the implementation of miniaudio.
#undef STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
}

AZ_POP_DISABLE_WARNING
