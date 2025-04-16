#include <iostream>
#include <cmath>
using namespace std;

// Q1
void Q1()
{
    float f_eps = 1.0f;
    while ((1.0f + f_eps / 2.0f) != 1.0f)
    {
        f_eps /= 2.0f;
    }
    cout << "Float machine epsilon: " << f_eps << endl;

    double d_eps = 1.0;
    while ((1.0 + d_eps / 2.0) != 1.0)
    {
        d_eps /= 2.0;
    }
    cout << "Double machine epsilon: " << d_eps << endl;
}

void Q3()
{

    int a = 200;
    int b = 300;
    int c = 400;
    int d = 500;

    int result = a * b * c * d;

    cout << "Result (int): " << result << endl;

    // 正确做法（使用 long long）
    long long correct_result = 1LL * a * b * c * d;
    cout << "Correct result (long long): " << correct_result << endl;
}

void Q4()
{
    unsigned int counter = 0;
    for (int i = 0; i < 3; ++i) --counter;

    cout << "Counter: " << counter << endl;
}


int main()
{
    int choice;
    cout << "Select Question from 1 to 9," << endl;
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1)
    {
        Q1();
    }
    else if (choice == 2)
    {
        cout << "Q2 is not implemented." << endl;
    }
    else if (choice == 3)
    {
        Q3();
    }
    else if (choice == 4)
    {
        Q4();
    }
    else if (choice == 5)
    {
        cout << "Q5 is not implemented." << endl;
    }
    else if (choice == 6)
    {
        cout << "Q6 is not implemented." << endl;
    }
    else if (choice == 7)
    {
        cout << "Q7 is not implemented." << endl;
    }
    else if (choice == 8)
    {
        cout << "Q8 is not implemented." << endl;
    }
    else if (choice == 9)
    {
        cout << "Q9 is not implemented." << endl;
    }
    else
    {
        cout << "Invalid choice." << endl;
    }

    return 0;
}