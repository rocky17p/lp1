#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include<thread>
using namespace std;

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

void handleclient(int client)
{
    char func;
    int a, b;

    read(client, &func, sizeof(func));
    read(client, &a, sizeof(a));
    read(client, &b, sizeof(b));

    int result;
    if (func == 'a')
    {
        result = add(a, b);
    }
    else
    {
        result = sub(a, b);
    }

    write(client, &result, sizeof(result));
    close(client);
}
int main()
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(6000);

    bind(sd, (sockaddr *)&addr, sizeof(addr));
    listen(sd, 5);

    while (true)
    {
        int client = accept(sd, NULL, NULL);
        thread(handleclient, client).detach();
    }
    close(sd);
}
