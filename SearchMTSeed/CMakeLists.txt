# CMakeの最低限必要なバージョンを指定します。
cmake_minimum_required(VERSION 3.10)

# プロジェクトの名前を定義します。ここでは "SearchMTSeedProject" とします。
project(SearchMTSeedProject CXX)

# C++の標準をC++23に設定します。
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenMPライブラリを見つけて、プロジェクトに追加します。
# これにより、#pragma omp のようなディレクティブが使えるようになります。
find_package(OpenMP REQUIRED)

# "SearchMTSeed" という名前の実行可能ファイルを作成します。
# この実行ファイルは、SearchMTSeed.cpp と ../lib/MT_1.cpp の2つのソースファイルから作られます。
add_executable(SearchMTSeed
    SearchMTSeed.cpp
    ../lib/MT_1.cpp
)

# libディレクトリをインクルードパスに追加します。
target_include_directories(SearchMTSeed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# 実行可能ファイルにOpenMPライブラリをリンクします。
# これでコンパイル時にOpenMP関連のエラーが出なくなります。
target_link_libraries(SearchMTSeed PRIVATE OpenMP::OpenMP_CXX)
