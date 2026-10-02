add_custom_target(CopyTextures SOURCES ${CMAKE_CURRENT_LIST_DIR})

message("Copying texture folder")
file(COPY ${CMAKE_CURRENT_LIST_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
