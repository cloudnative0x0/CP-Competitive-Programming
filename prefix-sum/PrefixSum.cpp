#include <vector>
#include <iostream>
#include <sstream>

#include "Profiler.hpp"


using namespace std;

vector<long long> buildPrefixSum(const vector<long long>& arr) {
    vector<long long> prefix(arr.size() + 1, 0);

    for (size_t i = 0; i < arr.size(); i++) {
        prefix[i + 1] = prefix[i] + arr[i];

        if (pkg::Trace()) {
            pkg::Debug("i=%zu arr[i]=%lld prefix[%zu]=%lld+%lld=%lld",
                       i, arr[i], i + 1, prefix[i], arr[i], prefix[i + 1]);
        }
    }

    return prefix;
}

long long rangeSum(const vector<long long>& prefix, int l, int r) {
    return prefix[r+1] - prefix[l];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    pkg::StartProfiling();

    int n;
    cin >> n;
    if (pkg::Trace()) pkg::Debug("n = %d", n);

    vector<long long> a(n);
    for (auto& x : a) cin >> x;

    vector<long long> prefix = buildPrefixSum(a);

    int q;
    cin >> q;
    if (pkg::Trace()) pkg::Debug("q = %d", q);

    ostringstream out;
    while (q--) {
        int l, r;
        cin >> l >> r;
        long long result = rangeSum(prefix, l - 1, r - 1);
        // if (pkg::Trace()) pkg::Debug("query l=%d r=%d -> result=%lld", l, r, result);
        out << result << '\n';
    }

    cout << out.str();

    pkg::StopProfiling();
    return 0;
}
