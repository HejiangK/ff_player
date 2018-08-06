
set( OPENAL_FOUND "NO" )

find_path( OPENAL_INCLUDE_DIR al.h
        HINTS
        PATH_SUFFIXES include
        PATHS
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/include
        /usr/include
        /sw/include
        /opt/local/include
        /opt/csw/include
        /opt/include
        /mingw
        ${THIRD_PATH}/openAL/include
        )


find_library( OPENAL_LIBRARY
        NAMES OpenAL OpenAL32
        HINTS
        PATH_SUFFIXES lib64 lib
        PATHS
        /usr/local
        /usr
        /sw
        /opt/local
        /opt/csw
        /opt
        /mingw
        ${THIRD_PATH}/openAL/lib
        )

if(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)
    set( OPENAL_FOUND "YES" )
endif()
