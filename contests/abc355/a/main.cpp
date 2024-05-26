#include "../../../library/library/template/template.cpp"

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    int a, b;
    cin >> a >> b;
    if (a == b) {
        print(-1);
    } else {
        print(6 - a - b);
    }
}