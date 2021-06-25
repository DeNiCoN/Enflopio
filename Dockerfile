FROM emscripten/emsdk as emscripten-build

RUN apt update && apt install -y cmake python3
RUN pip install conan

COPY . /Enflopio
WORKDIR /Enflopio/build

RUN ./cmake_emscripten.sh
WORKDIR /Enflopio/build/Emscripten
RUN make -j
RUN cpack


FROM ubuntu:impish as server-build
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
RUN apt update && apt install -y cmake python3-pip
RUN pip install conan

COPY . /Enflopio
WORKDIR /Enflopio/build

RUN ./cmake_release.sh
WORKDIR /Enflopio/build/Release
RUN make -j
RUN cpack


FROM python:3.7-alpine as backend
COPY --from=emscripten-build /Enflopio/package /app
WORKDIR /app
ENTRYPOINT ["/venv/bin/python3", "-m", "http.server", "80"]
EXPOSE 80


FROM scratch as server
COPY --from=server-build /Enflopio/package /
ENTRYPOINT ["/server"]
