#include "../../../library/library/template/template.cpp"

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    int n, t;
    cin >> n >> t;
    vl a(t);
    scan(a);

    vi cnt_vertical(n, 0);
    vi cnt_horizontal(n, 0);
    vi cnt_diagonal(2, 0);
    bool bingo = false;
    rep(i, t) {
        int x = (a[i] - 1) / n;
        int y = (a[i] - 1) % n;

        cnt_vertical[y]++;
        if (cnt_vertical[y] == n) {
            bingo = true;
        }
        cnt_horizontal[x]++;
        if (cnt_horizontal[x] == n) {
            bingo = true;
        }
        if (x == y) {
            cnt_diagonal[0]++;
            if (cnt_diagonal[0] == n) {
                bingo = true;
            }
        }
        if (x + y == n - 1) {
            cnt_diagonal[1]++;
            if (cnt_diagonal[1] == n) {
                bingo = true;
            }
        }

        if (bingo) {
            print(i + 1);
            return 0;
        }
    }

    print(-1);
}