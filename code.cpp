
/*

a: 2
b: 5
c: 4
d: 4
h: 6
m: 4
p: 8
q: 10



(a, 1)
(b, 2)
(c: 2)
(d: 2)
(h, 3)
(m, 2)
(p, 4)
(q, 5)
(b, 1)

(q, 5)
(p, 4)
(m, 2)
(h, 3)
(d: 2)
(c: 2)
(b, 2)
(a, 1)

a b a d c b b m h q p

*/

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using freq_t = array<uint32_t, 26>;
using pair_t = pair<char, uint32_t>;

struct node_t {
    freq_t freq = {};
    bool pend_upd = false;
};

vector<node_t> seg_tree;

bool can_be_made_pal(const freq_t& freq) {
    bool has_odd = false;
    for (const auto v : freq) {
        if (v % 2 != 0) {
            if (has_odd == false) {
                has_odd = true;
            }
            else {
                return false;
            }
        }
    }
    return true;
}

void build_tree(uint32_t cur_idx, const string& str, uint32_t l, uint32_t r) {
    if (seg_tree.size() <= cur_idx) {
        seg_tree.resize(cur_idx + 1);
    }
    if (l == r) {
        auto& cur_node = seg_tree[cur_idx];
        cur_node.freq[str[l] - 'a']++;
        return;
    }
    uint32_t m = (l + r) / 2;
    uint32_t l_idx = cur_idx * 2 + 1;
    uint32_t r_idx = cur_idx * 2 + 2;
    build_tree(l_idx, str, l, m);
    build_tree(r_idx, str, m + 1, r);
    auto& cur_node = seg_tree[cur_idx];
    for (uint8_t i = 0; i < tuple_size_v<freq_t>; i++) {
        cur_node.freq[i] = seg_tree[l_idx].freq[i] + seg_tree[r_idx].freq[i];
    }
    return;
}

void get_lr_parts(const freq_t& freq, vector<pair_t>& left, vector<pair_t>& right) {
    int8_t odd_idx = -1;
    left.clear();
    right.clear();
    for (uint8_t i = 0; i < freq.size(); ++i) {
        if (freq[i] % 2 != 0) {
            assert(odd_idx == -1);
            odd_idx = (int8_t)i;
        }
        uint32_t count = freq[i] / 2;
        if (count != 0) {
            left.push_back({'a' + i, count});
            right.push_back({'a' + i, count});
        }
    }
    if (odd_idx != -1) {
        left.push_back({'a' + odd_idx, 1});
    }
    reverse(left.begin(), left.end());
}

void get_half(const freq_t& freq, vector<pair_t>& left, vector<pair_t>& right, uint8_t wanted_part) {
    uint64_t i = 0, j = 25;
    for (uint64_t* cur_idx = &i; i != j || freq[i] != 0;) {
        if (freq[*cur_idx] != 0) {

        }
    }
    return;
}

char update_freq(vector<pair_t>& before_mid, vector<pair_t>& after_mid, node_t& cur_node, uint32_t nl, uint32_t nr) {
    cur_node.freq = {};
    char last_char = 0;
    for (uint32_t i = 0, e = nr - nl + 1; i < e;) {
        vector<pair_t>& cur_vec = (!before_mid.empty()) ? before_mid : after_mid;
        pair_t cur_pair = cur_vec.back();
        uint32_t used = min(cur_pair.second, e - i);
        cur_node.freq[cur_pair.first - 'a'] += used;
        i += used;
        cur_pair.second -= used;
        if (cur_pair.second == 0) {
            cur_vec.pop_back();
        }
    }
    return last_char;
}

