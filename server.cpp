#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

int add(int a, int b) return { a + b };
int sub(int a, int b) return { a - b };
void handleclient(int clientdescriptor)
{
    char func;
    int a, b;

    read(clientdescriptor, &func, sizeof(func));
    read(clientdescriptor, &a, sizeof(a));
    read(clientdescriptor, &b, sizeof(b));

    int result;
    if (func == 'a')
    {
        result = add(a, b);
    }
    else
    {
        result = sub(a, b);
    }

    write(clientdescriptor, &result, sizeof(result));
    close(clientdescriptor);
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
        int clientdescriptor = accept(sd, null, null);
        thread(handleclient, clientdescriptor).detach();
    }
    close(sd);
    return 0;
}
