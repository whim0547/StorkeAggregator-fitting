cmake_minimum_required(VERSION 3.1)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# ここは各自で設定してください
# インクルードパスの設定(これもそれぞれの環境に合わせる)
# SET(OpenCV_DIR /usr/local/Cellar/opencv/4.1.0_2/lib/cmake/opencv4/)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( /usr/local/include/eigen3 )

# find_package( Eigen3 REQUIRED )
# include_directories( ${Eigen3_INCLUDE_DIRS} )

# コンパイルに必要なソースコードを取得(それぞれの環境に合わせる)
file(GLOB my_src *.cpp)
# コンパイラへのオプション
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wconversion -pedantic")
# コンパイル対象コードと出力ファイル名
add_executable(main ${my_src})

# OpenCVに必要
target_link_libraries(main ${OpenCV_LIBS})
