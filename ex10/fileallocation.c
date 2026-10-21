cript started on Wednesday 21 October 2026 02:29:40 PM IST
[24bcs051@mepcolinux newex10]$cat newfile.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 50
#define INDEX_PTRS 4

typedef struct {
    char file_name[20];
    int file_size;
    int length;
    int start;
    int allocated;
} FileMeta;

typedef struct Node {
    int block_no;
    struct Node* next;
} Node;

typedef struct {
    char file_name[20];
    Node* head;
} LinkedFile;

typedef struct {
    int block_no;
    int ptr[INDEX_PTRS];
    int used;
} IndexBlock;

typedef struct {
    char file_name[20];
    int file_size;
    int num_data_blocks;
    int index_block;
    IndexBlock index;
} IndexedFile;

int BLOCK_SIZE = 1024;

int getRandomFreeBlock(int freeBlock[]) {
    int count = 0;
    int available[MAX];

    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1)
            available[count++] = i;
    }

    if (count == 0)
        return -1;

    int index = rand() % count;
    int block = available[index];
    freeBlock[block] = 0;

    return block;
}

int findContiguousFreeBlocks(int freeBlock[], int length) {
    int count = 0;
    int start_candidate = -1;

    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1) {
            if (count == 0)
                start_candidate = i;

            count++;

            if (count == length)
                return start_candidate;
        } else {
            count = 0;
            start_candidate = -1;
        }
    }

    return -1;
}

void resetFreeBlocks(int freeBlock[]) {
    for (int i = 0; i < MAX; i++)
        freeBlock[i] = 1;
}

void displayFreeBlocks(int freeBlock[]) {
    printf("\nFree Block List:\n");

    for (int i = 0; i < MAX; i++) {
        if (freeBlock[i] == 1)
            printf("%d ", i);
    }

    printf("\n");
}

/* 1. Contiguous Allocation */
void contiguous(FileMeta f[], int n, int freeBlock[]) {
    printf("\n--- CONTIGUOUS ALLOCATION ---\n");
    printf("Block Size: %d bytes\n\n", BLOCK_SIZE);

    printf("File\t\tSize(Bytes)\tLength\tStart\tEnd\tStatus\t\tAllocated Blocks\n");

    for (int i = 0; i < n; i++) {
        if (f[i].length == 0) {
            printf("%s\t%d\t\t0\t-\t-\tNo blocks needed\t-\n",
                   f[i].file_name, f[i].file_size);
            continue;
        }

        int start = findContiguousFreeBlocks(freeBlock, f[i].length);

        if (start != -1) {
            f[i].start = start;
            f[i].allocated = 1;

            int end_block = start + f[i].length - 1;

            for (int j = 0; j < f[i].length; j++)
                freeBlock[start + j] = 0;

            printf("%s\t%d\t\t%d\t%d\t%d\tAllocated\t",
                   f[i].file_name, f[i].file_size,
                   f[i].length, start, end_block);

            for (int j = 0; j < f[i].length; j++)
                printf("%d ", start + j);

            printf("\n");
        } else {
            f[i].start = -1;
            f[i].allocated = 0;

            printf("%s\t%d\t\t%d\t-\t-\tFailed (No space)\n",
                   f[i].file_name, f[i].file_size, f[i].length);
        }
    }
}

