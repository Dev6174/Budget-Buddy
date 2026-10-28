/* Intelligent Financial Planning and Budgeting System
   Simple menu-driven program in C (linked list, tree, graph) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- Data structures ---------- */
struct Transaction {            /* one node of the linked list */
    int id;
    char type[10];              /* "Income" or "Expense" */
    char category[20];
    float amount;
    struct Transaction *next;
};

struct CategoryNode {           /* one node of the binary tree */
    char name[30];
    struct CategoryNode *left, *right;
};

struct Transaction *head = NULL;    /* start of the list (NULL = empty) */
int nextId = 1;
float budget = 0;                   /* 0 means "not set" */

#define V 5                         /* number of departments */
char dept[V][12] = {"Finance", "Sales", "Marketing", "HR", "Operations"};
int adj[V][V] = {                   /* 1 = departments are connected */
    {0, 1, 0, 1, 1},
    {1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0}
};
int visited[V];

/* ---------- Small helper ---------- */
void clearInput() {                 /* throw away the rest of the line */
    scanf("%*[^\n]");
    scanf("%*c");
}

/* ---------- Module 1: Transactions (linked list) ---------- */
void addTransaction(char *type) {
    struct Transaction *n = malloc(sizeof(struct Transaction));
    if (n == NULL) { printf("Memory error\n"); return; }

    printf("Category: ");
    scanf("%19s", n->category);
    printf("Amount: ");
    if (scanf("%f", &n->amount) != 1 || n->amount <= 0) {
        printf("Invalid amount\n");
        clearInput();
        free(n);
        return;
    }
    n->id = nextId++;
    strcpy(n->type, type);
    n->next = NULL;

    if (head == NULL) {             /* first node */
        head = n;
    } else {                        /* walk to the last node and attach */
        struct Transaction *t = head;
        while (t->next != NULL) t = t->next;
        t->next = n;
    }
    printf("Added with ID %d\n", n->id);
}

void displayTransactions() {
    if (head == NULL) { printf("No transactions\n"); return; }
    for (struct Transaction *t = head; t != NULL; t = t->next)
        printf("%-3d %-8s %-12s %9.2f\n", t->id, t->type, t->category, t->amount);
}

/* ---------- Module 2: Search and Sort ---------- */
void searchTransaction() {
    int id;
    printf("Enter ID: ");
    if (scanf("%d", &id) != 1) { printf("Invalid input\n"); clearInput(); return; }
    for (struct Transaction *t = head; t != NULL; t = t->next) {   /* Linear Search */
        if (t->id == id) {
            printf("Found: %s %s %.2f\n", t->type, t->category, t->amount);
            return;
        }
    }
    printf("ID not found\n");
}

void sortTransactions() {           /* Bubble Sort: swap data, not links */
    if (head == NULL) { printf("No transactions\n"); return; }
    for (struct Transaction *a = head; a != NULL; a = a->next) {
        for (struct Transaction *b = head; b->next != NULL; b = b->next) {
            if (b->amount > b->next->amount) {
                struct Transaction *c = b->next;
                int id = b->id;           b->id = c->id;           c->id = id;
                float amt = b->amount;    b->amount = c->amount;   c->amount = amt;
                char tmp[20];
                strcpy(tmp, b->type);     strcpy(b->type, c->type);         strcpy(c->type, tmp);
                strcpy(tmp, b->category); strcpy(b->category, c->category); strcpy(c->category, tmp);
            }
        }
    }
    displayTransactions();
}

/* ---------- Module 3: Budget and Summary ---------- */
void setBudget() {
    float b;
    printf("Monthly budget: ");
    if (scanf("%f", &b) != 1 || b <= 0) {
        printf("Invalid budget\n");
        clearInput();
        return;
    }
    budget = b;
    printf("Budget saved\n");
}

void showSummary() {
    float income = 0, expenses = 0;
    for (struct Transaction *t = head; t != NULL; t = t->next) {
        if (strcmp(t->type, "Income") == 0) income += t->amount;
        else expenses += t->amount;
    }
    printf("Income:   %.2f\n", income);
    printf("Expenses: %.2f\n", expenses);
    printf("Net:      %.2f\n", income - expenses);

    if (budget == 0)                    printf("Budget not set\n");
    else if (expenses > budget)         printf("ALERT: budget exceeded\n");
    else if (expenses >= 0.8 * budget)  printf("WARNING: 80%% of budget reached\n");
    else                                printf("Within budget\n");
}

