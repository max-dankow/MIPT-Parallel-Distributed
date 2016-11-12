#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "master.h"
#include "slave.h"

const int MODE_MASTER = 0;
const int MODE_SLAVE = 1;

const char** read_net_args(int argc, const char* argv[], int* mode, char** addr, int* port) {
    if (argc < 3) {
        fprintf(stderr, "Wrong arguments.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-m") == 0) {
        *mode = MODE_MASTER;
        sscanf(argv[2], "%d", port);
        return argv + 2;
    }

    *mode = MODE_SLAVE;
    sscanf(argv[1], "%d", port);
    *addr = argv[2];
    return argv + 2;
}

int main(int argc, const char* argv[]) {
    // master: -m port [other arguments...]
    // slave: port address [other arguments...]
    int mode, port;
    char* addr;
    argv = read_net_args(argc, argv, &mode, &addr, &port);
    if (mode == MODE_MASTER) {
        run_master(port);
    } else {
        run_slave(port, addr);
    }
    return EXIT_SUCCESS;
}
