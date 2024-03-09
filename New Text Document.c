#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ucontext.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

void capture_state(int pid) {
    // Open the process memory file
    char mem_file[256];
    sprintf(mem_file, "/proc/%d/mem", pid);
    int mem_fd = open(mem_file, O_RDONLY);
    if (mem_fd == -1) {
        perror("Error opening memory file");
        exit(EXIT_FAILURE);
    }

    // Get the process registers
    ucontext_t context;
    if (syscall(SYS_getcontext, &context) == -1) {
        perror("Error getting context");
        close(mem_fd);
        exit(EXIT_FAILURE);
    }

    // Create a file to store the captured state
    int state_fd = open("captured_state.bin", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (state_fd == -1) {
        perror("Error creating state file");
        close(mem_fd);
        exit(EXIT_FAILURE);
    }

    // Write the process registers to the state file
    write(state_fd, &context, sizeof(context));

    // Read and write the process memory to the state file
    off_t offset = 0;
    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = pread(mem_fd, buffer, sizeof(buffer), offset)) > 0) {
        write(state_fd, buffer, bytesRead);
        offset += bytesRead;
    }

    // Close the files
    close(mem_fd);
    close(state_fd);

    printf("Process state captured successfully.\n");
}

int main() {
    // Get the process ID of the current process
    int pid = getpid();

    // Capture the state of the process
    capture_state(pid);

    return 0;
}
