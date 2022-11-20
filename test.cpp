#include <list>
#include <iostream>

int main()
{
    std::list<int> l { 1 };
    for (auto it = l.begin(); it != l.end(); it++)
    {
        if (*it == 1)
        {
            std::cout << *it << std::endl;
            it = l.erase(it);
            std::cout << *it << std::endl;
        }
    }
    return 0;
}