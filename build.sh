set -e    # if error, stop the build
cc -g -std=c99 -c -I /opt/raylib/src pong.c -o ./obj/pong.o
cc -o pong  obj/pong.o -s -Wall -std=c99 -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./pong
