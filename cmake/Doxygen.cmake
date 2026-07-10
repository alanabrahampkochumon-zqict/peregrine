include_guard()

find_package(Doxygen)
if (DOXYGEN_FOUND)
    message(STATUS "Doxygen found!")
    doxygen_add_docs(
            docs
            ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating documentation with doxygen..."
            CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            ALL
    )
    set_target_properties(docs PROPERTIES FOLDER ${DevToolsDir})
else ()
    message(WARNING "Doxygen not found! Skipping documentation generation.")
endif ()