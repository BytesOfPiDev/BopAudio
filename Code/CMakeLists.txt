
# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: o3de_pal_dir will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/BopAudio/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/BopAudio/Code
o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_parent_relative_path}")

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Check to see if building the Gem Modules are supported for the current platform
if(NOT PAL_TRAIT_BOPAUDIO_SUPPORTED)
  return()
endif()


# The ${gem_name}.API target declares the common interface that users of this gem should depend on in their targets
ly_add_target(
    NAME ${gem_name}.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        bopaudio_api_files.cmake
        ${pal_dir}/bopaudio_api_files.cmake
    INCLUDE_DIRECTORIES
        INTERFACE
            Include
    BUILD_DEPENDENCIES
        INTERFACE
           AZ::AzCore
           Gem::AudioSystem.API
)

if("${BOPAUDIO_USEMINIAUDIO}")
    ly_target_link_libraries(${gem_name}.API INTERFACE Gem::MiniAudio.API)
    target_compile_definitions(${gem_name}.API INTERFACE USE_MINIAUDIO) 
    message("-- BOPAUDIO_USEMINIAUDIO is set to ON. BopAudio will manage MiniAudio components as appropriate.")
else ()
    message("-- BOPAUDIO_USEMINIAUDIO is set to OFF. BopAudio will not manage MiniAudio components.")
endif()

ly_add_target(
    NAME ${gem_name}.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        bopaudio_private_files.cmake
        ${pal_dir}/bopaudio_private_files.cmake
        bopaudio_autogen_files.cmake 
    INCLUDE_DIRECTORIES
        PRIVATE
            Include
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            ${gem_name}.API
            AZ::AzCore
            AZ::AzFramework
            Gem::AudioSystem
            Gem::ScriptCanvas
            Gem::ScriptCanvas.Extensions
    AUTOGEN_RULES
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeable_Header.jinja,$path/$fileprefix.generated.h
        *.ScriptCanvasNodeable.xml,ScriptCanvasNodeable_Source.jinja,$path/$fileprefix.generated.cpp
)

# Here add ${gem_name} target, it depends on the Private Object library and Public API interface
ly_add_target(
    NAME ${gem_name} ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        bopaudio_shared_files.cmake
        ${pal_dir}/bopaudio_shared_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.API
            Legacy::CryCommon
        PRIVATE
            Gem::${gem_name}.Static
)

# By default, we will specify that the above target ${gem_name} would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME ${gem_name}.Clients NAMESPACE Gem TARGETS Gem::${gem_name})
ly_create_alias(NAME ${gem_name}.Servers NAMESPACE Gem TARGETS Gem::${gem_name})
ly_create_alias(NAME ${gem_name}.Unified NAMESPACE Gem TARGETS Gem::${gem_name})

# For the Client and Server variants of ${gem_name} Gem, an alias to the ${gem_name}.API target will be made
ly_create_alias(NAME ${gem_name}.Clients.API NAMESPACE Gem TARGETS Gem::${gem_name}.API)
ly_create_alias(NAME ${gem_name}.Servers.API NAMESPACE Gem TARGETS Gem::${gem_name}.API)
ly_create_alias(NAME ${gem_name}.Unified.API NAMESPACE Gem TARGETS Gem::${gem_name}.API)

# Add in CMake dependencies for each gem dependency listed in this gem's gem.json file
# for the Clients, Servers and Unified gem variants
o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Clients Servers Unified)

# If we are on a host platform, we want to add the host tools targets like the ${gem_name}.Editor MODULE target
if(PAL_TRAIT_BUILD_HOST_TOOLS)
  # The ${gem_name}.Editor.API target can be used by other gems that want to interact with the ${gem_name}.Editor module
  ly_add_target(
        NAME ${gem_name}.Editor.API INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            bopaudio_editor_api_files.cmake
            ${pal_dir}/bopaudio_editor_api_files.cmake
        INCLUDE_DIRECTORIES
            INTERFACE
                Include
        BUILD_DEPENDENCIES
            INTERFACE
                AZ::AzToolsFramework
                Gem::AudioSystem.Editor.API
    )

  # The ${gem_name}.Editor.Private.Object target is an internal target
  # which is only to be used by this gems CMakeLists.txt and any subdirectories
  # Other gems should not use this target
  ly_add_target(
        NAME ${gem_name}.Editor.Static STATIC
        NAMESPACE Gem
        AUTOMOC
        AUTORCC
        FILES_CMAKE
            bopaudio_editor_private_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Include
                Source
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                AZ::AssetBuilderSDK
                Gem::${gem_name}.Static
                Gem::AudioSystem.API
                Gem::AudioSystem.Editor.API
                Gem::MiniAudio.Editor
                Legacy::CryCommon
    )

  ly_add_target(
        NAME ${gem_name}.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            bopaudio_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::${gem_name}.Editor.API
            PRIVATE
                Gem::${gem_name}.Editor.Static
    )

  # By default, we will specify that the above target ${gem_name} would be used by
  # Tool and Builder type targets when this gem is enabled.  If you don't want it
  # active in Tools or Builders by default, delete one of both of the following lines:
  ly_create_alias(NAME ${gem_name}.Tools    NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)
  ly_create_alias(NAME ${gem_name}.Builders NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)

  # For the Tools and Builders variants of ${gem_name} Gem, an alias to the ${gem_name}.Editor API target will be made
  ly_create_alias(NAME ${gem_name}.Tools.API NAMESPACE Gem TARGETS Gem::${gem_name}.Editor.API)
  ly_create_alias(NAME ${gem_name}.Builders.API NAMESPACE Gem TARGETS Gem::${gem_name}.Editor.API)

  # Add in CMake dependencies for each gem dependency listed in this gem's gem.json file
  # for the Tools and Builders gem variants
  o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Tools Builders)

endif()

# # # # # # # # # # # # ####################################################################
# Tests
# # # # # # # # # # # # ####################################################################


get_property(audiosystem_gem_root GLOBAL PROPERTY "@GEMROOT:AudioSystem@")

# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
  # We globally support tests, see if we support tests on this platform for ${gem_name}.Tests
  if(PAL_TRAIT_BOPAUDIO_TEST_SUPPORTED)
    # We support ${gem_name}.Tests on this platform, add dependency on the Private Object target
    ly_add_target(
           NAME ${gem_name}.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                bopaudio_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source
                    Include
                        ${audiosystem_gem_root}/Code/Tests/Mocks
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::${gem_name}.Static
                    Gem::AudioSystem.Tests
                    Legacy::CryCommon
        )

    # Add ${gem_name}.Tests to googletest
    ly_add_googletest(
            NAME Gem::${gem_name}.Tests
        )
  endif()

  # If we are a host platform we want to add tools test like editor tests here
  if(PAL_TRAIT_BUILD_HOST_TOOLS)
    # We are a host platform, see if Editor tests are supported on this platform
    if(PAL_TRAIT_BOPAUDIO_EDITOR_TEST_SUPPORTED)
      # We support ${gem_name}.Editor.Tests on this platform, add ${gem_name}.Editor.Tests target which depends on
      # private ${gem_name}.Editor.Private.Object target
      ly_add_target(
                NAME ${gem_name}.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    bopaudio_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                        Include
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::${gem_name}.Editor.Static
            )

      # Add ${gem_name}.Editor.Tests to googletest
      ly_add_googletest(
                NAME Gem::${gem_name}.Editor.Tests
            )
    endif()
  endif()
endif()
