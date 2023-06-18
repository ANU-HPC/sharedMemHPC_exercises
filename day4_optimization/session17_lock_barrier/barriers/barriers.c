/* written by Xi Yang, 2009.
   SPARC port by Peter Strazdins, SoCS ANU, 08/09
*/

#include "barriers.h"

/*Central barrier*/
void init_central_barrier(struct cen_barrier *ba, unsigned int nr_thread) {
  ba->nr_thread = nr_thread;
  ba->counter = 0;
  ba->sense = TRUE;
}

void central_barrier_wait(struct cen_barrier *ba) {
  unsigned char volatile local_sense;
  unsigned int nr_waiting_threads;
#ifdef DEBUG
  printf("Cen_wait\n");
#endif
  local_sense = ba->sense ^ 1;
  nr_waiting_threads = fetch_add(&ba->counter, 1);
#ifdef DEBUG
  printf("central_barrier_wait: nr_waiting_threads=%d\n", nr_waiting_threads);
#endif
  if (nr_waiting_threads == ba->nr_thread - 1) {
#ifdef DEBUG
    printf("Last one\n");
#endif
    /*I am the last one*/
    ba->counter = 0;
    ba->sense ^= 1;
  } else {
#ifdef DEBUG
    printf("Nr_waiting=%d\n", nr_waiting_threads);
    printf("Begin waiting\n");
#endif
    while (local_sense != ba->sense)
      ;
#ifdef DEBUG
    printf("End waiting\n");
#endif
  }
}

/*
 *Combining tree barrier
 *If complete, return 0
 *If can not malloc space, return -1
 */

void print_com_tree(struct com_barrier *p_bar) {
  unsigned int i;
  printf(
      "\n------\n"
      "nr_th-%d,nr_node-%d\n",
      p_bar->nr_thread, p_bar->nr_node);
  for (i = 0; i < p_bar->nr_node; i++) {
    printf("N-%d:Con-%d,thr-%d,Sen-%d\n", i, p_bar->tree[i].counter,
           p_bar->tree[i].nr_thread, p_bar->tree[i].sense);
  }
}

int init_com_barrier(struct com_barrier *p_bar, unsigned int nr_thread) {
  unsigned int exp = 0;
  unsigned int nr_node = 0;
  unsigned int i;

  /*Try to find, how many nodes we need*/
#ifdef SPARC
  for (exp = 31; (nr_thread & (1 << exp)) == 0; exp--)
    ;
#else
  asm("bsrl %1,%0" : "=r"(exp) : "r"(nr_thread));
#endif
  if (nr_thread != 1 << exp) exp++;
  nr_node = 1 << exp;
#ifdef DEBUG
  printf("Com_barrier_init, nr_thread=%d,nr_node=%d,node size is %ld\n",
         nr_thread, nr_node, sizeof(struct com_tree_node));
#endif
  p_bar->nr_thread = nr_thread;
  p_bar->nr_node = nr_node;
  p_bar->tree =
      (struct com_tree_node *)malloc(sizeof(struct com_tree_node) * nr_node);

  if (p_bar->tree == NULL) {
    fprintf(stderr, "Error, can not malloc space for p_bar -> tree\n");
    return -1;
  }

  /*Init tree*/
  for (i = 0; i < nr_node; i++) {
    p_bar->tree[i].counter = 0;
    p_bar->tree[i].nr_thread = 0;
    p_bar->tree[i].sense = TRUE;
    p_bar->tree[i].id = i;
  }

  for (i = 0; i < nr_thread; i++) p_bar->tree[(i + nr_node) / 2].nr_thread++;

  for (i = nr_node - 1; i >= 1; i--) {
    if (p_bar->tree[i].nr_thread) p_bar->tree[i / 2].nr_thread++;
  }
#ifdef DEBUG
  print_com_tree(p_bar);
#endif
  return 0;
}

void com_barrier_destroy(struct com_barrier *p_bar) { free(p_bar->tree); }

