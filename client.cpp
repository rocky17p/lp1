#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

int main()
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(6000);

    connect(sd, (sockaddr *)&addr, sizeof(addr));

    char func;
    int a, b;
    
    cout << "Enter function to be performed a(add),s(sub)" << endl;
    cin >> func;
    cout << "enter two numbers space seperated" << endl;
    cin >> a >> b;

    write(sd, &func, sizeof(func));
    write(sd, &a, sizeof(a));
    write(sd, &b, sizeof(b));

    int result;

    read(sd, &result, sizeof(result));

    cout << "the result is " << result << endl;
    close(sd);
    return 0;
}
