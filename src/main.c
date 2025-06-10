#include <stdio.h>
#include <stddef.h>

#include "server.h"


int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

    // Start server
    printf("Starting server...\n");
    server_start();

	return 0;
}
