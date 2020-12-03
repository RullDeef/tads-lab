#ifndef __AVL_H_
#define __AVL_H_

struct avl
{
    int depth;
    int diff; // -1 - левое глубже, 0, 1 - правое глубже

    int data;
    int count;

    struct avl *left;
    struct avl *right;
};

struct avl *avl_create(void);
void avl_destroy(struct avl **tree);

int avl_insert(struct avl **tree, int data);

// возвращает -1 если элемента нет, и 0 - если успешно удалён.
int avl_remove(struct avl *tree, int data);

// NULL - если не найдено.
struct avl *avl_find(struct avl *tree, int data);

#endif // __AVL_H_