static void com_barrier_wait_node(struct com_tree_node *nod,
                                  struct com_barrier *p_bar) {
  unsigned int volatile local_sense = nod->sense ^ 1;
  unsigned int nr_waiting_threads;
  nr_waiting_threads = fetch_add(&nod->counter, 1);
#ifdef DEBUG
  printf("com_barrier_wait: nr_waiting_threads=%d\n", nr_waiting_threads);
#endif

  if (nr_waiting_threads == nod->nr_thread - 1) {
    if (nod->id / 2 != 0) {
      /*Still have parents*/
      com_barrier_wait_node(&(p_bar->tree[nod->id / 2]), p_bar);
    }
    /*Ok, all ready*/
    nod->counter = 0;
    nod->sense ^= 1;
  } else {
    /*Waiting*/
    while (local_sense != nod->sense)
      ;
  }
}

void com_barrier_wait(struct com_barrier *p_bar, unsigned int id) {
  struct com_tree_node *my_node = &(p_bar->tree[(id + p_bar->nr_node) / 2]);
  com_barrier_wait_node(my_node, p_bar);
}

/*
 *Scalable barrier*
 *        root(thread1)
 *       /             \
 *      /	        \
 *   node(thread2)   node(thread3)
 *    /     \
 *   /	     \
 *  node(t4)  node(t5)
 */

int init_scal_barrier(struct scal_barrier *p_bar, unsigned int nr_thread) {
  unsigned int exp = 0;
  unsigned int nr_node = 0;
  unsigned int i;

  /*Try to find, how many nodes we need*/
#ifdef SPARC
  for (exp = 31; ((nr_thread & (1 << exp)) == 0); exp--)
    ;
#else
  asm("bsrl %1,%0" : "=r"(exp) : "r"(nr_thread));
#endif
  exp++;
  nr_node = 1 << exp;

#ifdef DEBUG
  printf("Scal_barrier_init, nr_thread=%d,nr_node=%d, node size is %ld\n",
         nr_thread, nr_node, sizeof(struct scal_tree_node));
#endif
  p_bar->nr_thread = nr_thread;
  p_bar->nr_node = nr_node;
  p_bar->tree =
      (struct scal_tree_node *)malloc(sizeof(struct scal_tree_node) * nr_node);

  if (p_bar->tree == NULL) {
    fprintf(stderr, "Error, can not malloc space for p_bar -> tree\n");
    return -1;
  }

  for (i = 0; i < nr_node; i++) {
    p_bar->tree[i].child_ready.ready = CHILD_READY;
    p_bar->tree[i].parent_sense = TRUE;
  }

  for (i = nr_thread; i >= 1; i--)
    p_bar->tree[i / 2].child_ready.chk[i & 0x1] = CHILD_NOT_READY;

  return 0;
}

void scal_barrier_destroy(struct scal_barrier *p_bar) { free(p_bar->tree); }

void scal_barrier_wait(struct scal_barrier *p_bar, unsigned int id) {
  volatile unsigned char local_sense;
  volatile unsigned short *pready;
  struct scal_tree_node *parent_node;
  /*Id is form 0 -- N-1, change it to 1 --- N*/
  id++;
  parent_node = &p_bar->tree[id / 2];
  local_sense = p_bar->tree[id].parent_sense ^ 1;
#ifdef DEBUG
  printf("ID=%d, P_ID=%d\n", id, id / 2);
#endif
  /*Waiting for left and right child*/

#if 0
  while(p_bar->tree[id].child_ready.ready) 
    ;
#else /* bug fix P.S. 08/09 - above code breaks under optimization */
  pready = &p_bar->tree[id].child_ready.ready;
  while (*pready)
    ;
#endif

  if (2 * id <= p_bar->nr_thread)
    p_bar->tree[id].child_ready.chk[0] = CHILD_NOT_READY;
  if (2 * id + 1 <= p_bar->nr_thread)
    p_bar->tree[id].child_ready.chk[1] = CHILD_NOT_READY;

#ifdef DEBUG
  printf("Children are ready, i am %d\n", id);
#endif
  /*Tell parent, I am ready*/
  parent_node->child_ready.chk[id & 0x1] = CHILD_READY;

  if (id != 1) {
    /*So, I am not root, waiting for parents*/
    while (p_bar->tree[id].parent_sense != local_sense)
      ;
  }
#ifdef DEBUG
  printf("Wak up children, i am %d\n", id);
#endif
  if (2 * id < p_bar->nr_node - 1) {
    p_bar->tree[2 * id].parent_sense ^= 1;
    p_bar->tree[2 * id + 1].parent_sense ^= 1;
  }
}

