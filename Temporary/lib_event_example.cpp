#include <stdio.h>
#include <event2/event.h>
#include <fcntl.h>

void tmpCallback(int fd,short flags,void *p){
  printf("\n**My callback called**\n");
}

void event_base_add_virtual(struct event_base *);


int main(int argc, char **argv)
{
  // char * filename = "./foo";
  // int fd = open(filename, O_RDWR );
  // struct event_base *base = event_base_new();
  // event* ev = event_new(base,fd,EV_TIMEOUT|EV_WRITE,tmpCallback,NULL);
  // timeval tv = {2, 0};
  // int succ = event_add(ev,&tv);
  // printf("event_add succ = %d\n",succ);
  // event_base_dispatch(base);


  struct event_base *base = event_base_new();
  // event_base_add_virtual(base); // keep it from exiting
  event* ev = event_new(base,-1,EV_TIMEOUT,tmpCallback,NULL);
  event* ev2 = event_new(base,-1,EV_TIMEOUT,tmpCallback,NULL);
  timeval tv = {2, 0};
  int succ = event_add(ev,&tv);
  printf("event_add succ = %d\n",succ);
  succ = event_add(ev2,&tv);
  printf("event_add 2 succ = %d\n",succ);
  // event_base_dispatch(base);
  event_base_loop(base, 0x00);
  // event_base_loopcontinue(base)
  printf("End\n");
  return 0;
}
