FROM debian:bookworm-slim as build-env

# Install OS packages
RUN apt-get update
RUN apt-get install -y git build-essential wget

# https://github.com/micropython/micropython/issues/8685
RUN wget --quiet "https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz?rev=95edb5e17b9d43f28c74ce824f9c6f10&hash=D5ACE3A6F75F603551D7702E00ED7B29" -O /tmp/arm-gnu-toolchain.tar.xz
RUN tar -xf /tmp/arm-gnu-toolchain.tar.xz -C /usr/local/
ENV PATH="/usr/local/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi/bin:${PATH}"

# Install nrfutil
RUN echo "deb http://ftp.dk.debian.org/debian bullseye main" >/etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y python2 python2-dev
RUN wget --quiet "https://bootstrap.pypa.io/pip/2.7/get-pip.py" -O /tmp/get-pip.py
RUN python2 /tmp/get-pip.py
RUN pip install nrfutil
# mergehex is not included in nrfutil
RUN apt-get install -y libusb-1.0-0
RUN wget --quiet "https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/desktop-software/nrf-command-line-tools/sw/versions-10-x-x/10-23-2/nrf-command-line-tools_10.23.2_amd64.deb" -O /tmp/nrf-command-line-tools.deb
RUN dpkg -i /tmp/nrf-command-line-tools.deb
ENV PATH="/opt/nrf-command-line-tools/bin:${PATH}"

# Copy all files from the current directory into the build-env container
RUN mkdir -p /spaceinvader
COPY . /spaceinvader

ENTRYPOINT ["/spaceinvader/build.sh"]
