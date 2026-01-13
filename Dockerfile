# Dockerfile for LimeOS ISO Builder
# This image provides a complete build environment for creating LimeOS installation media

FROM debian:bookworm-slim

# Set working directory
WORKDIR /app

# Install build dependencies from shell.nix equivalents
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    git \
    curl \
    wget \
    jq \
    xorriso \
    grub-pc-bin \
    debootstrap \
    isolinux \
    squashfs-tools \
    libcurl4-openssl-dev \
    libjson-c-dev \
    libssl-dev \
    libcmocka-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . .

# Build the ISO builder application
RUN make clean && make

# Default command
CMD ["/app/bin/limeos-iso-builder"]