/* ---------- Module 4: Category tree (binary tree) ---------- */
struct CategoryNode *createCategoryNode(char *name, struct CategoryNode *l, struct CategoryNode *r) {
    struct CategoryNode *n = malloc(sizeof(struct CategoryNode));
    if (n == NULL) { printf("Memory error\n"); exit(1); }
    strcpy(n->name, name);
    n->left = l;
    n->right = r;
    return n;
}

void displayCategories(struct CategoryNode *root) {     /* Inorder: left, node, right */
    if (root == NULL) return;
    displayCategories(root->left);
    printf("%s ", root->name);
    displayCategories(root->right);
}

void freeCategoryTree(struct CategoryNode *root) {      /* Post-order: children first */
    if (root == NULL) return;
    freeCategoryTree(root->left);
    freeCategoryTree(root->right);
    free(root);
}

/* ---------- Module 5: Department graph (BFS and DFS) ---------- */
void displayGraph() {
    for (int i = 0; i < V; i++) {
        printf("%-10s -> ", dept[i]);
        for (int j = 0; j < V; j++)
            if (adj[i][j]) printf("%s ", dept[j]);
        printf("\n");
    }
}

int readDept() {                    /* ask for a start department, clear visited */
    int s;
    for (int i = 0; i < V; i++) printf("%d=%s  ", i, dept[i]);
    printf("\nStart department: ");
    if (scanf("%d", &s) != 1 || s < 0 || s >= V) {
        printf("Invalid department\n");
        clearInput();
        return -1;
    }
    for (int i = 0; i < V; i++) visited[i] = 0;
    return s;
}

void BFS(int s) {                   /* uses a queue (array with front and rear) */
    int q[V], front = 0, rear = 0;
    visited[s] = 1;
    q[rear++] = s;
    while (front < rear) {
        int u = q[front++];
        printf("%s ", dept[u]);
        for (int v = 0; v < V; v++)
            if (adj[u][v] && !visited[v]) { visited[v] = 1; q[rear++] = v; }
    }
    printf("\n");
}

void DFS(int u) {                   /* uses recursion */
    visited[u] = 1;
    printf("%s ", dept[u]);
    for (int v = 0; v < V; v++)
        if (adj[u][v] && !visited[v]) DFS(v);
}

/* ---------- Free the list ---------- */
void freeList() {
    while (head != NULL) {
        struct Transaction *temp = head;
        head = head->next;
        free(temp);
    }
}

/* ---------- Main menu ---------- */
int main() {
    /* build the fixed category tree */
    struct CategoryNode *root = createCategoryNode("Expenses",
        createCategoryNode("Operations",
            createCategoryNode("Rent", NULL, NULL),
            createCategoryNode("Electricity", NULL, NULL)),
        createCategoryNode("People", NULL,
            createCategoryNode("Salaries", NULL, NULL)));

    int choice = 0, s;
    while (choice != 12) {
        printf("\n===== Financial Planner =====\n");
        printf(" 1 Add Income            7 View Financial Summary\n");
        printf(" 2 Add Expense           8 Display Category Tree\n");
        printf(" 3 Display Transactions  9 Display Department Graph\n");
        printf(" 4 Search Transaction   10 Run BFS\n");
        printf(" 5 Sort Transactions    11 Run DFS\n");
        printf(" 6 Set Monthly Budget   12 Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { clearInput(); choice = 0; }

        switch (choice) {
            case 1:  addTransaction("Income");  break;
            case 2:  addTransaction("Expense"); break;
            case 3:  displayTransactions();     break;
            case 4:  searchTransaction();       break;
            case 5:  sortTransactions();        break;
            case 6:  setBudget();               break;
            case 7:  showSummary();             break;
            case 8:  displayCategories(root); printf("\n"); break;
            case 9:  displayGraph();            break;
            case 10: s = readDept(); if (s >= 0) BFS(s); break;
            case 11: s = readDept(); if (s >= 0) { DFS(s); printf("\n"); } break;
            case 12: printf("Goodbye!\n");      break;
            default: printf("Invalid choice\n");
        }
    }
    freeList();
    freeCategoryTree(root);
    return 0;
}