/* 2. Linked Allocation */
void linked(FileMeta f[], int n, int freeBlock[]) {
    LinkedFile lf[MAX];

    printf("\n--- LINKED ALLOCATION ---\n");
    printf("Block Size: %d bytes\n\n", BLOCK_SIZE);
    printf("File\tSize(Bytes)\tLength\tLinked Blocks\n");

    for (int i = 0; i < n; i++) {
        strcpy(lf[i].file_name, f[i].file_name);
        lf[i].head = NULL;

        Node* tail = NULL;
        int allocated_count = 0;

        if (f[i].length == 0) {
            printf("%s\t%d\t\t0\t(No blocks allocated)\n",
                   lf[i].file_name, f[i].file_size);
            continue;
        }

        for (int j = 0; j < f[i].length; j++) {
            int block = getRandomFreeBlock(freeBlock);

            if (block == -1) {
                printf("\nDisk full! Cannot allocate remaining blocks for %s\n",
                       lf[i].file_name);
                break;
            }

            Node* newNode = (Node*)malloc(sizeof(Node));

            if (newNode == NULL) {
                printf("\nMemory allocation failed.\n");
                freeBlock[block] = 1;
                break;
            }

            newNode->block_no = block;
            newNode->next = NULL;

            if (lf[i].head == NULL) {
                lf[i].head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }

            allocated_count++;
        }

        printf("%s\t%d\t\t%d\t",
               lf[i].file_name, f[i].file_size, f[i].length);

        Node* temp = lf[i].head;

        while (temp != NULL) {
            printf("%d", temp->block_no);

            Node* nextNode = temp->next;
            free(temp);
            temp = nextNode;

            if (temp != NULL)
                printf(" -> ");
        }

        if (allocated_count == f[i].length)
            printf(" -> END");
        else
            printf(" (PARTIAL: %d/%d blocks)",
                   allocated_count, f[i].length);

        printf("\n");
    }
}

/* 3. Single Indexed Allocation */
void singleIndexed(FileMeta f[], int n, int freeBlock[]) {
    printf("\n--- SINGLE INDEXED ALLOCATION ---\n");
    printf("Block Size: %d bytes\n", BLOCK_SIZE);
    printf("Pointers per index block: %d\n", INDEX_PTRS);
    printf("Maximum data blocks per file: %d\n\n", INDEX_PTRS);

    for (int i = 0; i < n; i++) {
        IndexedFile file;

        strcpy(file.file_name, f[i].file_name);
        file.file_size = f[i].file_size;
        file.num_data_blocks = f[i].length;
        file.index_block = -1;
        file.index.block_no = -1;
        file.index.used = 0;

        printf("File: %s | Size: %d bytes | Length: %d blocks\n",
               file.file_name, file.file_size, f[i].length);

        if (file.num_data_blocks == 0) {
            printf("  Empty file - no blocks allocated.\n\n");
            continue;
        }

        if (file.num_data_blocks > INDEX_PTRS) {
            printf("  Allocation failed: needs %d data blocks, "
                   "but maximum is %d.\n\n",
                   file.num_data_blocks, INDEX_PTRS);
            continue;
        }

        int ib = getRandomFreeBlock(freeBlock);

        if (ib == -1) {
            printf("  Allocation failed: no free block for index block.\n\n");
            continue;
        }

        file.index_block = ib;
        file.index.block_no = ib;

        int failed = 0;

        for (int j = 0; j < file.num_data_blocks; j++) {
            int b = getRandomFreeBlock(freeBlock);

            if (b == -1) {
                failed = 1;
                break;
            }

            file.index.ptr[file.index.used++] = b;
        }

        printf("  Index Block: %d\n", file.index_block);
        printf("  Data Blocks: ");

        for (int j = 0; j < file.index.used; j++)
            printf("%d ", file.index.ptr[j]);

        printf("\n");

        if (failed) {
            printf("  Status: PARTIALLY ALLOCATED (disk full)\n\n");
        } else {
            printf("  Status: FULLY ALLOCATED\n\n");
        }
    }
}

