FROM ubuntu:17.10

ENV DEBIAN_FRONTEND noninteractive

#install prerequisits
RUN apt-get update && \
    apt-get -y install sudo

RUN sudo apt update && sudo apt -y install ocl-icd-opencl-dev 

RUN sudo apt -y install opencl-headers && sudo apt-get -y install libssl-dev
