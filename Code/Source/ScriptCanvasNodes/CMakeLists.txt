
# Query the gem name from the gem.json file if possible
# otherwise fallback to using ${GemName}
o3de_find_ancestor_gem_root(gem_path gem_name "${CMAKE_CURRENT_SOURCE_DIR}")
if (NOT gem_name)
  set(gem_name "${GemName}")
endif()

ly_add_target(
    NAME ScriptCanvasNodes.Object OBJECT
    NAMESPACE ScriptCanvasNodes
    FILES_CMAKE
        ScriptCanvasNodes_files.cmake
    INCLUDE_DIRECTORIES
        PRIVATE
            Source
            ../../Source
        PUBLIC
            Include
    BUILD_DEPENDENCIES
        PUBLIC
            AZ::AzCore
            AZ::AzFramework
            Gem::AudioSystem.API
            Gem::BopAudio
            Gem::ScriptCanvas
            Gem::ScriptCanvas.Extensions
    AUTOGEN_RULES
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeable_Header.jinja,$path/$fileprefix.generated.h
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeable_Source.jinja,$path/$fileprefix.generated.cpp
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeableRegistry_Header.jinja,$path/$fileprefix_Nodeables.generated.h
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeableRegistry_Source.jinja,$path/$fileprefix_Nodeables.generated.cpp
)

set (dependent_targets ${gem_name}.Private.Object;${gem_name};${gem_name}.Editor)

foreach (dependent_target IN LISTS dependent_targets)
  if (TARGET ${dependent_target})
    target_link_libraries(${dependent_target} PRIVATE ScriptCanvasNodes::ScriptCanvasNodes.Object)
  else()
    message(FATAL_ERROR "The target ${dependent_target} was not found, if the gem was not created using scripts/o3de create-gem"
            " you may update dependent_targets to match the corresponding targets in your gem")
  endif()
endforeach()
