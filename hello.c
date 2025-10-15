#include <unistd.h>
#include <stdlib.h>

void main()
{
  write(1, "nice!\n", 6);
  _exit(0xa4);
}
