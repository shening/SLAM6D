set(SHOW_LIBS glui scan ann newmat ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
if (WIN32)
  set(SHOW_LIBS ${SHOW_LIBS} XGetopt)
  set(SHOW_LIBS_S glui scan_s ann_s newmat_s XGetopt ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
else()
  set(SHOW_LIBS_S glui_s scan_s ann newmat ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
endif()

if(WITH_GLEE)
  set(SHOW_LIBS ${SHOW_LIBS} glee)
  set(SHOW_LIBS_S ${SHOW_LIBS_S} glee_s)
endif()

set(SHOW_SRCS NurbsPath.cc  PathGraph.cc vertexarray.cc  viewcull.cc colormanager.cc compacttree.cc scancolormanager.cc display.cc)

if (WITH_SHOW)
  add_executable(show show.cc ${SHOW_SRCS})
  target_link_libraries(show ${SHOW_LIBS})
endif()

if(WITH_WXSHOW)
  add_executable(wxshow wxshow.cc selectionframe.cc ${SHOW_SRCS})
  target_link_libraries(wxshow ${wxWidgets_LIBRARIES} wxthings ${SHOW_LIBS})
endif()

add_library(showstatic STATIC ${SHOW_SRCS})

### EXPORT SHARED LIBS 
if(EXPORT_SHARED_LIBS)
add_library(show_s SHARED ${SHOW_SRCS})
target_link_libraries(show_s ${Boost_LIBRARIES} ${SHOW_LIBS_S})
endif()
