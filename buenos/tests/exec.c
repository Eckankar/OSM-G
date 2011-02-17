/*
 * Userland exec test
 */

#include "tests/lib.h"

static const char prog[] = "[arkimedes]hw"; /* The program to start. */

int main(void)
{
  uint32_t child = syscall_exec(prog);
  int ret;
  ret = (char)syscall_join(child);
  printf("Child %s joined with status: %d\n", prog, ret);
  syscall_halt();
  return 0;
}
