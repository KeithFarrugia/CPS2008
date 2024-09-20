#!/bin/bash

install_libs() {
    echo "Updating package lists and upgrading installed packages..."
    sudo apt update
    sudo apt upgrade -y

    echo "Installing SDL libraries..."

    # Install SDL2 development package
    sudo apt install -y libsdl2-dev
    
    # Install SDL2_ttf development package
    sudo apt install -y libsdl2-ttf-dev
    
    # Install SDL2_gfx development package
    sudo apt install -y libsdl2-gfx-dev

    echo "Installing Protobuf..."
    sudo apt install -y protobuf-compiler libprotobuf-dev

    echo "Installing OpenSSL..."
    sudo apt install -y libssl-dev

    sudo apt update
    sudo apt upgrade -y
}

uninstall_libs() {
    echo "Uninstalling libraries..."

    echo "Removing SDL libraries..."
    # Uninstall SDL2 development package
    sudo apt-get remove --purge -y libsdl2-dev
    
    # Uninstall SDL2_ttf development package
    sudo apt-get remove --purge -y libsdl2-ttf-dev

    # Uninstall SDL2_gfx development package
    sudo apt-get remove --purge -y libsdl2-gfx-dev

    echo "Removing Protobuf..."
    sudo apt-get remove --purge -y protobuf-compiler libprotobuf-dev

    echo "Removing OpenSSL..."
    sudo apt-get remove --purge -y libssl-dev

    echo "Cleaning up..."
    sudo apt autoremove -y
    sudo apt clean

    sudo apt update
    sudo apt upgrade -y
}

if [ "$1" = "install" ]; then
    install_libs
elif [ "$1" = "uninstall" ]; then
    uninstall_libs
else
    echo "Usage: $0 [install|uninstall]"
    exit 1
fi