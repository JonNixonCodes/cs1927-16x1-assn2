#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Item.h"
#include "Lists.h"

List newList() {
  List new = malloc(sizeof(*new));
  new->first = NULL;
  return new;
}

link newNode(Item path) {
  link new = malloc(sizeof(*new));
  new->item = path;
  new->next = NULL;
  return new;
}

List insertEnd(List list, link new) {
  assert(list != NULL);
  assert(new != NULL);
  link curr = list->first;
  if (curr == NULL) {
    list->first = new;
    return list;
  }
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = new;
  return list;
}

List insertFront(List list, link new) {
  assert(list != NULL);
  assert(new != NULL);
  new->next = list->first;
  list->first = new;
  return list;
}

Item getFirstItem(List list) {
  assert(list != NULL);
  assert(list->first != NULL);
  Item move = list->first->item;
  list->first = list->first->next;
  return move;
}

int isListEmpty(List list) {
  assert(list != NULL);
  if (list->first == NULL) {
    return 1;
  }
  return 0;
}

List emptyList(List list) {
  assert(list != NULL);
  link curr = list->first;
  link next = NULL;
  while (curr != NULL) {
    next = curr->next;
    free(curr);
    curr = next;
  }
  list->first = NULL;
  return list;
}

void deleteList(List list) {
  assert(list != NULL);
  link curr = list->first;
  link next = NULL;
  while (curr != NULL) {
    next = curr->next;
    free(curr);
    curr = next;
  }
  free(list);
}
