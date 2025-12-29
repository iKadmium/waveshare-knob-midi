ARG DOCKER_TAG=latest
FROM espressif/idf:${DOCKER_TAG}

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update -y && apt-get install udev -y

# install nvm
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash

# set env
ENV NVM_DIR=/root/.nvm

# install node
RUN echo "Installing Node.js version lol: $NODE_VERSION"
RUN bash -c "source $NVM_DIR/nvm.sh && nvm install --lts"

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

RUN curl -o /etc/udev/rules.d/60-openocd.rules https://github.com/espressif/openocd-esp32/raw/refs/heads/master/contrib/60-openocd.rules

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]