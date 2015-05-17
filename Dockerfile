FROM debian:jessie
MAINTAINER trusch

# keep apt quiet
ENV DEBIAN_FRONTEND=noninteractive

# bring base image up to speed
RUN apt-get update && apt-get upgrade -y

# install build tools and dependencies for SUSI
RUN apt-get install -y git wget cmake build-essential libsqlite3-dev liblzma-dev zlib1g-dev

# get SUSI code
RUN git clone --branch experimental --recursive http://github.com/webvariants/susi /susi-src

# build
RUN mkdir /susi-build && cd /susi-build && cmake -DCMAKE_BUILD_TYPE=Release /susi/Core && make -j4 install