freq_t get_freq_sum (uint32_t cur_idx, uint32_t nl, uint32_t nr, uint32_t ql, uint32_t qr) {
    auto& cur_node = seg_tree[cur_idx];
    if (cur_node.pend_upd == true) {
        auto& prev_node = seg_tree[(cur_idx - 1) / 2];
        vector<pair_t> prev_before;
        vector<pair_t> prev_after;
        get_lr_parts(prev_node.freq, prev_before, prev_after);
        update_freq(prev_before, prev_after, cur_node, nl, nr);
        if (nl != nr) {
            seg_tree[cur_idx * 2 + 1].pend_upd = true;
            seg_tree[cur_idx * 2 + 2].pend_upd = true;
        }
        cur_node.pend_upd = false;
    }
    if (qr < nl || nr < ql) {
        return {};
    }
    if (ql <= nl && nr <= qr) {
        return cur_node.freq;
    }
    uint32_t m = (nl + nr) / 2;
    uint32_t l_idx = cur_idx * 2 + 1;
    uint32_t r_idx = cur_idx * 2 + 2;
    freq_t l_freq = get_freq_sum(l_idx, nl, m, ql, qr);
    freq_t r_freq = get_freq_sum(r_idx, m + 1, nr, ql, qr);
    if (l_freq == freq_t{}) {
        return r_freq;
    }
    if (r_freq == freq_t{}) {
        return l_freq;
    }
    freq_t comb_freq = {};
    for (uint8_t i = 0; i < tuple_size_v<freq_t>; i++) {
        cur_node.freq[i] = seg_tree[l_idx].freq[i] + seg_tree[r_idx].freq[i];
        comb_freq[i] = l_freq[i] + r_freq[i];
    }
    return comb_freq;
}

void update_tree(uint32_t cur_idx, vector<pair_t>& before_mid, vector<pair_t>& after_mid, uint32_t nl, uint32_t nr, uint32_t ql, uint32_t qr) {
    auto& cur_node = seg_tree[cur_idx];
    if (qr < nl || nr < ql) {
        return;
    }
    uint32_t l_idx = cur_idx * 2 + 1;
    uint32_t r_idx = cur_idx * 2 + 2;
    if (ql <= nl && nr <= qr) {
        update_freq(before_mid, after_mid, cur_node, nl, nr);
        if (nl != nr) {
            seg_tree[l_idx].pend_upd = true;
            seg_tree[r_idx].pend_upd = true;
        }
        return;
    }
    uint32_t m = (nl + nr) / 2;
    update_tree(l_idx, before_mid, after_mid, nl, m, ql, qr);
    update_tree(r_idx, before_mid, after_mid, m + 1, nr, ql, qr);
    for (uint32_t i = 0; i < tuple_size_v<freq_t>; i++) {
        cur_node.freq[i] = seg_tree[l_idx].freq[i] + seg_tree[r_idx].freq[i];
    }
    return;
}

void get_final_str(string& final_str, uint32_t cur_idx, uint32_t nl, uint32_t nr, uint32_t& target) {
    if (target < nl || nr < target) {
        return;
    }
    auto cur_node = seg_tree[cur_idx];
    if (cur_node.pend_upd == true) {
        vector<pair_t> before_mid;
        vector<pair_t> after_mid;
        auto& prev_node = seg_tree[(cur_idx - 1) / 2];
        get_lr_parts(prev_node.freq, before_mid, after_mid);
        if (cur_idx % 2 == 1) {
            final_str += update_freq(before_mid, after_mid, cur_node, nl, nr);
        }
        else {
            final_str += update_freq(after_mid, before_mid, cur_node, nl, nr);
        }
        target++;
        cur_node.pend_upd = false;
        if (nl != nr) {
            seg_tree[cur_idx * 2 + 1].pend_upd = true;
            seg_tree[cur_idx * 2 + 2].pend_upd = true;
        }
        return;
    }

    if (nl == nr) {
        for (uint32_t i = 0; i < tuple_size_v<freq_t>; i++) {
            if (cur_node.freq[i] != 0) {
                final_str += ('a' + i);
                target++;
                return;
            }
        }
    }
    uint32_t m = (nl + nr) / 2;
    while (nl <= target && target <= nr) {
        get_final_str(final_str, cur_idx * 2 + 1, nl, m, target);
        get_final_str(final_str, cur_idx * 2 + 2, m + 1, nr, target);
    }
    return;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    uint32_t N, M;
    cin >> N >> M;
    string str;
    cin >> str;
    build_tree(0, str, 0, N - 1);
    for (size_t i = 0; i < M; i++) {
        uint32_t l, r;
        cin >> l >> r;
        freq_t freq = get_freq_sum(0, 0, N - 1, l - 1, r - 1);
        if (can_be_made_pal(freq)) {
            vector<pair_t> before_mid;
            vector<pair_t> after_mid;
            get_lr_parts(freq, before_mid, after_mid);
            update_tree(0, before_mid, after_mid, 0, N - 1, l - 1, r - 1);
        }
    }
    string final_str = "";
    uint32_t zero = 0;
    get_final_str(final_str, 0, 0, N - 1, zero);
    cout << final_str;
    return 0;
}
