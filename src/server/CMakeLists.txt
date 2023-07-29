cmake_minimum_required(VERSION 3.1)

project(server)

#set_target_properties(client PROPERTIES LINKER_LANGUAGE CXX)

add_executable(${PROJECT_NAME}
        "server.cpp"
)

target_link_libraries(${PROJECT_NAME}
                      tcpcommunication          
                      libboost_program_options.a
                      -lboost_system
                      -lboost_thread
                      -lpthread
)
