/*************************************************************************
	> File Name: regex.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue 02 Jun 2015 08:33:07 PM CST
 ************************************************************************/

#include<iostream>
#include<string>
#include<regex>

using namespace std;

//const char* llll = "\\w+\\s+\\([\\w=, \\t]*\\)\\s+at\\s+\\w+\\.c:\\d+";
string pattern("\\w+\\s+\\([\\w=, \\t]*\\)\\s+at\\s+\\w+\\.c:\\d+");
string str("main () at a.c:29");

int main()
{
    regex r(pattern);
    bool match = regex_match(str, r);
    cout<<match<<endl;
    return 0;
}
