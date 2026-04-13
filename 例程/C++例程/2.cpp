#include <iostream> //cpp不用h
#include <iomanip>


int main()
{
    // 1. 整型 int short long long
    int age = 25;
    short shortNum = 10;
    long long bigNum = 123456789012345LL;
    
    // 2. 浮点型 float double
    float floatNum = 3.14f;      // 注意f后缀
    double doubleNum = 3.14159265358979;
    
    // 3. 字符与字符串 char std::string
    char singleChar = 'A';       // 单引号是字符
    std::string str = "Hello C++"; // 双引号是字符串，必须用std::string接
    
    // 4. 布尔型 (C语言没有专门的bool，C++自带了)  bool
    bool isStudent = true;
    bool isGraduated = false;

    // 保留两位小数setprecision，使用iomanip
    std::cout << "保留两位小数: " << std::fixed << std::setprecision(2) << doubleNum << std::endl;

    std::cout << "hello" << str <<  isStudent  << std::endl;
    return 0;
}