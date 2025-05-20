FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    fuse3 \
    libfuse3-dev \
    pkg-config \
    build-essential \
    inotify-tools

WORKDIR /app
COPY antink.c /app/

ENV TZ=Asia/Jakarta
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN gcc -Wall antink.c `pkg-config fuse3 --cflags --libs` -o antink

RUN mkdir -p /antink_mount /it24_host /antink-system/antink-logs

CMD ["./antink", "-f", "/antink_mount"]
