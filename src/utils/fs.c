#include "utils/fs.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int mkdir_recursive(char *path) {
  char buf[256] = { 0 };
  unsigned long plen = strlen(path), i = 0;
  struct stat st = { 0 };

  if (plen > 2 && path[0] == '.' && path[1] == '/') {
    i = 2;
  }

  strcpy(buf, path);

  for (; i < plen; ++i) {
    if (i < plen - 1 && path[i] != '/')
      continue;

    if (i < plen - 1)
      buf[i] = '\0';

    if (stat(buf, &st) == -1) {
      if (mkdir(buf, 0700) == -1) {
        perror("failed to make db dir");
        return errno;
      }
    }

    buf[i] = '/';
  }
  
  return 0;
}

