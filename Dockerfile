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
   gcc \
   dpkg-dev \
   nasm \
   autoconf \
   automake \
   libtool \
   pkg-config

RUN apt-get install -y nodejs \
   python2.7 \
   cmake

WORKDIR /build
RUN wget https://www.imagemagick.org/download/ImageMagick-6.9.9-14.tar.gz && \
  tar xvf ImageMagick-6.9.9-14.tar.gz

RUN wget https://github.com/libjpeg-turbo/libjpeg-turbo/archive/1.5.2.tar.gz && \
  tar xvf 1.5.2.tar.gz

RUN wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz && \
  tar xvf emsdk-portable.tar.gz

RUN apt-get install -y python

WORKDIR /build/emsdk-portable
RUN ./emsdk update
RUN ./emsdk install latest
RUN ./emsdk activate latest
ENV PATH "/build/emsdk-portable:/build/emsdk-portable/clang/e1.37.21_64bit:/build/emsdk-portable/node/4.1.1_64bit/bin:/build/emsdk-portable/emscripten/1.37.21:$PATH"
ENV EMSDK "/build/emsdk-portable"
ENV EM_CONFIG "/root/.emscripten"
ENV BINARYEN_ROOT "/build/emsdk-portable/clang/e1.37.21_64bit/binaryen"
ENV EMSCRIPTEN "/build/emsdk-portable/emscripten/1.37.21"

WORKDIR /build/libjpeg-turbo-1.5.2
RUN  autoreconf -fiv && \
  mkdir build && cd build && \
  emconfigure ../configure CFLAGS='-s ASSERTIONS=2 -g -O0' --without-simd
RUN cd build && \
  emmake make -j4 V=1 && \
  emmake make install

WORKDIR /build/ImageMagick-6.9.9-14
COPY configure.ac .
RUN autoconf
RUN emconfigure ./configure CFLAGS='-s ASSERTIONS=2 -g -O0' --disable-docs --disable-hdri --with-jpeg=yes --with-magick-plus-plus=no
RUN emmake make -j4 V=1
RUN emmake make install
# create / patch 

WORKDIR /app
COPY . /app

RUN emcc `pkg-config --cflags MagickCore` convertjs.c `pkg-config --libs MagickCore` -O0 -g4 -o convertjs
RUN ln -s /build/ImageMagick-6.9.9-14/coders && ln -s /build/ImageMagick-6.9.9-14/magick && cp /build/libjpeg-turbo-1.5.2/*.h .. && cp /build/libjpeg-turbo-1.5.2/*.c ..
RUN emcc `pkg-config --cflags MagickCore` convertjs.c `pkg-config --libs MagickCore` -L/opt/libjpeg-turbo/lib32 -ljpeg /usr/local/lib/libMagickCore-6.Q16.a /opt/libjpeg-turbo/lib32/libjpeg.a -O0 -g -o convertjs.html
#RUN ldconfig

ENV TERM=linux

