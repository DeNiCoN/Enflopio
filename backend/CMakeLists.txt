if (backend_ENABLED)
  include(CPack)

  install(TARGETS client
    DESTINATION .
    )
endif()
