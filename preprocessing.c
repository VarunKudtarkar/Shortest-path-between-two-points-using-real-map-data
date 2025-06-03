#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct to store nodes and its data
typedef struct node{
    char id[15];
    float lat;
    float log;
    struct node *next;
}node;

typedef node *Node_Head;

//Struct to store adjacent nodes and related data
typedef struct edge{
    node *id1;
    node *id2;
    struct edge *next;
}edge;

typedef edge *Edge_head;

//Initialisation of both lists
void node_init(Node_Head *head){
    *head = NULL;
    return;
}

void edge_init(Edge_head *head){
    *head = NULL;
    return;
}

int check(char *buffer){
    int i;
    char temp[8] = "  <node";

    for(i = 0; i < 7; i++){
        if(temp[i] != buffer[i])
            return 0;
    }
    return 1;
}

//Function to store node data in a linked list
int fill(FILE *fp, Node_Head *head){
    node *p, *nn;
    char buffer[250], s[10] = " <>/=\"", *tok, temp[15];
    int i = 0;

    p = *head;

    while(fgets(buffer, sizeof(buffer), fp)){
        if(!check(buffer)){
            continue;
        }

        nn = (node *)malloc(sizeof(node));

        if(nn){
            tok = strtok(buffer, s);
            tok = strtok(NULL, s);
            tok = strtok(NULL, s);

            strcpy(nn -> id, tok);

            tok = strtok(NULL, s);
            tok = strtok(NULL, s);

            strcpy(temp, tok);
            nn -> lat = atof(temp);

            tok = strtok(NULL, s);
            tok = strtok(NULL, s);

            strcpy(temp, tok);
            nn -> log = atof(temp);

            nn -> next = NULL;

            i++;
        }
        else
            return i;

        if(!*head){
            *head = nn;
            p = nn;
            continue;
        }

        p -> next = nn;
        p = p -> next;
    }

    return i;
}

//Function to remove duplicates in Node list
int rem_dup1(Node_Head *head, int i){
    node *curr1 = *head;

    while(curr1){
        node *curr2 = curr1;

        while(curr2 -> next){
            if(strcmp(curr2 -> next -> id, curr1 -> id) == 0){
                node *dup = curr2 -> next;

                curr2 -> next = curr2 -> next -> next;

                free(dup);
                i--;
            }
            else
                curr2 = curr2 -> next;
        }

        curr1 = curr1 -> next;
    }

    return i;
}

int check1(char *buffer){
    int i;
    char temp[7] = "  <way";

    for(i = 0; i < 5; i++){
        if(temp[i] != buffer[i])
            return 0;
    }
    return 1;
}

int check2(char *buffer){
    int i;
    char temp[10] = "  </way>";

    for(i = 0; i < 7; i++){
        if(temp[i] != buffer[i])
            return 0;
    }
    return 1;
}

int check3(char *buffer){
    int i;
    char temp[15] = "    <tag";

    for(i = 0; i < 8 ; i++){
        if(temp[i] != buffer[i])
            return 0;
    }
    return 1;

}

int check4(char* buffer, Node_Head head){
    char s[10] = " <>/=\"", *tok, temp[50];
    node *p = head;

    strcpy(temp, buffer);

    tok = strtok(temp, s);
    tok = strtok(NULL, s);
    tok = strtok(NULL, s);
    
    while(p){
        if(strcmp(p ->id, tok) == 0)
            return 1;
        
        p = p -> next;
    }

    return 0;

}

//Function to generate intermediate file
void generator(FILE *fp, FILE *fq, Node_Head head){
    char buffer1[250], s[10] = " <>/=\"", *tok;

    while(fgets(buffer1, sizeof(buffer1), fp)){
        if(!check1(buffer1))
            continue;

        fputs("\n", fq);

        while(fgets(buffer1, sizeof(buffer1), fp) && !check2(buffer1)){
            if(check3(buffer1))
                continue;
            if(!check4(buffer1, head))
                continue;
            tok = strtok(buffer1, s);
            tok = strtok(NULL, s);
            tok = strtok(NULL, s);
            fputs(tok, fq);
            fputs("\n", fq);
        }

        fputs("\n", fq);
        
    }


}

//Function to search and return node in node list
node *search(char *id, Node_Head head){
    node *p = head;

    while(p){
        if(strcmp(p -> id, id) == 0)
            return p;
        
        p = p -> next;
    }

    return p;
}

