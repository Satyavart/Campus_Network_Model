#include<bits/stdc++.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
using namespace std;

int de_key=52009;
string key="1100101100101001";

//String comparison
bool isSmaller(string str1, string str2){ 
    int n1 = str1.length(), n2 = str2.length(); 
    if (n1 < n2) 
    return true; 
    if (n2 < n1) 
    return false; 
    for (int i=0; i<n1; i++) 
    if (str1[i] < str2[i]) 
        return true; 
    else if (str1[i] > str2[i]) 
        return false; 
    return false; 
} 

//string divison by integer
string longDivision(string number, int divisor){ 
    string ans; 
    int idx = 0; 
    int temp = number[idx] - '0'; 
    while (temp < divisor) 
        temp = temp * 10 + (number[++idx] - '0'); 
    while (number.size() > idx) { 
        ans += (temp / divisor) + '0'; 
        temp = (temp % divisor) * 10 + number[++idx] - '0'; 
    } 
    if (ans.length() == 0) 
        return "0"; 
    return ans; 
} 

//string to binary string
string TextToBinaryString(string words){
    string binaryString = "";
    for (char& _char : words) {
        binaryString +=bitset<7>(_char).to_string();
    }
    return binaryString;
}

//string multiplication
string multiply(string num1, string num2){
    int len1 = num1.size(); 
    int len2 = num2.size(); 
    if (len1 == 0 || len2 == 0) 
    return "0"; 
    if(num2=="0") return "1";
    vector<int> result(len1 + len2, 0); 
    int i_n1 = 0;  
    int i_n2 = 0;  
    for (int i=len1-1; i>=0; i--) 
    { 
        int carry = 0; 
        int n1 = num1[i] - '0'; 
        i_n2 = 0;                
        for (int j=len2-1; j>=0; j--){ 
            int n2 = num2[j] - '0';   
            int sum = n1*n2 + result[i_n1 + i_n2] + carry; 
            carry = sum/10; 
            result[i_n1 + i_n2] = sum % 10; 
            i_n2++; 
        }  
        if (carry > 0) 
            result[i_n1 + i_n2] += carry; 
        i_n1++; 
    } 
    int i = result.size() - 1; 
    while (i>=0 && result[i] == 0) 
    i--; 
    if (i == -1) 
    return "0"; 
    string s = ""; 
    while (i >= 0) 
        s += std::to_string(result[i--]); 
    return s; 
} 

//string addition
string findSum(string str1, string str2){ 
    if (str1.length() > str2.length()) 
        swap(str1, str2); 
    string str = "";  
    int n1 = str1.length(), n2 = str2.length(); 
    reverse(str1.begin(), str1.end()); 
    reverse(str2.begin(), str2.end()); 
    int carry = 0; 
    for (int i=0; i<n1; i++) 
    {  
        int sum = ((str1[i]-'0')+(str2[i]-'0')+carry); 
        str.push_back(sum%10 + '0'); 
        carry = sum/10; 
    } 
    for (int i=n1; i<n2; i++) 
    { 
        int sum = ((str2[i]-'0')+carry); 
        str.push_back(sum%10 + '0'); 
        carry = sum/10; 
    } 
    if (carry) 
        str.push_back(carry+'0'); 
    reverse(str.begin(), str.end()); 
    return str; 
}

//string substraction
string findDiff(string str1, string str2) { 
    if (isSmaller(str1, str2)) 
        swap(str1, str2); 
    string str = ""; 
    int n1 = str1.length(), n2 = str2.length();  
    reverse(str1.begin(), str1.end()); 
    reverse(str2.begin(), str2.end()); 
    int carry = 0; 
    for (int i=0; i<n2; i++)     { 
        int sub = ((str1[i]-'0')-(str2[i]-'0')-carry); 
        if (sub < 0)       { 
            sub = sub + 10; 
            carry = 1; 
        } 
        else
            carry = 0; 
        str.push_back(sub + '0'); 
    } 
    for (int i=n2; i<n1; i++)     { 
        int sub = ((str1[i]-'0') - carry); 
        if (sub < 0) { 
            sub = sub + 10; 
            carry = 1; 
        } 
        else
            carry = 0; 
        str.push_back(sub + '0'); 
    } 
    reverse(str.begin(), str.end()); 
    return str; 
} 

int main(){
    int socket_server;
    socket_server =socket(AF_INET,SOCK_STREAM,0);

    //defining address
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8090);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //bind ip and port
    bind(socket_server,(struct sockaddr *)&server_addr,sizeof(struct sockaddr_in));

    //listening
    listen(socket_server,8);

    //accepting connection
    int client;
    client=accept(socket_server,NULL,NULL);

    //program
    char mess[1024];
    recv(client,mess,sizeof(mess),0);
    string s(mess);
    string quo=longDivision(s,de_key);
    string rem=findDiff(multiply(quo,"52009"),s);
    //cout<<rem; 
    int flag=1;
    for(auto i:rem){
        if(int k=(i - '0')>0) flag=0;
    }
    ;
    if(flag==1) {
        char a[50]="\nNo Error Found in Transmission";
        send(client,a,sizeof(a),0);
    }
    else {
        char a[50]="Error Found!!!!!!!";
        send(client,a,sizeof(a),0);
    }
    int close(client);
    return 0;
}