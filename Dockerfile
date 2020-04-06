# production environment
FROM ubuntu:16.04
MAINTAINER Semen Koretskiy <semenmsu@gmail.com>

#7z
RUN apt-get update && apt-get install -y software-properties-common build-essential
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && apt-get update
RUN apt-get install -y gcc-9 g++-9

#set c++
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 10
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 10
RUN update-alternatives --config gcc
RUN update-alternatives --config g++

RUN apt-get install -y git

RUN git clone https://github.com/semenmsu/MultiQueueProcessor.git
WORKDIR MultiQueueProcessor
RUN make
RUN rm -rf result
ADD ./entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
RUN chmod +x /entrypoint.sh
#WORKDIR /code/fast-generator/fast2
#RUN make
#WORKDIR /code/fast-generator/fast2/bin
#ADD ./entrypoint.sh /entrypoint.sh
#ENTRYPOINT ["/entrypoint.sh"]
#ENV YARD_READABLE_PATH='/data/readable'
#ENV YARD_EXTRACTED_PATH='/data/extracted'
#RUN chmod +x /entrypoint.sh