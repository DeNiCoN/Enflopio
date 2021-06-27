FROM emscripten/emsdk as emscripten-build

RUN apt update && apt install -y cmake python3
RUN pip install conan

COPY CMakeLists.txt /Enflopio/
COPY libs/CMakeLists.txt /Enflopio/libs/
COPY build/cmake_emscripten.sh /Enflopio/build/
WORKDIR /Enflopio/build

RUN ./cmake_emscripten.sh -DDEPENDENCIES_ONLY=ON

COPY . /Enflopio/
WORKDIR /Enflopio/build

RUN ./cmake_emscripten.sh -DDEPENDENCIES_ONLY=OFF
WORKDIR /Enflopio/build/Emscripten
RUN make -j
RUN cpack -D CPACK_PACKAGE_FILE_NAME="package" -G TGZ
RUN mkdir package
RUN tar -C package -xzf package.tar.gz


FROM ubuntu:impish as server-build
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
RUN apt update && apt install -y cmake python3-pip
RUN pip install conan

COPY CMakeLists.txt /Enflopio/
COPY libs/CMakeLists.txt /Enflopio/libs/
COPY build/cmake_release.sh /Enflopio/build/
WORKDIR /Enflopio/build

RUN ./cmake_release.sh -DDEPENDENCIES_ONLY=ON -Dclient_ENABLED=OFF

COPY . /Enflopio
WORKDIR /Enflopio/build

RUN ./cmake_release.sh -DDEPENDENCIES_ONLY=OFF
WORKDIR /Enflopio/build/Release
RUN make -j
RUN cpack -D CPACK_PACKAGE_FILE_NAME="package" -G TGZ
RUN mkdir package
RUN tar -C package -xzf package.tar.gz


FROM python:3.7-alpine as backend
COPY --from=emscripten-build /Enflopio/build/Emscripten/package/ /
WORKDIR /app
ENTRYPOINT ["/venv/bin/python3", "-m", "http.server", "80"]
EXPOSE 80


FROM scratch as server
COPY --from=server-build /Enflopio/build/Release/package/ /
ENTRYPOINT ["/server"]
