FROM ubuntu:22.04
LABEL maintainer="Morgan Askins <maskins@berkeley.edu>"

SHELL ["/bin/bash", "-c"]

RUN apt-get -q update && DEBIAN_FRONTEND=noninteractive apt-get -qy install --no-install-recommends \
    curl build-essential vim libx11-dev libxpm-dev git \
    libqt5opengl5-dev cmake xserver-xorg-video-intel libxft-dev libxext-dev libxkbcommon-x11-dev libopengl-dev \
    python3-dev python3-numpy libcurl4-gnutls-dev ca-certificates libssl-dev libffi-dev libxerces-c-dev gdb
RUN strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt5Core.so
RUN git clone https://github.com/MorganAskins/ratpacSetup.git
RUN cd /ratpacSetup && ./ratpacSetup.sh -j24 --only root geant4 cry
#ENTRYPOINT ["/ratpacSetup/env.sh"]
CMD [ "/bin/bash" ]
