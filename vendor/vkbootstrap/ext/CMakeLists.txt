option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)

option(CATCH_BUILD_TESTING "" OFF)
option(CATCH_ENABLE_WERROR "" OFF)
option(CATCH_INSTALL_DOCS "" OFF)
option(CATCH_INSTALL_HELPERS "" OFF)
option(CATCH_INSTALL_EXTRAS "" OFF)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

