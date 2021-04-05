#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <unordered_map>
#include <map>
#include <iostream>

int gsize = 0;

class RedBlackTree
{
    std::map<std::string, int> map;

public:
    void Add(std::string key)
    {
        map[key] = 1;
    }

    int Find(std::string key)
    {
        return map[key];
    }

    int Size()
    {
        return map.size();
    }
};

class HashTable
{
    std::unordered_map<std::string, int> map;

public:
    void Add(std::string key)
    {
        map[key] = 1;
    }

    int Find(std::string key)
    {
        return map[key];
    }

    int Size()
    {
        return map.size();
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
        HashTable ht;
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

