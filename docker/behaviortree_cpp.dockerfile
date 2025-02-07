FROM ubuntu:24.04 AS builder

ARG BT_VERSION=4.6.2

LABEL maintainer="Alberto J. Tudela Roldán <ajtudela@uma.es>"

RUN apt update && apt upgrade -y && apt install -y \
  build-essential \
  cmake \
  git \
  libzmq3-dev \
  libncurses-dev \
  libsqlite3-dev

RUN git clone --branch ${BT_VERSION} https://github.com/BehaviorTree/BehaviorTree.CPP.git
RUN cd BehaviorTree.CPP && mkdir build

WORKDIR /BehaviorTree.CPP/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/behaviortree_cpp -DBTCPP_UNIT_TESTS=OFF
RUN make install

FROM ubuntu:24.04

RUN apt update && apt upgrade -y && rm -rf /var/lib/apt/lists/*
COPY --from=builder /opt/behaviortree_cpp /usr/local
RUN ldconfig
