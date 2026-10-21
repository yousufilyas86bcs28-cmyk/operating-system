#include <stdio.h>
#include <stdlib.h>

#define MAX_PAGES 100
#define MAX_FRAMES 100

int main()
{
    int pageSize;
    int processSize;
    int memorySize;

    int numPages;
    int numFrames;

    int pageTable[MAX_PAGES];
    int freeList[MAX_FRAMES];

    int choice;
    int i;

    printf("Enter process size: ");
    scanf("%d", &processSize);

    printf("Enter page size: ");
    scanf("%d", &pageSize);

    if (processSize <= 0 || pageSize <= 0)
    {
        printf("Invalid process size or page size.\n");
        return 1;
    }
    numPages = (processSize + pageSize - 1) / pageSize;

    printf("Enter physical memory size: ");
    scanf("%d", &memorySize);

    if (memorySize <= 0)
    {
        printf("Invalid memory size.\n");
        return 1;
    }
    numFrames = memorySize / pageSize;

    if (numPages > MAX_PAGES || numFrames > MAX_FRAMES)
    {
        printf("Memory or process size exceeds the program limit.\n");
        return 1;
    }

    if (numFrames < numPages)
    {
        printf("\nNot enough frames to allocate all pages.\n");
        printf("Pages required  : %d\n", numPages);
        printf("Frames available: %d\n", numFrames);
        return 1;
    }
    for (i = 0; i < numFrames; i++)
    {
        freeList[i] = i;
    }
    for (i = numFrames - 1; i > 0; i--)
    {
        int randomIndex = rand() % (i + 1);

        int temp = freeList[i];
        freeList[i] = freeList[randomIndex];
        freeList[randomIndex] = temp;
    }
    for (i = 0; i < numPages; i++)
    {
        pageTable[i] = freeList[i];
    }

    printf("\nPaging initialized successfully.\n");
    printf("Number of pages  : %d\n", numPages);
    printf("Number of frames : %d\n", numFrames);

    do
    {
        printf("1. Display Page Table\n");
        printf("2. Display Number of Pages\n");
        printf("3. Display Internal Fragmentation\n");
        printf("4. Display External Fragmentation\n");
        printf("5. Logical to Physical Address Conversion\n");
        printf("6. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("\nPage Table:\n");
                printf("Page Number\tFrame Number\n");

                for (i = 0; i < numPages; i++)
                {
                    printf("%d\t\t%d\n", i, pageTable[i]);
                }
                break;

            case 2:
                printf("\nNumber of Pages  = %d\n", numPages);
                printf("Number of Frames = %d\n", numFrames);
                break;

            case 3:
            {
                int allocatedMemory;
                int internalFragmentation;

                allocatedMemory = numPages * pageSize;
                internalFragmentation = allocatedMemory - processSize;

                printf("\nAllocated Memory       = %d bytes\n",
                       allocatedMemory);
                printf("Process Size           = %d bytes\n",
                       processSize);
                printf("Internal Fragmentation = %d bytes\n",
                       internalFragmentation);

                break;
            }

            case 4:
            {
                int unusedFrames;
                int externalFragmentation;

                unusedFrames = numFrames - numPages;
                externalFragmentation = unusedFrames * pageSize;

                printf("\nUnused Frames          = %d\n", unusedFrames);
                printf("External Fragmentation = %d bytes\n",
                       externalFragmentation);
                printf("In paging, unused frames remain available.\n");

                break;
            }

            case 5:
            {
                int logicalAddress;
                int pageNumber;
                int offset;
                int frameNumber;
                int physicalAddress;

                printf("\nEnter Logical Address: ");
                scanf("%d", &logicalAddress);

                if (logicalAddress < 0 ||
                    logicalAddress >= processSize)
                {
                    printf("Invalid Logical Address!\n");
                    break;
                }

                pageNumber = logicalAddress / pageSize;
                offset = logicalAddress % pageSize;

                frameNumber = pageTable[pageNumber];

                physicalAddress =
                    frameNumber * pageSize + offset;

                printf("\nLogical Address  = %d\n", logicalAddress);
                printf("Page Number      = %d\n", pageNumber);
                printf("Offset           = %d\n", offset);
                printf("Frame Number     = %d\n", frameNumber);
                printf("Physical Address = %d\n", physicalAddress);

                break;
            }

            case 6:
                printf("\nExiting program...\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 6);

    return 0;
}
