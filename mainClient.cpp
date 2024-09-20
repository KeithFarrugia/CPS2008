#include "Client/Client.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include <string>
#include <regex>

// Function to print the usage of the program
void print_usage(const char* program_name) {
    std::cerr << "Usage: " << program_name 
              << " --server <IP address> --port <port number> --nickname <artist name> [--test]" 
              << std::endl;
}

// Function to validate the IP address format
bool is_valid_ip(const std::string& ip_address) {
    const std::regex ip_pattern(
        R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"
    );
    return std::regex_match(ip_address, ip_pattern);
}

// Function to parse the command-line arguments
bool parse_arguments(int argc, char* argv[], std::string& ip_address, int& port, std::string& nickname, bool& test_mode) {
    if (argc != 7 && argc != 8 && argc != 9) {
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--server" && i + 1 < argc) {
            ip_address = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--nickname" && i + 1 < argc) {
            nickname = argv[++i];
        } else if (arg == "--test") {
            test_mode = true;
        } else {
            return false;
        }
    }

    if (ip_address.empty() || port <= 0 || !is_valid_ip(ip_address) || nickname.empty()) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::string ip_address;
    int port;
    std::string nickname;
    bool test_mode = false;

    if (!parse_arguments(argc, argv, ip_address, port, nickname, test_mode)) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1 || TTF_Init() == -1) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    Client client(ip_address, nickname, test_mode);
    client.connect_to_server(ip_address, port);

    google::protobuf::ShutdownProtobufLibrary();
    SDL_Quit();
    TTF_Quit();
    
    return EXIT_SUCCESS;
}
