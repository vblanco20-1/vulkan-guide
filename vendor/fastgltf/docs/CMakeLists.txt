find_package(Doxygen)
if (NOT DOXYGEN)
    message(STATUS "fastgltf: Doxygen not found; docs will not be built")
    return()
endif()

# Get the sources and create proper absolute paths
get_target_property(FASTGLTF_SOURCES fastgltf SOURCES)
set(FASTGLTF_ABS_SOURCES "")
foreach (SOURCE_FILE ${FASTGLTF_SOURCES})
    cmake_path(ABSOLUTE_PATH SOURCE_FILE BASE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.." OUTPUT_VARIABLE ABSOLUTE_SOURCE_FILE)
    list(APPEND FASTGLTF_ABS_SOURCES ${ABSOLUTE_SOURCE_FILE})
endforeach()

# For the Doxygen file we remove the semicolons for spaces
list(JOIN FASTGLTF_ABS_SOURCES " " DOXYGEN_INPUT)

set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT}/xml/index.xml)
set(DOXYGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYGEN_FILE} @ONLY)

# Generate doxygen XML
add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${FASTGLTF_ABS_SOURCES}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE}
    MAIN_DEPENDENCY ${DOXYGEN_FILE} Doxyfile.in
    COMMENT "Generating docs"
)

add_custom_target(generate_docs DEPENDS ${DOXYGEN_INDEX_FILE})

# Find sphinx
find_program(SPHINX_EXECUTABLE
    NAMES sphinx-build
    DOC "Path to sphinx-build executable")

if (NOT SPHINX_EXECUTABLE)
    message(STATUS "fastgltf: Sphinx not found; docs website will not be built")
    return()
else()
    message(STATUS "fastgltf: Found Sphinx")
endif()

set(SPHINX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_OUTPUT}/index.html)
set(SPHINX_CONF ${CMAKE_CURRENT_SOURCE_DIR}/conf.py)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${SPHINX_CONF} @ONLY)

# Let Sphinx generate our site's HTML
add_custom_command(
    OUTPUT ${SPHINX_INDEX_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${DOXYGEN_INDEX_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/index.rst ${CMAKE_CURRENT_SOURCE_DIR}/overview.rst
        ${CMAKE_CURRENT_SOURCE_DIR}/api.rst ${CMAKE_CURRENT_SOURCE_DIR}/tools.rst ${CMAKE_CURRENT_SOURCE_DIR}/options.rst
        ${CMAKE_CURRENT_SOURCE_DIR}/changelog.rst ${CMAKE_CURRENT_SOURCE_DIR}/guides.rst
    COMMAND ${SPHINX_EXECUTABLE} -b html
        -Dbreathe_projects.fastgltf=\"${DOXYGEN_OUTPUT}/xml\"
        ${CMAKE_CURRENT_SOURCE_DIR} ${SPHINX_OUTPUT}
    MAIN_DEPENDENCY ${SPHINX_CONF}
    COMMENT "Generating docs with Sphinx"
)

add_custom_target(generate_sphinx DEPENDS ${SPHINX_INDEX_FILE})
