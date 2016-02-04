// Item.h  ... definition for items

#ifndef ITEM_H
#define ITEM_H

#include "Graph.h"

typedef Edge Item;

#define ItemCopy(i)     (i)
#define ItemShow(i)     printf("%d %d",((i).v),((i).w))
#endif

