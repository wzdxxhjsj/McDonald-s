#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START 25200
#define END 79200
#define FINISH 1
#define FAIL 0

int W2, W1;
int M, N;
int n;

 typedef struct {
    char name[55] ;
    int needtime;
    int time;
    int cap;
    int left;
}Food ;

typedef struct 
{
    char name[55] ;
    int foods[5] ;
    int  num;
} Set;

 typedef struct 
{
    int time;
    int end;
    int num;
    int undonum;
    int food[5] ;
    int foodstate[5];
    int orderstate;
}Order;

struct Doing
{
    Order* data;
    struct Doing* next;
};
typedef struct Doing* DoingPtr;

DoingPtr head;
Set set[105];
Order order1[54111];
Food food1[105];

int change1(char* tmp1)
{
    for (int i = 0; i < N; i++) 
    {
        if (strcmp(food1[i].name, tmp1) == 0)
            return i;
    }
    return -1;
}

int change2(char* name)
{
    for (int i = 0; i < M; i++) {
        if (strcmp(set[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int timechange1(char* t)
{
    int h, m, s;
    h = (t[0] - '0') * 10 + t[1] - '0';
    m = (t[3] - '0') * 10 + t[4] - '0';
    s = (t[6] - '0') * 10 + t[7] - '0';
    return h * 3600 + m * 60 + s;
}

void timechange2(int sec, char* time)
{
    int h, m, s;
    h = sec / 3600;
    m = (sec % 3600) / 60;
    s = sec % 60;
    sprintf(time, "%02d:%02d:%02d", h, m, s);
}

void find(Order* order, char* name)
{
    int a = change1(name);
    if (a > -1)
    {
        order->undonum = order->num = 1;
        order->food[0] = a;
    }
    else
    {
        a = change2(name);
        order->undonum = order->num = set[a].num;

        for (int i = 0; i < set[a].num; i++)
            order->food[i] = set[a].foods[i];
    }
}

void readmenu()
{
    FILE* fPtr = fopen("dict.dic", "r");
    if (fPtr == NULL) 
        printf("Fail\n");
    else
    {
        fscanf(fPtr, "%d %d", &N, &M);
        for (int m = 0; m < N; m++)
        {
            fscanf(fPtr, "%s", food1[m].name);
            food1[m].left = 0;
            food1[m].time = 0;
        }
                     
        for (int p = 0; p < N; p++)
            fscanf(fPtr, "%d", &food1[p].needtime);

        for (int q = 0; q < N; q++) 
            fscanf(fPtr, "%d", &food1[q].cap);

        fscanf(fPtr, "%d%d", &W1, &W2);
        fgetc(fPtr);
        for (int i = 0; i < M; i++)
        {
            char tmp[1000];
            char tmp1[55];
            fgets(tmp, 1000, fPtr);            
            if (tmp[strlen(tmp) - 1] == '\n')
                tmp[strlen(tmp) - 1] = '\0';
            int num=0;//食物数量
            for (int j = 0; tmp[j] != '\0'; j++)
            {
                if (tmp[j] == ' ')
                    num++;
            }
            set[i].num = num;
            int k;
            for (k = 0; tmp[k] != ' '; k++)
                set[i].name[k] = tmp[k];
            set[i].name[k] = '\0';
            for (int b = 0; b < num; b++)
            {
                for (int a = 0;; a++) 
                {
                    ++k;
                    if (tmp[k] == ' ' || tmp[k] == '\0') 
                    {
                        tmp1[a] = '\0';
                        break;
                    }
                    tmp1[a] = tmp[k];
                }                
                set[i].foods[b] = change1(tmp1);
            }  
            
        }
    }
    fclose(fPtr);
}

void readinput()
{
    scanf("%d", &n);
    getchar();
    for (int a = 0; a < n; a++) 
    {
        for (int b = 0; b < 5; b++)
            order1[a].foodstate[b] = 0;
        order1[a].orderstate = 0;
        char time[10];
        scanf("%s", time);
        order1[a].time = timechange1(time);
        char name[55];
        scanf("%s", name);
        find(order1+a, name);
    }
}


int doingnum()
{
    DoingPtr curr = head->next;
    int len = 0;
    while (curr)
    {
        len++;
        curr = curr->next;
    }   
    return len;
}


void inDoing(Order* order)
{
    DoingPtr curr = (DoingPtr)malloc(sizeof(struct Doing));
    DoingPtr c = head->next;
    DoingPtr p=head;

    curr->data = order;
    curr->next = NULL;
    while (c) 
    {
        p = p->next;
        c = c->next;
    }
    p->next = curr;
}

void update()
{
    for (int f = 0; f < N; f++)
    {
        if (food1[f].left < food1[f].cap)
        {
            if (food1[f].time == 0)
                food1[f].time = food1[f].needtime;
            food1[f].time--;
            if (food1[f].time == 0)
                food1[f].left++;
        }
    }
}

void dealdoing(int t)
{
    DoingPtr pre=head;
    DoingPtr curr=head->next;
    while (curr) 
    {        
        for (int i = 0; i < curr->data->num; i++)
        {
            if (curr->data->foodstate[i] == 1)
                continue;
            if (food1[curr->data->food[i]].left == 0)
                continue;
            curr->data->foodstate[i] = 1;
            curr->data->undonum -= 1;
            food1[curr->data->food[i]].left -= 1;
        }
        if (curr->data->undonum == 0) {            
            curr->data->orderstate = FINISH;
            curr->data->end = t;
            
            pre->next = curr->next;
            free(curr);
            curr = pre->next;     
            continue;
            
        }
        pre = pre->next;
        curr=curr->next;
    }    
}


void dealorder()
{
    int time = START;
    int isopen = 1;//系统打开
    int ord = 0;//订单编号
    head = (DoingPtr)malloc(sizeof(struct Doing));
    head->next = NULL; 

    if (time == order1[ord].time) 
        inDoing(&order1[ord++]);
          
    while(1){       
        time++;        
        int l1 = doingnum();
        update();
        if(l1!=0)
            dealdoing(time);

        if (time == order1[ord].time)
        {
            if (isopen) 
            {          
                inDoing(&order1[ord]);   
                dealdoing(time);
            }                
            else
                order1[ord].orderstate = FAIL;
            ord++;
        }

        if (ord == n && doingnum() == 0)
            break;
        l1 = doingnum();
        if (time >= END)
            isopen = 0;

        if (isopen&& l1> W1 )
            isopen = 0;
        else if (isopen == 0 && l1 < W2 )
            isopen = 1;
        
    }
}

void output()
 {
    char t[10];
    for (int i = 0; i < n; i++) 
    {
        if (order1[i].orderstate == FINISH) 
        {
            timechange2(order1[i].end, t);
            puts(t);
        }
        else
            puts("Fail");
    }
}

int main() 
{
    readmenu();
    readinput();
    dealorder();
    output();
    return 0;
}

