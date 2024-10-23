#include <iostream>
#include <algorithm>
#include <string>
#define endl '\n'
using namespace std;

struct PCB
{                     // 进程状态定义
    int in_id;        // 内部ID
    string out_id;    // 外部ID
    int run_time;     // 要求服务时间
    int state;        // 标志进程状态，0为不在内存，1为在内存，2为阻塞态
    struct PCB *next; // 队列指针
};

struct PCB *ready_head = new PCB();
struct PCB *ready_tail = ready_head;
struct PCB *blocked_head = new PCB();
struct PCB *blocked_tail = blocked_head;

int time_limt = 2; // 时间片大小
int id;

// 带头结点的链表-->尾插
void InsertTail(struct PCB **head, struct PCB **tail, struct PCB *L)
{
    if (L == NULL)
    {
        cout << "待插入节点为空" << endl;
        return;
    }

    L->next = NULL;

    if ((*head)->next == NULL) // 如果链表为空
    {
        (*head)->next = L;
        *tail = L;
    }
    else
    {
        (*tail)->next = L;
        *tail = L;
    }
}

// 带头结点的链表-->头删
void DeletHead(struct PCB **head, struct PCB **tail)
{
    if ((*head)->next == NULL)
        return;

    struct PCB *toDelete = (*head)->next; // 指向要删除的节点
    (*head)->next = toDelete->next;       // 更新头节点的next指向第二个节点

    if ((*tail) == toDelete) // 如果删除的是尾节点
    {                  
        *tail = *head; // 更新尾指针为头指针
    }

    delete (toDelete); // 释放被删除节点的内存
}

// 将头结点移动到链表尾
void moveTotail(struct PCB **head, struct PCB **tail)
{
    if((*head)->next == NULL) //只有头结点，没有进程
        return;

    if((*head)->next == *tail && *tail == ready_tail) // 只有一个进程且是就绪队列到就绪队列
        return;

    struct PCB *toMove = (*head)->next; // 要移动的节点
    (*head)->next = toMove->next;       // 更新头结点指向下一个节点

    (*tail)->next = toMove; // 将该节点移动到链表尾
    *tail = toMove;
    (*tail)->next = NULL;
}

// 打印链表
void Print(struct PCB *L)
{
    while (L)
    {
        cout << L->out_id << "-->";
        L = L->next;
    }
    cout << "NULL" << endl;
}

// 结束进程
void over(int id)
{
    struct PCB *cur = NULL;
    // 查看就绪队列
    cur = ready_head;
    while (cur->next)
    {
        if (cur->next->in_id == id) // 删除该节点
        {
            struct PCB *tmp = cur->next;
            cur->next = tmp->next;
            if(tmp == ready_tail)
            {
                ready_tail = ready_head;
            }
            delete (tmp); // 释放内存
            return;
        }
        cur = cur->next;
    }
    // 查看阻塞队列
    cur = blocked_head;
    while (cur->next)
    {
        if (cur->next->in_id == id) // 删除该节点
        {
            struct PCB *tmp = cur->next;
            cur->next = tmp->next;
            if(tmp == blocked_tail)
            {
                blocked_tail = blocked_head;
            }
            delete (tmp); // 释放内存
            return;
        }
        cur = cur->next;
    }
    
    cout << "未找到该进程" << endl;
}

// 创建进程
void create()
{
    struct PCB *p = new PCB();
    cout << "输入内部ID:";
    cin >> p->in_id;
    cout << "输入外部ID:";
    cin >> p->out_id;
    cout << "输入要求服务时间:";
    cin >> p->run_time;
    p->state = 1;                            // 在内存
    InsertTail(&ready_head, &ready_tail, p); // 插入就绪队列尾
}

// 时间片到
void time_over()
{
    if(ready_head->next == NULL)
    {
        cout << "就绪队列为空，没有执行进程" << endl;
        return;
    }
    
    struct PCB *running = ready_head->next;
    if (running->run_time > time_limt) // 要求运行时间大于时间片
    {
        running->run_time -= time_limt;
        moveTotail(&ready_head, &ready_tail);
    }
    else
    {
        // 进程运行结束
        if(running->next) // 如果下一个进程存在，则减去剩余时间片
            running->next->run_time -= time_limt - running->run_time;
        over(running->in_id);
    }
}

// 进程阻塞
void blocked()
{
    if (ready_head->next == NULL)
    {
        cout << "就绪队列为空，无法阻塞" << endl;
        return;
    }

    ready_head->next->state = 2; // 改为阻塞态
    moveTotail(&ready_head, &blocked_tail);
}

// 显示执行进程，就绪队列，阻塞队列
void display()
{
    // 显示执行进程
    struct PCB *running = ready_head->next;
    if (running == NULL)
        cout << "没有执行进程" << endl;
    else
    {
        cout << "----------------------------------------" << endl;
        cout << "执行进程:" << endl;
        cout << "内部ID:" << running->in_id << endl;
        cout << "外部ID:" << running->out_id << endl;
        cout << "进程状态:" << running->state << endl;
        cout << "所需服务时间:" << running->run_time << endl;
        cout << "--------------------------------------- " << endl;
    }

    // 打印就绪队列
    struct PCB *cur1 = ready_head->next;
    cout << "就绪队列：";
    Print(cur1);
    // 打印阻塞队列
    struct PCB *cur2 = blocked_head->next;
    cout << "阻塞队列：";
    Print(cur2);
    cout << "--------------------------------------- " << endl;
}

// 唤醒进程
void weak()
{
    if (blocked_head->next == NULL)
    {
        cout << "阻塞队列为空，无法唤醒" << endl;
        return;
    }

    blocked_head->next->state = 1; // 改为就绪态
    moveTotail(&blocked_head, &ready_tail);
}

// 主函数
signed main()
{
    int num;
    while (1)
    {
        cout << "******************************************" << endl;
        cout << "*              进程演示系统              *" << endl;
        cout << "******************************************" << endl;
        cout << "*      1.创建进程        2.时间片到      *" << endl;
        cout << "*      3.进程阻塞        4.唤醒进程      *" << endl;
        cout << "*      5.结束进程        6.结束程序      *" << endl;
        cout << "******************************************" << endl;
        cout << "请选择(1~6): "; // 提示用户选择
        cin >> num;
        switch (num)
        {
        case 1:
            create();
            display();
            break;
        case 2:
            time_over();
            display();
            break;
        case 3:
            blocked();
            display();
            break;
        case 4:
            weak();
            display();
            break;
        case 5:
            cout << "输入要结束进程的内部ID:";
            cin >> id;
            over(id);
            display();
            break;
        case 6:
            cout << "已退出系统" << endl;
            exit(0);
        default:
            cout << "输入错误" << endl;
        }
    }
    return 0;
}
