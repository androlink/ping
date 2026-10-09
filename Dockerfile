FROM debian

WORKDIR /ping

RUN apt-get update && apt-get install -y libasan8 inetutils-ping make valgrind
