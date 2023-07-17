find_program(GLSL_VALIDATOR glslangValidator REQUIRED)

set(SHADERS
    shader.frag
    shader.vert)

foreach (SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}.spv)
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER})
    list(APPEND SPIRV_FILES ${SPIRV})
endforeach ()

add_custom_target(nae-shaders DEPENDS ${SPIRV_FILES} SOURCES ${SHADERS})
