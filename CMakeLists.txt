cmake_minimum_required(VERSION 2.8)
project( carTracking )
find_package( OpenCV  REQUIRED )
find_package( cvBlob  REQUIRED )
add_executable(carTracking  src/CarTracking.cpp src/tracker/BlobTracking.cpp 
src/motion/MotionTracker.cpp )
target_link_libraries(carTracking ${OpenCV_LIBS})
target_link_libraries(carTracking ${cvBlob_LIBS})
