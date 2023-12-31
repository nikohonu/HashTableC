#include "hashtable.h"

HashTable *hash_table_init() {
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
  ht->size = INIT_SIZE;
  ht->length = 0;
  ht->elements = (HashTableNode **)malloc(INIT_SIZE * sizeof(HashTableNode *));
  return ht;
}

// djb2 hash function
static unsigned long hash(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

static HashTableItem *create_item(char *key, void *value) {
  HashTableItem *item = (HashTableItem *)malloc(sizeof(HashTableItem));
  item->key = key;
  item->value = value;
  return item;
}

static HashTableNode *create_node(HashTableItem *item) {
  HashTableNode *node = (HashTableNode *)malloc(sizeof(HashTableNode));
  node->item = item;
  node->next = NULL;
  return node;
}

static bool insert(HashTableNode **elements, size_t size, HashTableItem *item) {
  size_t index = hash(item->key) % size;
  HashTableNode **current = &elements[index];
  while (*current) {
    if (strcmp((*current)->item->key, item->key) == 0) {
      free((*current)->item);
      (*current)->item = item;
      return false;
    }
    current = &(*current)->next;
  }
  *current = create_node(item);
  return true;
}

static void resize(HashTable *ht) {
  size_t new_size = ht->size * 2;
  HashTableNode **elements =
      (HashTableNode **)malloc(new_size * sizeof(HashTableNode *));
  for (size_t i = 0; i < ht->size; i++) {
    HashTableNode *node = ht->elements[i];
    while (node) {
      HashTableNode *prev = node;
      insert(elements, new_size, prev->item);
      node = node->next;
      free(prev);
    }
  }
  free(ht->elements);
  ht->elements = elements;
  ht->size = new_size;
}

bool hash_table_insert(HashTable *ht, char *key, void *value) {
  if ((ht->size * 0.7) < (ht->length + 1))
    resize(ht);
  HashTableItem *item = create_item(key, value);
  if (insert(ht->elements, ht->size, item))
    ht->length++;
  return true;
}

static HashTableNode *get_node(HashTable *ht, char *key) {
  size_t index = hash(key) % ht->size;
  HashTableNode *node = ht->elements[index];
  while (node) {
    if (strcmp(node->item->key, key) == 0)
      return node;
    node = node->next;
  }
  return NULL;
}

void *hash_table_get(HashTable *ht, char *key) {
  HashTableNode *node = get_node(ht, key);
  if (node) {
    return node->item->value;
  }
  return NULL;
}

bool hash_table_has(HashTable *ht, char *key) { return get_node(ht, key); }

HashTableItem **hash_table_items(HashTable *ht) {
  size_t length = ht->length;
  HashTableItem **items =
      (HashTableItem **)malloc(sizeof(HashTableItem *) * length);
  size_t index = 0;
  for (size_t i = 0; i < ht->size; i++) {
    HashTableNode *node = ht->elements[i];
    while (node) {
      items[index++] = node->item;
      node = node->next;
    }
  }
  return items;
}

void hash_table_print(HashTable *ht) {
  for (size_t i = 0; i < ht->size; i++) {
    HashTableNode *node = ht->elements[i];
    while (node) {
      printf("%zu %s %p\n", i, node->item->key, node);
      node = node->next;
    }
  }
}

void hash_table_remove(HashTable *ht, char *key) {
  size_t index = hash(key) % ht->size;
  HashTableNode **current = &(ht->elements[index]);
  while (*current) {
    if (strcmp((*current)->item->key, key) == 0) {
      HashTableNode *temp = *current;
      *current = (*current)->next;
      free(temp);
      ht->length--;
      return;
    }
    current = &((*current)->next);
  }
}

static void clear(HashTable *ht) {
  for (size_t i = 0; i < ht->size; i++) {
    HashTableNode **current = &(ht->elements[i]);
    while (*current) {
      HashTableNode *temp = *current;
      *current = (*current)->next;
      free(temp->item);
      free(temp);
    }
  }
  free(ht->elements);
}

void hash_table_clear(HashTable *ht) {
  clear(ht);
  ht->size = INIT_SIZE;
  ht->elements = (HashTableNode **)calloc(INIT_SIZE, sizeof(HashTableNode *));
  ht->length = 0;
}

void hash_table_free(HashTable *ht) {
  clear(ht);
  free(ht);
}
