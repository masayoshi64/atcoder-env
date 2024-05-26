#include "../../../library/library/template/template.cpp"

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    int n, m;
    cin >> n >> m;
    vl a(n), b(m);
    scan(a);
    scan(b);

    sort(all(a));
    sort(all(b));

    vl c = a;
    c.insert(c.end(), all(b));

    sort(all(c));
    int ai = 0, bi = 0;
    int before = -1;
    rep(ci, n + m) {
        if (ai < n && c[ci] == a[ai]) {
            if (before == 0) {
                print("Yes");
                return 0;
            }
            ai++;
            before = 0;
        } else {
            bi++;
            before = 1;
        }
    }
    print("No");
}