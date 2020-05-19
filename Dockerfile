FROM python:3.7-buster

# OpenFst

ENV FST_VERSION "1.7.7"
ENV FST_DOWNLOAD_PREFIX "http://www.openfst.org/twiki/pub/FST/FstDownload"

RUN cd /tmp \
    && wget -q ${FST_DOWNLOAD_PREFIX}/openfst-${FST_VERSION}.tar.gz \
    && tar -zxf openfst-${FST_VERSION}.tar.gz \
    && rm openfst-${FST_VERSION}.tar.gz

RUN cd /tmp/openfst-${FST_VERSION} \
    && ./configure  --enable-grm \
    && make --jobs=4 \
    && make install \
    && rm -rd /tmp/openfst-${FST_VERSION}

RUN ldconfig

# Thrax

ENV THRAX_VERSION "1.3.3"
ENV THRAX_DOWNLOAD_PREFIX "http://www.opengrm.org/twiki/pub/GRM/ThraxDownload"

RUN cd /tmp \
    && wget -q ${THRAX_DOWNLOAD_PREFIX}/thrax-${THRAX_VERSION}.tar.gz \
    && tar -zxf thrax-${THRAX_VERSION}.tar.gz \
    && rm thrax-${THRAX_VERSION}.tar.gz

RUN cd /tmp/thrax-${THRAX_VERSION} \
    && ./configure \
    && make --jobs=4 \
    && make install \
    && rm -rd /tmp/thrax-${THRAX_VERSION}
