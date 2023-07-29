cmake_minimum_required(VERSION 3.1)

project(tcpcommunication)

#set_target_properties(client PROPERTIES LINKER_LANGUAGE CXX)

add_library(tcpcommunication
            client_interface.h
            connection.h
            tcp_message.h
            owned_message.h
            thread_safe_queue.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${PROJECT_NAME}
                      libboost_program_options.a
                      -lboost_system
                      -lboost_thread
                      -lpthread
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)