#ifndef _HT_H_
#define _HT_H_
typedef struct hashtable_t hashtable_t;

hashtable_t *ht_create(unsigned int capacity);
void *ht_put(hashtable_t *hasht, char *key, void *data);
void *ht_get(hashtable_t *hasht, char *key);
void ht_destroy(hashtable_t *hasht);
#endif // _HT_H_