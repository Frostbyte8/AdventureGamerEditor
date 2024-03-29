cmake_minimum_required(VERSION 2.8)

# DO NOT EDIT THIS FILE WITH AN EXTERNAL TOOL! Cmake is already awful, let's not make it worse!

# For some reason, using 2005 and specifying the location of the compilers does not
# set MSVC to 1. So do that here. TODO: Test this on another machine.

if("${CMAKE_GENERATOR}" MATCHES "Visual Studio 8 2005")
	if(NOT MSVC_VERSION)
		set (MSVC 1)
        set (MSVC2005 1)
        # For some reason, CMAKE refuses to set this for 2005, so we'll force it here.
        set(CMAKE_CXX_STANDARD_LIBRARIES "kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib")
	endif()
endif()

# Set the project name.
project(advedit)

#------------------------------------------------------------------------------
# Set output directories
#------------------------------------------------------------------------------

if(MSVC)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

#elseif(CMAKE_COMPILER_IS_GNUCXX)

    ## Default to a debug build for GCC

    #if(NOT CMAKE_BUILD_TYPE)
    #    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose Build Type: Debug or Release" FORCE)
    #endif(NOT CMAKE_BUILD_TYPE) 

    #if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    #    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/debug)
    #else()
    #    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/release)
    #endif()

endif(MSVC)

