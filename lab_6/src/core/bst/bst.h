#ifndef __BST_H_
#define __BST_H_

#pragma pack(push, 4)

struct bst
{
    int depth;
    int data;
    int count;

    struct bst *left;
    struct bst *right;
};

struct bst *bst_create(void);
void bst_destroy(struct bst **tree);

int bst_insert(struct bst **tree, int data, int *cmp);
int bst_shallow_insert(struct bst **tree, int data);

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int bst_remove(struct bst *tree, int data);

// NULL - если не найдено.
struct bst *bst_find(struct bst *tree, int data, int *cmp);

#endif // __BST_H_
