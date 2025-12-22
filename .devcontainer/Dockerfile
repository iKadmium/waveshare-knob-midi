ARG BOARD=esp32s3
ARG DOCKER_TAG=${BOARD}_latest
FROM espressif/idf-rust:${DOCKER_TAG}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

USER root

RUN apt-get update -y && apt-get install sudo udev libsdl2-dev libclang-dev cmake -y
RUN usermod -aG sudo esp

RUN echo '%sudo ALL=(ALL) NOPASSWD: ALL' > /etc/sudoers.d/nopasswd_sudo

RUN chmod 0440 /etc/sudoers.d/nopasswd_sudo

USER esp

# install nvm
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# set env
ENV NVM_DIR="/home/esp/.nvm"

# install node
RUN echo "Installing Node.js LTS"
RUN bash -c "source $NVM_DIR/nvm.sh && nvm install --lts"

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

RUN espup install

CMD ["/bin/bash", "-c"]