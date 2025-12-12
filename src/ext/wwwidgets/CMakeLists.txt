# wwWidgets library integration
# Pre-built library from wwWidgets project (https://github.com/bitbasher/wwWidgets-1.0-qt5.git)
# Branch: port-to-msys-mingw64

# Ensure Qt version matches
if(NOT USE_QT6)
  message(WARNING "wwWidgets library is built for Qt6, but OpenSCAD is configured for Qt5. This may cause compatibility issues.")
endif()

# Define the wwwidgets imported library
add_library(wwwidgets STATIC IMPORTED GLOBAL)

# Set library paths
set(WWWIDGETS_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(WWWIDGETS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(wwwidgets PROPERTIES
  IMPORTED_LOCATION "${WWWIDGETS_LIB_DIR}/libwwwidgets.a"
  INTERFACE_INCLUDE_DIRECTORIES "${WWWIDGETS_INCLUDE_DIR}"
)

# Note: Qt6 libraries will be linked automatically when OpenSCADLibInternal links both Qt6 and wwwidgets
# We don't link them here because Qt6 hasn't been found yet at this point in the build

message(STATUS "wwWidgets: ${WWWIDGETS_INCLUDE_DIR}")
message(STATUS "wwWidgets library: ${WWWIDGETS_LIB_DIR}/libwwwidgets.a")
