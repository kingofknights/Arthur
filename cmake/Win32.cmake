if(WIN32)
  add_compile_definitions(WIN32_LEAN_AND_MEAN)
  add_compile_definitions(WINVER=0x0A00)
  add_compile_definitions(_WIN32_WINNT=0x0A00)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/bigobj)

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

endif()
