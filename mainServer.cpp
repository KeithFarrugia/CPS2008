#include "Server/Server.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h> // for getpid()

void print_usage(const char* program_name) {
    std::cerr << "Usage: " << program_name << " --port <port_number>" << std::endl;
}

bool parse_arguments(int argc, char* argv[], int& port) {
    if (argc != 3) {
        return false;
    }

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--port") {
            port = std::atoi(argv[i + 1]);
        } else {
            return false;
        }
    }

    if (port <= 0) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int port;

    if (!parse_arguments(argc, argv, port)) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1 || TTF_Init() == -1) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "> PID of the current process: " << getpid() << std::endl;

    Server server;
    server.start(port);

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
