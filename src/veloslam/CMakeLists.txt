set(SHOW_LIBS ${OPENGL_LIBRARIES} glui scan ann)
if(WIN32)
  set(SHOW_LIBS ${SHOW_LIBS} ${GLUT_LIBRARIES} XGetopt)
endif()
if (UNIX)
  set(SHOW_LIBS newmat dl ${GLUT_LIBRARIES} ${SHOW_LIBS})
endif()

if(WITH_VELOSLAM)
  add_executable(veloslam veloslam.cc veloscan.cc debugview.cc pcddump.cc tracker.cc
   trackermanager.cc drawtrackers.cc kalmanfilter.cc matrix.cc lap.cc)

if(UNIX)
  target_link_libraries(veloslam dl scan newmat cxsparse ann ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${SHOW_LIBS})
endif()

if(WIN32)
  target_link_libraries(veloslam scan newmat cxsparse ann XGetopt ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${SHOW_LIBS})
endif()
endif()

#if(WITH_VELOSLAM)
 #  set(SHOW_SRCS ../show/NurbsPath.cc ../show/PathGraph.cc ../show/vertexarray.cc
 #      ../show/viewcull.cc ../show/colormanager.cc ../show/compacttree.cc
 #      ../show/scancolormanager.cc ../show/display.cc)

 # add_executable(veloshow veloshow.cc veloscan.cc
 #     debugview.cc  pcddump.cc cluster_classification.cc
 #     tracker.cc  trackermanager.cc drawtrackers.cc
 #     svm.cc  clusterboundingbox.cc multiscan_random_field.cc
 #     kalmanfilter.cc matrix.cc lap.cc 
 #     intersection_detection.cc SegIter.model ${SHOW_SRCS})

#if(UNIX)
#  target_link_libraries(veloshow dl scan newmat sparse ANN  ${SHOW_LIBS})
#endif()

#if(WIN32)
#  target_link_libraries(veloshow scan newmat sparse ANN XGetopt ${Boost_LIBRARIES}  ${SHOW_LIBS})
#endif()

#endif()



