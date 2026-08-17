FROM alpine:latest
WORKDIR /app
RUN apk add gcc ncurses-dev musl-dev

COPY main.c /app/
RUN gcc main.c -o 2duniverse -lncurses

CMD ["./2duniverse"]
