cmake_minimum_required(VERSION 3.0.0)
project(NetScetch VERSION 0.1.0 LANGUAGES C CXX)
# ============================== LIBRARIES ==============================

find_package(Protobuf REQUIRED)

# ========================================================================================
#                                      SDL LIBRARIES                                      
# ========================================================================================

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/SDL/cmake/sdl")

find_package(SDL2       REQUIRED)
find_package(SDL2_ttf   REQUIRED)
find_package(SDL2_gfx   REQUIRED)

include_directories ( ${SDL2_INCLUDE_DIRS}      )
include_directories ( ${SDL2_TTF_INCLUDE_DIRS}  )
include_directories ( ${SDL2_GFX_INCLUDE_DIRS}  )


# ========================================================================================
#                                          OpenSSL                                         
# ========================================================================================

find_package(OpenSSL REQUIRED)
include_directories ( ${OPENSSL_INCLUDE_DIR}    )

# ========================================================================================
#                                          COMMON                                         
# ========================================================================================
set (COMMON_HEADER
    Common/DrawList.h
    Common/Serialize/Serialize.pb.h
)
set (COMMON_SOURCE
    Common/DrawList.cpp
    Common/Serialize/Serialize.pb.cc
)

# ========================================================================================
#                                          CLIENT                                         
# ========================================================================================
set (CLIENT_HEADER
    Client/Client.h
    Client/Lexer/DFSA/DFSA.h
    Client/Lexer/Lexer.h
    Client/Lexer/Utility.h
)
set (CLIENT_SOURCE
    Client/Client.cpp
    Client/Server_Connect.cpp

    Client/Render_Thread.cpp
    Client/Main_Client_Thread.cpp
    Client/Message_Reader_Thread.cpp
    
    Client/User_Functionality/Create.cpp
    Client/User_Functionality/Handle_SDL.cpp
    Client/User_Functionality/Parse_Input.cpp
    Client/User_Functionality/Command_List.cpp
    Client/User_Functionality/Execute_Command.cpp
    
    # ------------------------ Lexer
    Client/Lexer/Lexer.cpp
    Client/Lexer/DFSA/DFSA.cpp
    Client/Lexer/Lexer_Utility/Transitions.cpp
    Client/Lexer/Lexer_Utility/Generate.cpp
    Client/Lexer/Lexer_Utility/TokenMap.cpp
    Client/Lexer/Lexer_Utility/Identify.cpp
)

# ========================================================================================
#                                         SERVER                                          
# ========================================================================================
set (SERVER_HEADER
    Server/Server.h
)
set (SERVER_SOURCE
    Server/Server.cpp 
    Server/Server_Main.cpp
    Server/Client_Handler.cpp 
    Server/Thread_Functions.cpp
    Server/Execute_And_Broadcast.cpp
)

# ========================================================================================
#                                      LIB CREATION                                       
# ========================================================================================
add_library(COMMON_LIB ${COMMON_HEADER} ${COMMON_SOURCE}    )
add_library(CLIENT_LIB ${CLIENT_HEADER} ${CLIENT_SOURCE}    )
add_library(SERVER_LIB ${SERVER_HEADER} ${SERVER_SOURCE}    )


# ========================================================================================
#                                       EXECUTABLES                                       
# ========================================================================================

# ----------------------------------- Client
set (SC_CLIENT  mainClient.cpp)

add_executable          (client ${SC_CLIENT}            )
target_link_libraries   (client CLIENT_LIB              )
target_link_libraries   (client COMMON_LIB              )

target_link_libraries   (client ${SDL2_LIBRARIES}       )
target_link_libraries   (client ${SDL2_TTF_LIBRARIES}   )
target_link_libraries   (client ${SDL2_GFX_LIBRARIES}   )
target_link_libraries   (client ${PROTOBUF_LIBRARIES}   )
target_link_libraries   (client OpenSSL::SSL            )
target_link_libraries   (client OpenSSL::Crypto         )




# ----------------------------------- Server
set (SC_SERVER  mainServer.cpp)

add_executable          (server ${SC_SERVER}            )
target_link_libraries   (server SERVER_LIB              )
target_link_libraries   (server COMMON_LIB              )

target_link_libraries   (server ${SDL2_LIBRARIES}       )
target_link_libraries   (server ${SDL2_TTF_LIBRARIES}   )
target_link_libraries   (server ${SDL2_GFX_LIBRARIES}   )
target_link_libraries   (server ${PROTOBUF_LIBRARIES}   )
target_link_libraries   (server OpenSSL::SSL            )
target_link_libraries   (server OpenSSL::Crypto         )


# =============================== Linking ===============================
file(COPY ${CMAKE_SOURCE_DIR}/SDL/fonts DESTINATION ${CMAKE_BINARY_DIR})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
