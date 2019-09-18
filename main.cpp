#include <iostream>

extern "C" {
    int myfunc(int, int);
}

int main() {
  int x,y;
  
  std::cout << "x? > ";
  std::cin >> x;

  std::cout << "y? > ";
  std::cin >> y;

  std::cout << "Call myfunc with x: " << x << " and y: " << y << " => " << myfunc(x, y) << std::endl;

    return 0;
}
