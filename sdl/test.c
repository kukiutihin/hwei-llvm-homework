#include <stdlib.h>
#include <stdio.h>

int func(int x) {
  int b = 0;
  if (x > 3)
    b = 1;
  else
    b = 2;

  return b;
}

int main(int argc, char* argv[]) {
  printf("%i", func(atoi(argv[2])));
}
