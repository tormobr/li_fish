
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct node{
    int v;
    struct node* next;
} node_t;

node_t* head = NULL;


int push(int val){
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node -> v = val;
    node -> next = head;
    head = node;

}

int main(){
    push(1);
    push(2);
    push(3);
    node_t* current = head;
    while (current != NULL){
        printf("%d\n", current->v);
        current = current -> next;
    }
    return 1;
}
