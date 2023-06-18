/* thread placement functions for COMP8320 Ass1, 2009
 *      written by Peter Strazdins, SOCS ANU, August 2009.
 *      version 1.0 18/08/09
 */

#define NUM_GROUPS_CORE 2
#define NUM_STRANDS_GROUP 4
#define NUM_STRANDS_CORE (NUM_STRANDS_GROUP * NUM_GROUPS_CORE)

static inline int omp2cpuId(int ompid, int numThreads, int cores) {
  int thrPerCore = (numThreads + cores - 1) / cores;
  int coreNum, strandNum, i;

  for (coreNum = 0, i = 0; 1; coreNum++) {
    if (coreNum == numThreads % cores && coreNum > 0) thrPerCore--;
    if (i + thrPerCore > ompid) break;
    i += thrPerCore;
  }
  strandNum = ompid - i;
  return (coreNum * NUM_STRANDS_CORE + strandNum +
          ((strandNum < thrPerCore / NUM_GROUPS_CORE)
               ? 0
               : NUM_STRANDS_GROUP - thrPerCore / NUM_GROUPS_CORE));
}

// place each OpenMP thread id on virtual processor omp2cpuId(id, cores)
//      print out a warning for each thread on a separate line where this fails
// and print out a line indicating the list of processor ids, in order, that the
// OpenMP threads are on
void placeThreads(int cores);

