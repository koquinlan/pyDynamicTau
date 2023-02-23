project(dynTau)

set(SOURCES 
    bayes.cpp 
    proc.cpp 
    spectrum.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} training.cpp ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}" Eigen3::Eigen)

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if(MSVC)
    message(STATUS "Copying DLLs to avoid memory errors")
    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${TORCH_DLLS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif(MSVC)