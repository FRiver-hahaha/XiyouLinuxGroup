# Install script for directory: /home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetToolsExample

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample"
         RPATH "D:/Qt/5.15.2/msvc2019_64/lib:/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetTools/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample" TYPE EXECUTABLE FILES "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetToolsExample/ElaWidgetToolsExample")
  if(EXISTS "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample"
         OLD_RPATH "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools::::::::::::::::::::"
         NEW_RPATH "D:/Qt/5.15.2/msvc2019_64/lib:/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetTools/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetToolsExample/ElaWidgetToolsExample")
    endif()
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetToolsExample/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
