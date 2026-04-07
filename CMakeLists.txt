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
        src/Connect.cpp
        src/Ascent.cpp
        src/C.cpp
        src/ChooseInitialTour.cpp
        src/main.cpp
        src/Minimum1TreeCost.cpp
        src/MinimumSpanningTree.cpp
        src/OrderCandidateSet.cpp
        src/RecordBetterTour.cpp
        src/SymmetrizeCandidateSet.cpp
        src/data/Context.cpp
        src/data/Param.cpp
        src/data/Problem.cpp
        src/data/ProblemReader.cpp
        src/utils/Random.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} m)
target_compile_definitions(${PROJECT_NAME} PRIVATE PLOG_CAPTURE_FILE)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/../LKH
)
set_property(DIRECTORY APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../LKH
)

if (MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
    )
endif ()
