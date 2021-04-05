#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <unordered_map>
#include <iostream>
extern "C"
{
#include "rbt.h"
#include "ht.h"
}

int gsize = 0;

class RedBlackTree
{
    rbt *tree;

public:
    RedBlackTree()
    {
        tree = rbt_init((rbt_compare_f)strcmp, free);
    }

    void Add(const char *key)
    {
        rbt_pair pair;
        pair.key = strdup(key);
        pair.value = (void *)0;
        rbt_insert(tree, pair);
    }

    int Find(const char *key)
    {
        return (int)rbt_at(tree, key);
    }

    int Size()
    {
        gsize = 0;
        rbt_trav(tree, [](const char *key, const void *value_ptr) {
            gsize++;
        });
        return gsize;
    }
};

class HashTable
{
    hashtable_t *hashtbl;

public:
    HashTable(int capacity)
    {
        hashtbl = ht_create(capacity);
    }

    void Add(const char *key)
    {
        ht_put(hashtbl, (char *)key, (void *)0);
    }

    int Find(const char *key)
    {
        return (int)ht_get(hashtbl, (char *)key);
    }
};

template <typename T>
int InitContainer(T &container, long size)
{
    auto time1 = clock();
    for (long i = 0; i < size; i++)
    {
        std::string str = std::to_string(i);
        container.Add(str.c_str());
    }
    auto delta_time = clock() - time1;
    return (int)delta_time;
}

template <typename T>
int BenchmarkSearch(T container, int size)
{
    auto time1 = clock();
    for (int i = 0; i < size; i++)
    {
        std::string str = std::to_string(i);
        container.Find(str.c_str());
    }
    auto delta_time = clock() - time1;
    return (int)delta_time;
}

const char out_file_path[] = "D:/Projects/hashtbl-vs-rbtree-benchmark/out.txt";
int main()
{
    FILE *outfile = fopen(out_file_path, "w");
    fprintf(outfile, "%s\t%s\n", "ht", "rbt");
    for (int i = 0; i < 18; i++)
    {
        HashTable ht(300);
        RedBlackTree rbt;
        int size = pow(2, i + 10);
        int it1 = InitContainer(ht, size);
        int it2 = InitContainer(rbt, size);
        int t1 = BenchmarkSearch(ht, 1000000);
        int t2 = BenchmarkSearch(rbt, 1000000);
        int tree_size = rbt.Size();
        fprintf(outfile, "%d\t%d\n", t1, t2);
        printf("hashtable: init = %d, search = %d. red-black tree: init = %d, search = %d, size=%d\n", it1, t1, it2, t2, tree_size);
    }
    return 0;
}