
set(PAL_TRAIT_BOPAUDIO_SUPPORTED TRUE)
set(PAL_TRAIT_BOPAUDIO_TEST_SUPPORTED TRUE)
set(PAL_TRAIT_BOPAUDIO_EDITOR_TEST_SUPPORTED TRUE)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(STEAMAUDIO_LIBS ${gem_path}/3rdParty/SteamAudio/steamaudio_4.4.1/steamaudio/lib/linux-x86/libphonon.so)
else()
  set(STEAMAUDIO_LIBS ${gem_path}/3rdParty/SteamAudio/steamaudio_4.4.1/steamaudio/lib/linux-x64/libphonon.so)

  set(STEAMAUDIO_DLLS
  ${gem_path}/3rdParty/SteamAudio/steamaudio_4.4.1/steamaudio/lib/linux-x64/libphonon.so
)
endif()

