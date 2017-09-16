FROM debian:stretch

# Configure APT for minimal installs
ENV DEBIAN_FRONTEND noninteractive
RUN \
  echo 'APT::Install-Recommends "0";' > /etc/apt/apt.conf.d/50no-install-recommends \
  && echo 'APT::Install-Suggests "0";' > /etc/apt/apt.conf.d/50no-install-suggests

WORKDIR /app

RUN apt-get update \
 && apt-get -y install \
   libjpeg-dev \
   make \
   git \
   less \
   vim-nox \
   wget \
   gcc

RUN apt-get -y install \
  dpkg-dev \
  nasm



# WORKDIR /build/libjpeg
# RUN apt-get source libjpeg-turbo
# RUN ./configure
# make -j4

WORKDIR /build
RUN wget https://www.imagemagick.org/download/ImageMagick-7.0.7-2.tar.gz
RUN tar xvf ImageMagick-7.0.7-2.tar.gz

WORKDIR /build/ImageMagick-7.0.7-2
RUN ./configure --disable-docs --disable-hdri
RUN make -j4
RUN make install
# create / patch 

WORKDIR /app
COPY . /app

ENV TERM=linux

