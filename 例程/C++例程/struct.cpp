#include <iostream>
#include <string> // C++专有的字符串头文件

// 1. 定义结构体 (C++中可以直接用struct名字作为类型，不需要加struct关键字)
struct Employee {
    int id;
    std::string name; // C++的字符串，比C语言的char数组好用太多！
    double salary;
};

// 2. 函数接口声明
// 注意这里的 & 符号：表示“引用传参”，相当于C语言的指针，但用起来像普通变量
void printEmployee(const Employee& emp); 
void giveRaise(Employee& emp, double amount);

int main() {
    // 3. 实例化结构体 (不需要写 struct Employee emp1; 直接写 Employee 即可)
    Employee emp1;
    emp1.id = 1001;
    emp1.name = "张三"; // 可以直接用等号赋值字符串，不需要strcpy，也不怕越界！
    emp1.salary = 8000.0;

    Employee emp2 = {1002, "李四", 9500.0}; // 也可以像C语言一样用大括号初始化

    // 4. 调用函数
    std::cout << "=== 初始状态 ===" << std::endl;
    printEmployee(emp1);
    printEmployee(emp2);

    // 5. 调用加薪函数
    std::cout << "\n=== 加薪之后 ===" << std::endl;
    giveRaise(emp1, 1500.0);
    printEmployee(emp1);
    
    return 0;
}

// 函数接口实现
void printEmployee(const Employee& emp) {
    // const 表示在这个函数里不能修改emp的值
    // & 表示引用传参，避免了像C语言那样传递整个结构体的拷贝，提高效率
    std::cout << "员工ID: " << emp.id 
              << ", 姓名: " << emp.name 
              << ", 薪水: " << emp.salary << std::endl;
}

void giveRaise(Employee& emp, double amount) {
    // 因为是引用传参，这里修改emp会直接修改main函数里的原始数据
    emp.salary += amount;
    std::cout << emp.name << " 获得了加薪，金额: " << amount << std::endl;
}
