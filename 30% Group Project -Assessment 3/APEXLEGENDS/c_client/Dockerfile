FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    && rm -rf /var/lib/apt/lists/*

COPY client.c /usr/src/client.c

WORKDIR /usr/src

RUN gcc client.c -o client

CMD ["./client"]
