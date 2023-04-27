#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILE_NAME "input.txt"
#define DATA_COUNT 2
#define INSERT 0
#define DELETE 1
#define COMPLETE -1

typedef struct priority_queue *queue_pointer;
struct priority_queue {
    int priority;
    int computing_time;
    queue_pointer left_link, right_link;
};

typedef struct queue_head* head_pointer;
struct queue_head {
    queue_pointer left_link;
    queue_pointer right_link;
};

typedef struct input_data* input_pointer;
struct input_data {
    int type;
    int priority;
    int computing_time;
};

void initialize_queue(void);
int insert_queue(int priority, int computing_time);
int delete_queue(int priority);
void print_queue(void);
FILE* open_file(void);
input_pointer extract_value(char* line);

head_pointer header[3];

int main(int argc, char const *argv[]) {
    initialize_queue();
    print_queue();
}

void initialize_queue(void) {
    FILE* fp = NULL;
    char buffer[256] = "";
    char* line = NULL;
    char backspace = '\b';

    fp = open_file();

    for (size_t i = 0; i < sizeof(header); i++) {
        header[i] = (head_pointer) malloc(sizeof(struct queue_head));
        header[i] -> left_link = NULL;
        header[i] -> right_link = NULL; 
    }

    while (1) {
        line = fgets(buffer, 256, fp);

        if (line == NULL)
            break;

        input_pointer input = extract_value(line);

        if (input->type == INSERT) {
            insert_queue(input->priority, input->computing_time);   
        } else if (input->type == DELETE) {
            delete_queue(input->priority);
        } else if (input->type == COMPLETE) {
            break;
        }
    }
    fclose(fp);
}

int insert_queue(int priority, int computing_time) {
    queue_pointer newJob = (queue_pointer) malloc(sizeof(struct priority_queue));
    queue_pointer currentJob, previous;
    head_pointer head;

    if (newJob == NULL) {
        printf("Memory allocation error\n");
        return -1;
    }

    head = header[priority/10];
    newJob->priority = priority;
    newJob->computing_time = computing_time;
    newJob->right_link = NULL; 

    if (head->right_link == NULL) {
        head->right_link = newJob;
        newJob->left_link = head;
    } else {
        currentJob = head->right_link;
        previous = NULL;
        while (currentJob != NULL && priority >= currentJob->priority) {
            if (priority == newJob->priority && computing_time < newJob->computing_time) {
                break;
            }
            previous = newJob;
            currentJob = currentJob->right_link;
        }
        if (currentJob == head->right_link) {
            newJob->right_link = head->right_link;
            head->right_link->left_link = newJob;
            head->right_link = newJob;
        } else if (currentJob == NULL) {
            newJob->right_link = previous;
            newJob->left_link = previous->left_link;
            previous->left_link->right_link = newJob;
            previous->left_link = newJob;
        } else {
            newJob->left_link = currentJob->left_link;
            newJob->right_link = currentJob;
            currentJob->left_link->right_link = newJob;
            currentJob->left_link = newJob;
        }
    }
    return 0;
}

int delete_queue(int priority) {
    queue_pointer currentJob;
    head_pointer head = header[priority/10];

    if (head->right_link == NULL) {
        printf("Queue is empty.");
        return -1;
    } else {
        currentJob = head->right_link;

        while (currentJob != NULL && currentJob->priority <= priority) {
            currentJob = currentJob->right_link;
        }

        if (currentJob == NULL) {
            printf("Node with priority %d not found\n", priority);
            return -1;
        } else 
            if (currentJob == head ->right_link) {
                head->right_link = NULL;

            }
            
        }

        currentJob->left_link->right_link = currentJob->right_link;
        currentJob->right_link->left_link = currentJob->left_link;
        free(currentJob);

}

void print_queue() {
    for (size_t i = 0; i < sizeof(header); i++) {
        queue_pointer current_node;
        printf("Priority Queue:\n");
        current_node = header[i]->right_link;
        while (current_node != header[i]) {
            printf("Priority: %d, Computing Time: %d\n", current_node->priority, current_node->computing_time);
            current_node = current_node->right_link;
        }
    }
}

FILE* open_file(void) {
    FILE* fp;
    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("open error!\n");
        exit(0);
    }
    return fp;
}

input_pointer extract_value(char* line) {
    char* data1= strtok(line, " ");
    char* data2 = strtok(NULL, " ");
    char* data3 = strtok(NULL, " ");

    input_pointer result = (input_pointer) malloc(sizeof(struct input_data));
    result->type = atoi(data1);
    result->priority = atoi(data2);
    result->computing_time = atoi(data3);
    return result;
}