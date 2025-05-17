FROM debian:bookworm

RUN apt-get update && \
    apt-get install -y gcc make fuse libfuse-dev && \
    mkdir -p /antink_mount /it24_host /antink-logs

COPY antink /antink
RUN gcc -o /antink/antink_fuse /antink/antink.c `pkg-config fuse3 --cflags --libs`

CMD ["fusermount3", "-u", "/antink_mount"]
