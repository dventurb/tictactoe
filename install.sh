#!/bin/bash

# Debian / Ubuntu
install_with_apt() {
  sudo apt update
  sudo apt install -y \
    build-essential \
    pkg-config \
    libgtk-4-dev \
    libsdl2-mixer-dev \
    libsdl2-dev
}

# Red Hat / CentOS
install_with_dnf() {
  sudo dnf install -y \
    gcc \
    make \
    pkgconf-pkg-config \
    gtk4-devel \
    SDL2-devel \
    SDL2-mixer-devel
}

# Arch Linux
install_with_pacman() {
  sudo pacman -Sy --noconfirm \
    base-devel \
    pkgconf \
    gtk4 \
    sdl2 \
    sdl2_mixer
}

if [ -f /etc/arch-release ]; then
  install_with_pacman
elif [ -f /etc/debian_version ]; then
  install_with_apt
elif [ -f /etc/redhat-release ] || [ -f /etc/centos-release ]; then
  install_with_dnf
else
  echo "Your linux distro is not supported currently."
  echo "You need to manualy install those packages: gcc, make, pkg-config, gtk4, sdl2 and sdl2_mixer."
  exit 1
fi

make
echo "INSTALLATION FINISHED"