//Function to store adjacent data in Edge list
int fill2(FILE *fp, Edge_head *head, Node_Head head1){
    int i = 0;
    char buffer1[50], buffer2[50];
    edge *p = *head, *nn;

    while(fgets(buffer1, sizeof(buffer1), fp) && fgets(buffer2, sizeof(buffer2), fp)){
            if(strcmp(buffer1, "\n") == 0 || strcmp(buffer2, "\n") == 0){
                continue;
            }

            nn = (edge *)malloc(sizeof(edge));

            if(nn){
                nn -> id1 = search(strtok(buffer1, "\n"), head1);
                nn -> id2 = search(strtok(buffer2, "\n"), head1);
                nn -> next = NULL;
                i++;
            }
            else
                return i;
            
            if(!*head){
                *head = nn;
                p = nn;
                continue;
            }

            p -> next = nn;
            p = p -> next;
    }

    rewind(fp);

    fgets(buffer1, sizeof(buffer1), fp);

    while(fgets(buffer1, sizeof(buffer1), fp) && fgets(buffer2, sizeof(buffer2), fp)){
            if(strcmp(buffer1, "\n") == 0 || strcmp(buffer2, "\n") == 0){
                continue;
            }

            nn = (edge *)malloc(sizeof(edge));

            if(nn){
                nn -> id1 = search(strtok(buffer1, "\n"), head1);
                nn -> id2 = search(strtok(buffer2, "\n"), head1);
                nn -> next = NULL;
                i++;
            }
            else
                return i;
            
            if(!*head){
                *head = nn;
                p = nn;
                continue;
            }

            p -> next = nn;
            p = p -> next;
    }

    return i;
}

//Function to remove duplicates in Edge list
int remove_dup(Edge_head *head, int i){
        edge *curr1 = *head;

    while(curr1){
        edge *curr2 = curr1;

        while(curr2 -> next){
            if(curr2 -> next -> id1 == curr1 -> id1 && curr2 -> next -> id2 == curr1 -> id2){
                edge *dup = curr2 -> next;

                curr2 -> next = curr2 -> next -> next;

                free(dup);
                i--;
            }
            else
                curr2 = curr2 -> next;
        }

        curr1 = curr1 -> next;
    }

    return i;
}

//Function to create input file for python script to use
void input_python(FILE *fp, Edge_head head){
    edge *p = head;

    while(p){
        fprintf(fp, "%f %f %f %f\n", p -> id1 -> lat, p -> id1 -> log, p -> id2 -> lat, p -> id2 -> log);
        p = p -> next;
    }

    return;
}

//Function to create final graph file
void final_output(FILE *fp, Node_Head head1, Edge_head head2, int size1, int size2){
    int i;
    char buffer[10];
    FILE *fq;
    node *p = head1;
    edge *q = head2;

    fq = fopen("output.txt", "r");

    if (fq == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(fp, "%d %d\n", size1, size2);

    for(i = 0; p; i++){
        fprintf(fp, "%s Node%d\n", p -> id, i);
        p = p -> next;
    }

    for(i = 0; q; i++){
        fgets(buffer, sizeof(buffer), fq);
        fprintf(fp, "%s %s %s", q -> id1 -> id, q -> id2 -> id, buffer);
        q = q -> next;
    }

    fclose(fq);
    return;
}

int main(){
    Node_Head head1;
    Edge_head head2;
    FILE *fp, *fq;
    int size1, size2;

    node_init(&head1);
    edge_init(&head2);

    fp = fopen("node.txt", "r");

    if (fp == NULL) {
        perror("Failed to open file");
        return 0;
    }

    size1 = fill(fp, &head1);// Store node data in a struct
    fclose(fp);
    size1 = rem_dup1(&head1, size1);

    fp = fopen("ways.txt", "r");

    if (fp == NULL) {
        perror("Failed to open file");
        return 0;
    }

    fq = fopen("inter.txt", "w");

    if (fq == NULL) {
        perror("Failed to open file");
        return 0;
    }
    
    generator(fp, fq, head1); // Create intermediate file

    fclose(fq);
    fclose(fp);

    fp = fopen("inter.txt", "r");

    if (fp == NULL) {
        perror("Failed to open file");
        return 0;
    }

    size2 = fill2(fp, &head2, head1); // Store adajacent node indexes in different struct
    fclose(fp);
    size2 = remove_dup(&head2, size2);
    
    fq = fopen("input.txt", "w");

    if (fq == NULL) {
        perror("Failed to open file");
        return 0;
    }

    input_python(fq, head2); //Create input file for python script

    fclose(fp);

    system("/home/varun/Collage/DSA_Traffic_Project/.venv/bin/python /home/varun/Collage/DSA_Traffic_Project/calulation.py");

    fp = fopen("graph2.txt", "w");

    if (fp == NULL) {
        perror("Failed to open file");
        return 0;
    }

    final_output(fp, head1, head2, size1, size2); //Create final file

    fclose(fp);

    fp = fopen("node_data.txt", "w");

    if (fp == NULL) {
        perror("Failed to open file");
        return 0;
    }

    node *p = head1;

    while(p){
        fprintf(fp, "%s %f %f\n", p -> id, p -> lat, p -> log);
        p = p -> next;
    }
    return 0;
}