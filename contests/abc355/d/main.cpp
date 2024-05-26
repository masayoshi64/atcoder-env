#include "../../../library/library/template/template.cpp"

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    int N;
    cin >> N;
    vl l(N), r(N);
    rep(i, N) {
        cin >> l[i] >> r[i];
    }

    vector<tuple<ll, ll, ll>> events;
    rep(i, N) {
        events.emplace_back(l[i], 1, (ll)i);
        events.emplace_back(r[i] + 1, 0, (ll)i);
    }
    sort(all(events));

    ll cnt = 0;
    ll ans = 0;
    for (auto [t, is_l, i] : events) {
        if (is_l) {
            ans += cnt;
            cnt++;
        } else {
            cnt--;
        }
    }
    print(ans);
}