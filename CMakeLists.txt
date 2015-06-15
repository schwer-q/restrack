if( NOT MSVC )
  if( NOT LLVM_REQUIRES_RTTI )
    if( NOT LLVM_REQUIRES_EH )
      set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/restrack.exports)
    endif()
  endif()
endif()

add_llvm_loadable_module(restrack restrack.cxx)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  target_link_libraries(restrack ${cmake_2_8_12_PRIVATE}
    clangAST
    clangBasic
    clangFrontend
    LLVMSupport
    )
endif()
