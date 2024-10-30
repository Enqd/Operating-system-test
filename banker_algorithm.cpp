#include <iostream>
#include <algorithm>
#include <vector>
#define endl '\n'
using namespace std;

class BankersAlgorithm
{
private:
    int n;                          // 进程数量
    int m;                          // 资源类型数量
    vector<int> available;          // 当前可用的各类资源数目
    vector<vector<int>> max;        // 每个进程对各类资源的最大需求
    vector<vector<int>> allocation; // 每个进程当前已分配到的各类资源数目
    vector<vector<int>> need;       // 每个进程尚需的各类资源数目

public:
    BankersAlgorithm(int num_processes, int num_resources, vector<int> avail, vector<vector<int>> max_matrix, vector<vector<int>> alloc_matrix)
    {
        n = num_processes;
        m = num_resources;
        available = avail;
        max = max_matrix;
        allocation = alloc_matrix;
        need.resize(n, vector<int>(m));
        calculateNeed();
    }

    void calculateNeed()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                need[i][j] = max[i][j] - allocation[i][j];
            }
        }
    }

    bool isSafeState()
    {
        vector<int> work = available;
        vector<bool> finish(n, false);
        vector<int> safeSequence;

        while (safeSequence.size() < n)
        {
            bool found = false;
            for (int i = 0; i < n; i++)
            {
                if (!finish[i])
                {
                    bool canProceed = true;
                    for (int j = 0; j < m; j++)
                    {
                        if (need[i][j] > work[j])
                        {
                            canProceed = false;
                            break;
                        }
                    }
                    if (canProceed)
                    {
                        for (int k = 0; k < m; k++)
                        {
                            work[k] += allocation[i][k];
                        }
                        safeSequence.push_back(i);
                        finish[i] = true;
                        found = true;
                    }
                }
            }
            if (!found)
            {
                return false;
            }
        }
        return true;
    }

    bool requestResources(int process_id, vector<int> request)
    {
        for (int i = 0; i < m; i++)
        {
            if (request[i] > need[process_id][i] || request[i] > available[i])
            {
                return false; // 请求不合法
            }
        }

        // 试探性分配资源
        for (int i = 0; i < m; i++)
        {
            available[i] -= request[i];
            allocation[process_id][i] += request[i];
            need[process_id][i] -= request[i];
        }

        // 检查系统是否处于安全状态
        if (isSafeState())
        {
            return true; // 请求被批准
        }
        else
        {
            // 恢复原状态
            for (int i = 0; i < m; i++)
            {
                available[i] += request[i];
                allocation[process_id][i] -= request[i];
                need[process_id][i] += request[i];
            }
            return false; // 请求被拒绝
        }
    }
};

int main()
{
    int n = 5; // 进程数量
    int m = 3; // 资源类型数量

    vector<int> available = {3, 3, 2}; // 当前可用的各类资源数目

    vector<vector<int>> max = {// 每个进程对各类资源的最大需求
                               {7, 5, 3},
                               {3, 2, 2},
                               {9, 0, 2},
                               {2, 2, 2},
                               {4, 3, 3}};

    vector<vector<int>> allocation = {// 每个进程当前已分配到的各类资源数目
                                      {0, 1, 0},
                                      {2, 0, 0},
                                      {3, 0, 2},
                                      {2, 1, 1},
                                      {0, 0, 2}};

    BankersAlgorithm ba(n, m, available, max, allocation);

    int process_id = 1;              // 请求资源的进程ID
    vector<int> request = {1, 0, 2}; // 请求的资源向量

    if (ba.requestResources(process_id, request))
    {
        cout << "资源请求被批准。系统处于安全状态。" << endl;
    }
    else
    {
        cout << "资源请求被拒绝。系统将进入不安全状态。" << endl;
    }

    return 0;
}