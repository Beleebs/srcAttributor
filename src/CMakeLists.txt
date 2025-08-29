file(GLOB SRCATTRIB_SRC cpp/*.cpp)
file(GLOB SRCATTRIB_HPP hpp/*.hpp)

add_executable(srcattributor ${SRCATTRIB_SRC} ${SRCATTRIB_HPP})
target_link_libraries(srcattributor ${LIBXML2_LIBRARIES} crypto)