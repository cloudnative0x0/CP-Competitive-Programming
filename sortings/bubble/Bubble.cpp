#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Profiler.hpp>

using namespace std;

template<typename Collection, typename Comparator, typename = typename Collection::size_type>
void bubble_sort(Collection& collection, Comparator comparator) noexcept {
    bool swapped = true;

    for (typename Collection::size_type i = 0; i < collection.size() && swapped; i++) {
        swapped = false;

        for (typename Collection::size_type j = 0; j + 1 < collection.size() - i; j++) {
            if (pkg::Trace()) {
                pkg::Debug("pass=%llu compare j=%llu: a[%llu]=%lld a[%llu]=%lld",
                           (unsigned long long)i, (unsigned long long)j,
                           (unsigned long long)j, (long long)collection[j],
                           (unsigned long long)(j + 1), (long long)collection[j + 1]);
            }

            if (comparator(collection[j], collection[j + 1])) {
                std::swap(collection[j], collection[j + 1]);
                swapped = true;

                if (pkg::Trace()) {
                    pkg::Debug("  swapped -> a[%llu]=%lld a[%llu]=%lld",
                               (unsigned long long)j, (long long)collection[j],
                               (unsigned long long)(j + 1), (long long)collection[j + 1]);
                }
            }
        }

        if (pkg::Trace()) pkg::Debug("pass=%llu finished, swapped=%d", (unsigned long long)i, swapped);
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    pkg::StartProfiling();

    int n;
    cin >> n;
    if (pkg::Trace()) pkg::Debug("n = %d", n);

    vector<long long> arr(n);
    for (auto& x : arr) cin >> x;

    bubble_sort(arr, [](long long a, long long b) { return a > b; });

    ostringstream out;
    for (vector<long long>::size_type i = 0; i < arr.size(); i++) {
        out << arr[i] << (i + 1 < arr.size() ? ' ' : '\n');
    }

    cout << out.str();

    pkg::StopProfiling();

    return 0;
}