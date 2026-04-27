#include <iostream>
#include <vector>
#include <string>
#include "vector.hpp"
#include <concepts>

class Point {
public:
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

int main() {
    Vector<int> nums;

    std::cout << "Initially:\n";
    std::cout << "size = " << nums.size() << "\n";
    std::cout << "capacity = " << nums.capacity() << "\n";
    std::cout << "empty = " << nums.empty() << "\n\n";

    nums.push_back(10);
    nums.push_back(20);
    nums.push_back(30);

    std::cout << "After push_back:\n";
    std::cout << "size = " << nums.size() << "\n";
    std::cout << "capacity = " << nums.capacity() << "\n";
    std::cout << "nums.at(1) = " << nums.at(1) << "\n";
    std::cout << "nums[2] = " << nums[2] << "\n\n";

    int& ref = nums.at(1);
    ref = 999;

    std::cout << "After modifying reference from at():\n";
    std::cout << "nums.at(1) = " << nums.at(1) << "\n\n";

    Vector<int> other;
    other.push_back(10);
    other.push_back(999);
    other.push_back(30);

    std::cout << "nums == other: " << (nums == other) << "\n\n";

    std::cout << "Manual iterator loop:\n";
    for (auto it = nums.begin(); it != nums.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n\n";

    std::cout << "Range-based for loop:\n";
    for (const auto& value : nums) {
        std::cout << value << " ";
    }
    std::cout << "\n\n";

    const Vector<int>& const_nums = nums;

    std::cout << "Const range-based for loop:\n";
    for (const auto& value : const_nums) {
        std::cout << value << " ";
    }
    std::cout << "\n\n";

    Vector<std::string> words;
    words.push_back("generic");
    words.push_back("vector");
    words.push_back("lab");

    std::cout << "String vector:\n";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n\n";

    Vector<Point> points;
    points.push_back(Point(1, 2));
    points.push_back(Point(3, 4));

    Vector<Point> points2;
    points2.push_back(Point(1, 2));
    points2.push_back(Point(3, 4));

    std::cout << "points == points2: " << (points == points2) << "\n";

    return 0;
}