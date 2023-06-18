#ifndef VECTOR_SEARCH_H
#define VECTOR_SEARCH_H

#include <stdlib.h>

// binary search
long VectorSearch(float* array, const float key, const long imin,
                  const long imax);

// binary search with tasks
long VectorSearchParallel(float* array, const float key, const long imin,
                          const long imax);

// execute all searches
void ExecuteVectorSearches(const long size, const float key);

#endif /* VECTOR_SEARCH_H */