int main() {
    FileMeta f[MAX];
    int freeBlock[MAX];
    int n, choice;

    srand((unsigned int)time(NULL));
    resetFreeBlocks(freeBlock);

    printf("===== FILE ALLOCATION TECHNIQUES =====\n");

    printf("\nEnter Disk Block Size (in bytes): ");
    if (scanf("%d", &BLOCK_SIZE) != 1 || BLOCK_SIZE <= 0)
        BLOCK_SIZE = 1024;

    printf("\nEnter number of files (1-%d): ", MAX);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX) {
        printf("Invalid number of files.\n");
        return 1;
    }

    printf("\nEnter File Name and File Size in bytes:\n");

    for (int i = 0; i < n; i++) {
        printf("File %d [Name SizeInBytes]: ", i + 1);

        if (scanf("%19s %d", f[i].file_name, &f[i].file_size) != 2 ||
            f[i].file_size < 0) {
            printf("Invalid file input.\n");
            return 1;
        }

        if (f[i].file_size == 0)
            f[i].length = 0;
        else
            f[i].length =
                (f[i].file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

        f[i].start = -1;
        f[i].allocated = 0;
    }

    do {
        printf("\n===== MENU =====\n");
        printf("1. Contiguous Allocation\n");
        printf("2. Linked Allocation\n");
        printf("3. Single Indexed Allocation\n");
        printf("4. Display Free Block List\n");
        printf("5. Reset Free Blocks\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
            break;

        switch (choice) {
            case 1:
                resetFreeBlocks(freeBlock);
                contiguous(f, n, freeBlock);
                break;

            case 2:
                resetFreeBlocks(freeBlock);
                linked(f, n, freeBlock);
                break;

            case 3:
                resetFreeBlocks(freeBlock);
                singleIndexed(f, n, freeBlock);
                break;

            case 4:
                displayFreeBlocks(freeBlock);
                break;

            case 5:
                resetFreeBlocks(freeBlock);
                printf("\nFree block list reset completely.\n");
                break;

            case 6:
                printf("\nProgram Ended.\n");
                break;

            default:
                printf("\nInvalid Choice!\n");
        }

    } while (choice != 6);

    return 0;
}

[24bcs051@mepcolinux newex10]$g++ newfile.c
[24bcs051@mepcolinux newex10]$./a.out
===== FILE ALLOCATION TECHNIQUES =====

Enter Disk Block Size (in bytes): 4080

Enter number of files (1-50): 3

Enter File Name and File Size in bytes:
File 1 [Name SizeInBytes]: file1.txt 1024
File 2 [Name SizeInBytes]: file2.txt 1024
File 3 [Name SizeInBytes]: file3.txt 2056

===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 1

--- CONTIGUOUS ALLOCATION ---
Block Size: 4080 bytes

File            Size(Bytes)     Length  Start   End     Status          Allocated Blocks
file1.txt       1024            1       0       0       Allocated       0
file2.txt       1024            1       1       1       Allocated       1
file3.txt       2056            1       2       2       Allocated       2

===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 2

--- LINKED ALLOCATION ---
Block Size: 4080 bytes

File    Size(Bytes)     Length  Linked Blocks
file1.txt       1024            1       3 -> END
file2.txt       1024            1       40 -> END
file3.txt       2056            1       12 -> END

===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 3

--- SINGLE INDEXED ALLOCATION ---
Block Size: 4080 bytes
Pointers per index block: 4
Maximum data blocks per file: 4

File: file1.txt | Size: 1024 bytes | Length: 1 blocks
  Index Block: 20
  Data Blocks: 42
  Status: FULLY ALLOCATED

File: file2.txt | Size: 1024 bytes | Length: 1 blocks
  Index Block: 29
  Data Blocks: 18
  Status: FULLY ALLOCATED

File: file3.txt | Size: 2056 bytes | Length: 1 blocks
  Index Block: 43
  Data Blocks: 15
  Status: FULLY ALLOCATED


===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 4

Free Block List:
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 16 17 19 21 22 23 24 25 26 27 28 30 31 32 33 34 35 36 37 38 39 40 41 44 45 46 47 48 49

===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 5

Free block list reset completely.

===== MENU =====
1. Contiguous Allocation
2. Linked Allocation
3. Single Indexed Allocation
4. Display Free Block List
5. Reset Free Blocks
6. Exit
Enter your choice: 6

Program Ended.
[24bcs051@mepcolinux newex10]$exit
exit

Script done on Wednesday 21 October 2026 02:30:58 PM IST
