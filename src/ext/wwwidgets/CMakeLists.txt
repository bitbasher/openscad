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

# Link Qt6 libraries that wwwidgets depends on
target_link_libraries(wwwidgets INTERFACE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)

message(STATUS "wwWidgets: ${WWWIDGETS_INCLUDE_DIR}")
message(STATUS "wwWidgets library: ${WWWIDGETS_LIB_DIR}/libwwwidgets.a")
