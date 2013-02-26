cmake_minimum_required (VERSION 2.6)
project (gamma)
FIND_PACKAGE( OpenCV REQUIRED )

ADD_EXECUTABLE( ${PROJECT_NAME} main.cpp FrameList.cpp Frame.cpp)
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OpenCV_LIBS} )
