if (NOT EXISTS "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/out/build/x64-Debug (default)/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/out/build/x64-Debug (default)/install_manifest.txt\"")
endif(NOT EXISTS "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/out/build/x64-Debug (default)/install_manifest.txt")

file(READ "D:/RPI/GameArch/Final/final-projecct-LionLin27/src/3rdparty/SDL2-2.0.5/out/build/x64-Debug (default)/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach (file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND D:/Microsoft Visual Studio/2019/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach(file)

