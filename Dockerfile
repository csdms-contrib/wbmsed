# Containerize WBMsed using a GHAAS base image.
FROM csdms/ghaas:0.1

LABEL org.opencontainers.image.authors="CSDMS <csdms@colorado.edu>"
LABEL org.opencontainers.image.url="https://hub.docker.com/r/csdms/wbmsed"
LABEL org.opencontainers.image.source="https://github.com/csdms-contrib/wbmsed"
LABEL org.opencontainers.image.vendor="Community Surface Dynamics Modeling System (CSDMS)"

RUN apt-get update && \
    apt-get -y install make cmake git clang && \
    apt-get autoclean && apt-get purge

RUN git clone --depth 1 https://github.com/csdms-contrib/wbmsed /opt/wbmsed
RUN GHAASDIR=/usr/local/share/ghaas /opt/wbmsed/install.sh

WORKDIR /opt/wbmsed
