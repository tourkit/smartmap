#pragma once

template <typename T>
static void ADD_UNIQUE( std::vector<T>& list, T n) {

    for (auto x : list) if (x == n) return;

    list.push_back(n);

}
