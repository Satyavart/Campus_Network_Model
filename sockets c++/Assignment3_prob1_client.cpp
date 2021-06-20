#include <bits/stdc++.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
using namespace std;

//string comparison
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

//Binary string to decimal string
string binaryToDecimal(string b){
    string deci="",two="2"; int k=0;
    for(int i=b.size()-1;i>=0;i--){
        string temp="1";
        if(k>0){
           temp=multiply(two,"2");
           two=temp;
        }
        string ch(1,b[i]);
        string temp2=multiply(ch,temp);
        string temp3 = findSum(deci,temp2);
        k++;
        deci=temp3;
    }
    return deci;
}

//Decimal string to binary string 
string DecimalToBinary(string b,string r){
    int rem=stoi(r);
    string s;
    while(rem>0){
        if(rem%2==0) s.push_back('0');
        else s.push_back('1');
        rem/=2;
    }
    return findSum(b,s);
}

int main(){
    int net_socket;
    //creating socket
    net_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specifying server address
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(8090);
    server_address.sin_addr.s_addr=INADDR_ANY;

    //connecting
    int status = connect(net_socket,(struct sockaddr*)&server_address,sizeof(struct sockaddr_in));
    if(status == -1) {printf("\n Coneection error \n"); return 0;}

    cout<<"\nConnected \n";
    string s,b;
	printf("\nEnter the message : ");
    cin>>s;
    string key="1100101100101001";
    cout<<"\nKey : "<<key;
    b=TextToBinaryString(s);
    cout<<"\n\nInitial message : "<<b;
    b=b+"000000000000000";
   
    string deci = binaryToDecimal(b);
    //cout<<endl<<deci;

    
     cout<<"\nCode after adding 15 '0' bits : "<<b;
    int de_key=stol(key,nullptr,2);
    //cout<<endl<<"key"<<de_key;
    string quot=longDivision(deci,de_key);
    quot=findSum(quot,"1");
    //cout<<endl<<quot;
    string rem=findDiff(multiply(quot,"52009"),deci);
    //cout<<endl<<rem;
    string newmess=findSum(deci,rem);
    //cout<<endl<<newmess;
    string crc = DecimalToBinary(b,rem);
    cout<<"\nCode after clearing CRC error : "<<crc;
    
    char sen[1024]; int j=0;

    for(auto i:newmess){
        sen[j]=i;
        j++;
    }

    send(net_socket,sen,sizeof(sen),0);

    char a[50];
    recv(net_socket,a,sizeof(a),0);
    cout<<endl<<a<<endl; 

    printf("\nExiting\n");
    int close(net_socket);
    return 0;
}