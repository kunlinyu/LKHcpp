cmake_minimum_required(VERSION 3.15)
project(LKHcpp
        VERSION 0.1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SRC
        src/Ascent.cpp
        src/Between.cpp
        src/Between_SL.cpp
        src/Between_SSL.cpp
        src/BridgeGain.cpp
        src/ChooseInitialTour.cpp
        src/Connect.cpp
        src/Distance.cpp
        src/Excludable.cpp
        src/Exclude.cpp
        src/FindTour.cpp
        src/Flip.cpp
        src/Gain23.cpp
        src/Improvement.cpp
        src/LKHcpp.cpp
        src/LinKernighan.cpp
        src/MergeWithTourIPT.cpp
        src/Minimum1TreeCost.cpp
        src/MinimumSpanningTree.cpp
        src/NormalizeNodeList.cpp
        src/NormalizeSegmentList.cpp
        src/RecordBetterTour.cpp
        src/RestoreTour.cpp
        src/SegmentSize.cpp
        src/StatusReport.cpp
        src/StoreTour.cpp
        src/WriteTour.cpp
        src/candidate/AddCandidate.cpp
        src/candidate/AdjustCandidateSet.cpp
        src/candidate/CandidateReport.cpp
        src/candidate/CreateCandidateSet.cpp
        src/candidate/GenerateCandidates.cpp
        src/candidate/OrderCandidateSet.cpp
        src/candidate/POpMUSICCandidateSetCreator.cpp
        src/candidate/ResetCandidateSet.cpp
        src/candidate/SymmetrizeCandidateSet.cpp
        src/candidate/TrimCandidateSet.cpp
        src/data/Context.cpp
        src/data/Param.cpp
        src/data/ParamReader.cpp
        src/data/TSPLIBReader.cpp
        src/move/Best2OptMove.cpp
        src/move/Best3OptMove.cpp
        src/move/Best4OptMove.cpp
        src/move/Best5OptMove.cpp
        src/move/Make2OptMove.cpp
        src/move/Make3OptMove.cpp
        src/move/Make4OptMove.cpp
        src/move/Make5OptMove.cpp
        src/utils/Random.cpp
        src/variant/EdgeDataEncoder.cpp
        src/variant/NodeCoordEncoder.cpp
        src/variant/STTSPVariant.cpp
)

add_library(LKHcpp ${SRC})
target_compile_definitions(LKHcpp PRIVATE PLOG_CAPTURE_FILE)
if(MSVC)
    target_compile_options(LKHcpp PRIVATE /permissive-)
endif()

add_executable(${PROJECT_NAME}_exe src/main.cpp)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE LKHcpp)
target_compile_definitions(${PROJECT_NAME}_exe PRIVATE PLOG_CAPTURE_FILE)
set_target_properties(${PROJECT_NAME}_exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
if(UNIX)
    target_link_libraries(${PROJECT_NAME}_exe PRIVATE m)
elseif(MSVC)
    target_compile_options(${PROJECT_NAME}_exe PRIVATE /permissive-)
endif()

target_compile_options(LKHcpp PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_compile_options(${PROJECT_NAME}_exe PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

#set_source_files_properties(${SRC}
#        PROPERTIES
#        COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Werror;-Wconversion;"
##        COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Werror;-Wconversion;-Wsign-conversion;-Wshadow;-Warith-conversion"
#)

add_custom_command(
    TARGET ${PROJECT_NAME}_exe
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}_exe> $<TARGET_FILE_DIR:${PROJECT_NAME}_exe>/../
)

if (MINGW)
    target_link_options(${PROJECT_NAME}_exe PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
    )
endif ()
if(MSVC)
    add_compile_options(/permissive-)
endif()
