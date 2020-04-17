# CMakeList.txt: player 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("player")

include(./.SET_LOCAL_PATH.cmake)

######################################
#  FFmpeg lib setting
######################################
#make sure ffmpeg/bin is in your PATH to RUN.

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h)
find_library(AVFILTER_LIBRARY avfilter)

find_path(POSTPROC_INCLUDE_DIR libpostproc/postprocess.h)
find_library(POSTPROC_LIBRARY postproc)

find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)


######################################
#  SDL2 lib setting
######################################
#make sure SDL2.dll is in your PATH.

find_path(SDL_INCLUDE_DIR "SDL.h")
find_library(SDL_LIBRARY "SDL2.lib")

######################################
#  OpenAL lib setting
######################################

find_path(OpenAL_INCLUDE_DIR "al.h")
find_library(OpenAL_LIBRARY "OpenAL32.lib")



# 将源代码添加到此项目的可执行文件。
add_executable (${PROJECT_NAME}  
    "src/main.cpp" 
    "src/play.cpp" 
    "src/playVideo.cpp" 
    "src/playAudio.cpp" 
)

target_include_directories( ${PROJECT_NAME}  
	PRIVATE 
		${PROJECT_SOURCE_DIR}/include
		${AVCODEC_INCLUDE_DIR} 
		${AVFORMAT_INCLUDE_DIR} 
		${AVUTIL_INCLUDE_DIR} 
		${AVDEVICE_INCLUDE_DIR}
		${AVFILTER_INCLUDE_DIR}
		${POSTPROC_INCLUDE_DIR}
		${SWRESAMPLE_INCLUDE_DIR}
		${SWSCALE_INCLUDE_DIR}
		${SDL_INCLUDE_DIR}
		${OpenAL_INCLUDE_DIR}
)

target_link_libraries( ${PROJECT_NAME}  
	PRIVATE 
		${AVCODEC_LIBRARY} 
		${AVFORMAT_LIBRARY} 
		${AVUTIL_LIBRARY} 
		${AVDEVICE_LIBRARY}
		${AVFILTER_LIBRARY}
		${POSTPROC_LIBRARY}
		${SWRESAMPLE_LIBRARY}
		${SWSCALE_LIBRARY}
		${SDL_LIBRARY}
		${OpenAL_LIBRARY}

)

# TODO: 如有需要，请添加测试并安装目标。
