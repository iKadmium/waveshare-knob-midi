# Dockerfile
ARG VARIANT=bookworm
FROM mcr.microsoft.com/devcontainers/cpp:${VARIANT}

# Add your LVGL and simulator-specific packages
RUN sudo apt-get update && sudo apt-get install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libwayland-dev \
  python3 \
  python3-pip \
  python3-venv \
  libspdlog-dev \
  udev \
  usbutils \
  picocom \
  minicom \
  screen

## Set up udev rules for PlatformIO
RUN curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
RUN service udev restart 
RUN usermod -a -G dialout vscode
RUN usermod -a -G plugdev vscode

ARG FEDORA_COMPAT=0
### Set up compatibility with Fedora host (if needed)
### Since Fedora uses `18` as the group for dialout, we need to add it to the container
RUN if [ "$FEDORA_COMPAT" = "1" ]; then \
  sudo groupadd -g 18 compat_dialout; \
  sudo usermod -a -G compat_dialout vscode; \
fi
  
# Create a dedicated virtual environment for PlatformIO

USER vscode

# Create venv in home directory with proper permissions
RUN python3 -m venv /home/vscode/platformio-venv && \
    chmod +x /home/vscode/platformio-venv/bin/activate

# Install PlatformIO into the virtual environment
RUN /home/vscode/platformio-venv/bin/pip install platformio

WORKDIR /tmp

COPY ./esp32/platformio.ini ./platformio.ini

# Pre-install dependencies
RUN /home/vscode/platformio-venv/bin/pio pkg install -p espressif32 && \
  mkdir -p src && \
  echo "// dummy file" > src/main.cpp && \
  /home/vscode/platformio-venv/bin/pio pkg install && \
  sudo rm -rf /tmp/platformio.ini /tmp/src

# Add the venv bin directory to PATH
ENV PATH="/home/vscode/platformio-venv/bin:$PATH"
ENV PLATFORMIO_CORE_DIR="/home/vscode/.platformio"