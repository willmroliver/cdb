#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

int main(int argc, char** argv) {
  size_t len;
  char* cmd;

  while (true) {
    cmd = fgetln(stdin, &len);

    if (ferror(stdin)) {
      perror("error reading input: ");
      return 1;
    }

    if (cmd == NULL || len == 0 || *cmd == '\n') {
      break;
    }

    fwrite(cmd, len, 1, stdout);
    fflush(stdout);
  }
  
  return 0;
}
