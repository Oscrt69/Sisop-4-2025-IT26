FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    fuse3 \
    libfuse3-dev \
    pkg-config \
    build-essential \
    inotify-tools

WORKDIR /app
COPY antink.c /app/

RUN gcc -Wall antink.c `pkg-config fuse3 --cflags --libs` -o antink

RUN mkdir -p /antink_mount /it24_host /antink-system/antink-logs

CMD ["./antink", "-f", "/antink_mount"]
