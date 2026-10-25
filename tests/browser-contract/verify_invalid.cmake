execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE}" -B "${BUILD}" -DSDK_ROOT=${SDK_ROOT} -DCASE=${CASE}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(result EQUAL 0 OR NOT "${output}${error}" MATCHES "${EXPECT}")
    message(FATAL_ERROR "Expected browser asset rejection: ${output}${error}")
endif()
