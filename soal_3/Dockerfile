FROM debian:bookworm

RUN apt-get update && \apt-get install -y gcc make fuse3 libfuse3-dev pkg-config
RUN mkdir -p /antink
COPY antink.c /antink/
RUN gcc -o /antink/antink_fuse /antink/antink.c $(pkg-config fuse3 --cflags --libs)
WORKDIR /antink

CMD ["/antink/antink_fuse", "/it24_host", "/antink_mount"]
