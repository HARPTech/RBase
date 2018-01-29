FROM dockcross/linux-arm64

ENV DEFAULT_DOCKCROSS_IMAGE rbase-arm64

RUN apt-get install -yq libboost-system-dev:arm64 libboost-log-dev:arm64 libboost-dev:arm64 libboost-program-options-dev:arm64 libsqlite3-dev:arm64 swig2.0:arm64

# Install Python Libraries
RUN apt-get install -yq libpython3-dev:arm64 python3-numpy

ENV LD_LIBRARY_PATH /opt/swig/usr/lib

# Add swig from debian unstable, so the newest parser features are available.
RUN echo "deb http://ftp.at.debian.org/debian unstable main" >> /etc/apt/sources.list

RUN apt-get update && apt-get -t sid download -y \
  swig3.0 && \
  mkdir -p /opt/swig && \
  mv ./*swig*.deb /opt/swig && \
  cd /opt/swig && \
  dpkg -x swig*.deb ./

# Link swig directory to expected path
RUN ln -s /opt/swig/usr/share/swig3.0/ /usr/share/swig3.0
RUN ln -s /opt/swig/usr/bin/swig3.0 /usr/bin/swig3.0