message("Output directory is ${CMAKE_BINARY_DIR}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

option(WIN9X_COMPAT "Windows 98/ME compatibility build? (Requires Visual C++ 2005)" NO)

option(USE_LUA "Use Lua to update Gitinfo and remove ALL_BUILD" YES)

if(MSVC)
    option(NO_ALLBUILD "Remove 'All Build' from project file (Requires LUA)" YES)
endif(MSVC)

#------------------------------------------------------------------------------
# Setup Platform independent sources and libraries
#------------------------------------------------------------------------------

set(SOURCES
    src/main.cpp
    src/gitinfo.h
    src/adventuregamer_constants.h
    src/editor_constants.cpp
    src/editor_constants.h
    src/credits_and_thanks.h   
)

if (EXISTS ${CMAKE_SOURCE_DIR}/src/gitinfo.h)
    set(SOURCES ${SOURCES} src/gitinfo.h)
endif()

file(GLOB JSON_LIB
    src/thirdparty/simpleson/*.h
    src/thirdparty/simpleson/*.cpp
)

file(GLOB INTERFACES
    src/interface/*.h
)

file(GLOB DATA_MODELS
    src/model/*.h
    src/model/*.cpp
)

file(GLOB CONTROLLERS
    src/controller/*.h
    src/controller/*.cpp
)

#------------------------------------------------------------------------------
# Setup Shared Windows resources
#------------------------------------------------------------------------------

file(GLOB WIN32_SOURCES
	src/win32/*.h
	src/win32/*.cpp
)

file(GLOB WIN32_RESOURCES
    src/resources/*.h
    src/resources/*.rc
)

#------------------------------------------------------------------------------
# Setup Windows with Win32++
#------------------------------------------------------------------------------

file(GLOB WIN_SOURCES
    src/win32pp/*.h
    src/win32pp/*.cpp
)

file(GLOB WIN_SOURCES_OBJECTDLG
    src/win32pp/objectdlg/*.h
    src/win32pp/objectdlg/*.cpp
)

file(GLOB WIN_SOURCES_CHARDLG
    src/win32pp/chardlg/*.h
    src/win32pp/chardlg/*.cpp
)

file(GLOB WIN_SOURCES_RESIZEDLG
    src/win32pp/resizedlg/*.h
    src/win32pp/resizedlg/*.cpp
)

file(GLOB WIN_SOURCES_STORYDLG
    src/win32pp/storydlg/*.h
    src/win32pp/storydlg/*.cpp
)

file(GLOB WIN_SOURCES_TILEDESCDLG
    src/win32pp/tiledescdlg/*.h
    src/win32pp/tiledescdlg/*.cpp
)

file(GLOB WIN_SOURCES_WORLDINFODLG
    src/win32pp/worldinfodlg/*.h
    src/win32pp/worldinfodlg/*.cpp
)

file(GLOB WIN_SOURCES_ABOUTINFODLG
    src/win32pp/about_dialog/*.h
    src/win32pp/about_dialog/*.cpp
)

#------------------------------------------------------------------------------
# Compatiblity Headers
#------------------------------------------------------------------------------

file(GLOB COMPAT
    src/compat/*.h
    src/compat/*.cpp
)

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

file(GLOB UTIL
    src/util/*.h
    src/util/*.cpp
)

#------------------------------------------------------------------------------
# TODO: Other toolkits such as GTK maybe. This is just from another project
# but I kept it here to remind myself.
#------------------------------------------------------------------------------

#if(WINAPI_BUILD)
#    file(GLOB WINBUILD_SOURCES
#        src/win32api/*.h
#        src/win32api/*.cpp
#    )
#    add_compile_definitions(USE_WINAPI)
#endif(WINAPI_BUILD)

#------------------------------------------------------------------------------
# Group all the sources for easy finding (MSVC only)
#------------------------------------------------------------------------------

SOURCE_GROUP("compat"               FILES ${COMPAT})
SOURCE_GROUP("controller"           FILES ${CONTROLLERS})
SOURCE_GROUP("model"                FILES ${DATA_MODELS})
SOURCE_GROUP("interfaces"           FILES ${INTERFACES})
SOURCE_GROUP("util"                 FILES ${UTIL})
SOURCE_GROUP("resources"            FILES ${WIN32_RESOURCES})
SOURCE_GROUP("thirdparty/simpleson" FILES ${JSON_LIB})
SOURCE_GROUP("win32pp"              FILES ${WIN_SOURCES})
SOURCE_GROUP("win32pp/objectdlg"    FILES ${WIN_SOURCES_OBJECTDLG})
SOURCE_GROUP("win32pp/chardlg"      FILES ${WIN_SOURCES_CHARDLG})
SOURCE_GROUP("win32pp/resizedlg"    FILES ${WIN_SOURCES_RESIZEDLG})
SOURCE_GROUP("win32pp/storydlg"     FILES ${WIN_SOURCES_STORYDLG})
SOURCE_GROUP("win32pp/tiledescdlg"  FILES ${WIN_SOURCES_TILEDESCDLG})
SOURCE_GROUP("win32pp/worldinfodlg" FILES ${WIN_SOURCES_WORLDINFODLG})
SOURCE_GROUP("win32"                FILES ${WIN32_SOURCES})

#if(WINAPI_BUILD)
#
#    SOURCE_GROUP("winapi"       FILES ${WINBUILD_SOURCES})
#
#else()
#
#    #SOURCE_GROUP("gtk"        FILES ${WINBUILD_SOURCES})
#
#endif()

#------------------------------------------------------------------------------
# Set required Libs (MSVC)
#------------------------------------------------------------------------------

# TODO: Globbing? Find_Library? Also this breaks on older VCs.

if(MSVC)
    set(
        WIN32_LIBS
        comctl32.lib
		Msimg32.lib
    )
endif()

#==============================================================================
# Set compiler specific things 
#==============================================================================

#------------------------------------------------------------------------------
# Shared Settings
#------------------------------------------------------------------------------

set(WIN32PP_INC_DIR "" CACHE PATH "Path to the win32++ include folder")

#------------------------------------------------------------------------------
# Compiler Specific things pertaining to Microsoft Visual C++
#------------------------------------------------------------------------------

if(MSVC)

    # TODO: Confirm that this works with a newer VC.
    # NOMINMAX is so std::max and std::min can be used
    add_definitions(-D_UNICODE -DNOMINMAX -D__WIN32PP__HELPERFUNCTIONS__)
    
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	
	# Get rid of "ZERO_CHECK" in the project file
	set(CMAKE_SUPPRESS_REGENERATION true)
   
    # We will provide our own Manifest File
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /MANIFESTUAC:NO")
    
    # VS2005 needs these, there does not appear to be a way to force it to generate debug info for some reason
    if(MSVC2005)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /Zi")
    endif(MSVC2005)

    #if(CMAKE_CONFIGURATION_TYPES STREQUAL "Debug")

        #TODO: differences for Debug/Release builds

        # Get the user to set the lib and include folders
        
        # Setup the executable
        add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${WIN32_SOURCES} ${WIN_SOURCES_OBJECTDLG} ${WIN_SOURCES_CHARDLG} ${WIN_SOURCES_RESIZEDLG} ${WIN_SOURCES_STORYDLG} ${WIN_SOURCES_TILEDESCDLG} ${WIN_SOURCES_WORLDINFODLG} ${WIN_SOURCES_ABOUTINFODLG} ${WIN32_RESOURCES} ${COMPAT} ${UTIL} ${JSON_LIB} ${COMPAT} ${INTERFACES} ${DATA_MODELS} ${CONTROLLERS} ${WIN_SOURCES} ${WINBUILD_SOURCES})
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "advedit")

        # Set the libs to link
        
        if(MSVC2005)
            # For some reason, the other line has problems with VS2005, causing it do say -lcomctl32.lib. Why? Who knows,
            # CMAKE is worse than Javascript.
            set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} ${WIN32_LIBS}")
        else()
            target_link_libraries(${PROJECT_NAME} ${WIN32_LIBS})
        endif()

        # Set the include directories
        target_include_directories(${PROJECT_NAME} PRIVATE ${WIN32PP_INC_DIR})

        # Get rid of the annoying ALL_BUILD project that gets added to the solution.
        if(USE_LUA)
            if(NO_ALLBUILD)
                execute_process(COMMAND "lua" strip_allbuild.lua "${CMAKE_CURRENT_BINARY_DIR}" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} )
                message(STATUS "Press Configure again after Generate to remove ALL_BUILD, or run lua strip_allbuild.lua")
            endif(NO_ALLBUILD)
        endif(USE_LUA)
    #endif()

    # If LUA is enabled, we can use it to generate the gitinfo.h header.
    # TODO: Release builds need to generate the info differently.

    if(USE_LUA)
        add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND lua "${CMAKE_CURRENT_LIST_DIR}/updategitinfo.lua" DEPENDS ${PROJECT_NAME} VERBATIM)
    endif(USE_LUA)
    
    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND XCOPY $(SolutionDir)..\\assets\\tileset.bmp $(TargetDir)tileset.bmp /Y /D)
    #add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND XCOPY $(SolutionDir)..\\assets\\lang_en.json $(TargetDir)lang_en.json /Y /D)

endif(MSVC)
