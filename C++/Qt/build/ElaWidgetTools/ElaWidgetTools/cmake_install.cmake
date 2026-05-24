# Install script for directory: /home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools

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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so"
         RPATH "D:/Qt/5.15.2/msvc2019_64/lib:/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetTools/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib" TYPE SHARED_LIBRARY FILES "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/libElaWidgetTools.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so"
         OLD_RPATH "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "D:/Qt/5.15.2/msvc2019_64/lib:/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/Install/ElaWidgetTools/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/include" TYPE FILE FILES
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaAcrylicUrlCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaActionCommander.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaAppBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaApplication.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaBreadcrumbBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaCalendar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaCalendarPicker.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaCheckBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaColorDialog.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaComboBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaContentDialog.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDef.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDialog.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDockWidget.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDoubleSpinBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDrawerArea.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaDxgiManager.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaEventBus.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaExponentialBlur.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaFlowLayout.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaGraphicsItem.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaGraphicsLineItem.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaGraphicsScene.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaGraphicsView.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaIcon.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaIconButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaImageCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaInteractiveCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaKeyBinder.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaLCDNumber.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaLineEdit.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaListView.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaLog.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaMenu.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaMenuBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaMessageBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaMessageButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaMultiSelectComboBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaNavigationBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPivot.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPlainTextEdit.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPopularCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaProgressBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaProgressRing.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPromotionCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPromotionView.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaProperty.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaPushButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaRadioButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaReminderCard.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaRoller.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaRollerPicker.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaScrollArea.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaScrollBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaScrollPage.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaScrollPageArea.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaSingleton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaSlider.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaSpinBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaStatusBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaSuggestBox.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaTabBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaTabWidget.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaTableView.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaText.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaTheme.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaToggleButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaToggleSwitch.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaToolBar.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaToolButton.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaToolTip.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaTreeView.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaWidget.h"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/ElaWidgetTools/ElaWidgetTools/ElaWindow.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake"
         "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/CMakeFiles/Export/0a725064e637b157163c54398f042a49/ElaWidgetToolsTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/CMakeFiles/Export/0a725064e637b157163c54398f042a49/ElaWidgetToolsTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/CMakeFiles/Export/0a725064e637b157163c54398f042a49/ElaWidgetToolsTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/ElaWidgetToolsConfig.cmake"
    "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/ElaWidgetToolsConfigVersion.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/friver/gitclone/XiyouLinuxGroup/C++/Qt/build/ElaWidgetTools/ElaWidgetTools/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
