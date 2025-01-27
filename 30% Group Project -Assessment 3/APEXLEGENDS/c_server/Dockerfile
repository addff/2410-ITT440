FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    libmysqlclient-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy server source code
COPY server.c /usr/src/server.c

# Compile
WORKDIR /usr/src
RUN gcc server.c -o server -lmysqlclient

# Expose server port
EXPOSE 2001

# Run the server
CMD ["./server"]
