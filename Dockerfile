FROM alpine:latest

RUN apk update && apk add --no-cache \
    autoconf build-base binutils cmake curl file gcc g++ git libgcc libtool linux-headers make musl-dev ninja tar unzip wget

# Boost
RUN apk add --no-cache boost-dev

# OpenFst

ENV FST_VERSION "1.7.7"
ENV FST_DOWNLOAD_PREFIX "http://www.openfst.org/twiki/pub/FST/FstDownload"

RUN cd /tmp \
    && wget -q ${FST_DOWNLOAD_PREFIX}/openfst-${FST_VERSION}.tar.gz \
    && tar -zxf openfst-${FST_VERSION}.tar.gz \
    && rm openfst-${FST_VERSION}.tar.gz

RUN cd /tmp/openfst-${FST_VERSION} \
    && ./configure  --enable-grm \
    && make --jobs=3 \
    && make install \
    && rm -rf /tmp/openfst-${FST_VERSION}

# Thrax

ENV THRAX_VERSION "1.3.3"
ENV THRAX_DOWNLOAD_PREFIX "http://www.opengrm.org/twiki/pub/GRM/ThraxDownload"

RUN cd /tmp \
    && wget -q ${THRAX_DOWNLOAD_PREFIX}/thrax-${THRAX_VERSION}.tar.gz \
    && tar -zxf thrax-${THRAX_VERSION}.tar.gz \
    && rm thrax-${THRAX_VERSION}.tar.gz

RUN cd /tmp/thrax-${THRAX_VERSION} \
    && ./configure \
    && make --jobs=2 \
    && make install \
    && rm -rf /tmp/thrax-${THRAX_VERSION}
