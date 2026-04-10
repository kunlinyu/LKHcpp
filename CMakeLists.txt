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
        src/candidate/AddCandidate.cpp
        src/candidate/AdjustCandidateSet.cpp
        src/candidate/CandidateReport.cpp
        src/candidate/CreateCandidateSet.cpp
        src/candidate/GenerateCandidates.cpp
        src/candidate/POpMUSICCandidateSetCreator.cpp
        src/candidate/ResetCandidateSet.cpp
        src/candidate/TrimCandidateSet.cpp
        src/move/Best2OptMove.cpp
        src/move/Best3OptMove.cpp
        src/move/Best4OptMove.cpp
        src/move/Best5OptMove.cpp
        src/move/Make2OptMove.cpp
        src/move/Make3OptMove.cpp
        src/move/Make4OptMove.cpp
        src/move/Make5OptMove.cpp
        src/data/Context.cpp
        src/data/Param.cpp
        src/data/TSPLIBReader.cpp
        src/utils/Random.cpp
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
        src/LinKernighan.cpp
        src/LKHmain.cpp
        src/main.cpp
        src/MergeWithTourIPT.cpp
        src/Minimum1TreeCost.cpp
        src/MinimumSpanningTree.cpp
        src/NormalizeNodeList.cpp
        src/NormalizeSegmentList.cpp
        src/OrderCandidateSet.cpp
        src/ReadParameters.cpp
        src/RecordBetterTour.cpp
        src/RestoreTour.cpp
        src/SegmentSize.cpp
        src/StatusReport.cpp
        src/StoreTour.cpp
        src/SymmetrizeCandidateSet.cpp
        src/WriteTour.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} m)
target_compile_definitions(${PROJECT_NAME} PRIVATE PLOG_CAPTURE_FILE)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/../${PROJECT_NAME}
)
set_property(DIRECTORY APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../${PROJECT_NAME}
)

if (MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
    )
endif ()
