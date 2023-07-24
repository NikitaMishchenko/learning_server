cmake_minimum_required(VERSION 3.1)

project(basic)

#set_target_properties(client PROPERTIES LINKER_LANGUAGE CXX)

add_executable(${PROJECT_NAME}
        "main.cpp"
)

target_link_libraries(${PROJECT_NAME}
                        libboost_program_options.a
                        -lboost_system
                        -lboost_thread
                        -lpthread
)
