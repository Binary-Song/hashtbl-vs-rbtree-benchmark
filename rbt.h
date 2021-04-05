#ifndef _RBT_H_
#define _RBT_H_

typedef struct rbt rbt;
typedef struct rbt_node rbt_node;

/// 红黑树的每个结点都存放一个键值对
typedef struct
{
    char *key;
    void *value;
} rbt_pair;

/// 对红黑树操作的返回值
typedef enum  
{
    /// 修改成功。
    RBT_MODIFY_SUCCESS = 0,
    RBT_MODIFY_DUP_OR_MISSING = 1
} rbt_modify_result;

typedef int (*rbt_compare_f)(const void *key1, const void *key2);
typedef void (*rbt_val_dealloc_f)(void *p);
typedef void (*rbt_travfunc_t)(const char *key, const void *value_ptr);
typedef void (*rbt_travfunc_wa_t)(const char *key, const void *value_ptr, void* context);

/// 初始化红黑树。
/// @param compare 定义value之间的顺序
/// @param val_dealloc 回收value的函数
rbt *rbt_init(rbt_compare_f compare, rbt_val_dealloc_f val_dealloc);

/// 回收红黑树。
void rbt_deinit(rbt *p);

/// 给定key，返回value。如果未找到，返回null。
void *rbt_at(const rbt *tree, const char *key);

/// 按key从小到大的顺序，遍历红黑树。
/// @param f 对每个键值对执行的回调函数
void rbt_trav(const rbt *tree, rbt_travfunc_t f);

/// 按key从小到大的顺序，遍历红黑树。
/// @param f 对每个键值对执行的回调函数
void rbt_trav_with_arg(const rbt *t, rbt_travfunc_wa_t f, void *arg);

/// 删除红黑树的一个结点。
/// @retval RBT_MODIFY_SUCCESS：成功找到结点并删除。
/// @retval RBT_MODIFY_DUP_OR_MISSING：没找到结点，无操作。
rbt_modify_result rbt_remove(rbt *tree, const char *key);

/// 向红黑树中插入结点。
/// @param key_val 键值对
/// @retval RBT_MODIFY_SUCCESS：成功插入结点。
/// @retval RBT_MODIFY_DUP_OR_MISSING：已有此结点，无操作。
rbt_modify_result rbt_insert(rbt *tree, rbt_pair key_val);



#ifndef NDEBUG
void rbtdebug_property_asserts(const rbt *t);
#endif

#endif // _RBT_H_