# Compiler warning flags

function(set_project_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4
            /WX-  # Don't treat warnings as errors for now
        )
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wformat=2
            -Wno-error=return-type  # Don't treat return-type warnings as errors
            -Wno-error=unused-parameter  # Don't treat unused-parameter warnings as errors
        )
    endif()
endfunction()
