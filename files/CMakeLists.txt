set(application_name "planetary_imager" CACHE INTERNAL "Force executable name, useful for wrapper launch")
configure_file(planetary_imager.desktop.in planetary_imager.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/planetary_imager.desktop DESTINATION share/applications)
add_subdirectory(icons)
