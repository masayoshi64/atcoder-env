#include "../../../library/library/template/template.cpp"

const int max_turn = 10000;
int N;
mat<int> A;

struct Crane {
    int x, y;
    int holding_container = -1;
    bool is_alive = true;
    Crane(int x, int y) : x(x), y(y) {}

    bool is_holding() { return holding_container != -1; }
};

struct Terminal {
    mat<int> grid;
    mat<int> dispatched_containers;
    mat<int> waiting_containers;
    vector<Crane> cranes;

    Terminal() : grid(N, vi(N, -1)), dispatched_containers(N), waiting_containers(N) {
        // 積み込み前のコンテナ（列ごとに出る順番の降順）
        rep(i, N) rrep(j, N) { waiting_containers[i].pb(A[i][j]); }

        // クレーンの初期位置
        rep(i, N) { cranes.pb(Crane(0, i)); }

        // グリッドの左端にコンテナを積む
        rep(i, N) {
            grid[i][0] = waiting_containers[i].back();
            waiting_containers[i].pop_back();
        }
    }

    void step(string actions) {
        rep(crane_id, N) {
            int action = actions[crane_id];
            if (action == 'P') {
                pick(cranes[crane_id]);
            } else if (action == 'Q') {
                release(cranes[crane_id]);
            } else if (action == '.') {
                continue;
            } else if (action == 'B') {
                destroy(cranes[crane_id]);
            } else {
                int dx = 0, dy = 0;
                if (action == 'L') { dx = -1; }
                if (action == 'R') { dx = 1; }
                if (action == 'U') { dy = -1; }
                if (action == 'D') { dy = 1; }
                move(cranes[crane_id], dx, dy);
            }
        }
    }

    void move(Crane &crane, int dx, int dy) {
        int x = crane.x, y = crane.y;
        int nx = crane.x + dx;
        int ny = crane.y + dy;
        cerr << "move " << x << " " << y << " " << nx << " " << ny << endl;
        crane.x = nx;
        crane.y = ny;
        if (crane.is_holding()) {
            if (x == 0) {
                grid[y][x] = waiting_containers[y].back();
                waiting_containers[y].pop_back();
            }
        }
    }

    void pick(Crane &crane) {
        crane.holding_container = grid[crane.y][crane.x];
        grid[crane.y][crane.x] = -1;
    }

    void destroy(Crane &crane) { crane.is_alive = false; }

    void release(Crane &crane) {
        grid[crane.y][crane.x] = crane.holding_container;
        crane.holding_container = -1;
        if (crane.x == N - 1) {
            dispatched_containers[crane.y].pb(grid[crane.y][crane.x]);
            grid[crane.y][crane.x] = -1;
        }
    }

    int calc_score(int num_turn) {
        int M0 = num_turn;
        int M1 = 0;
        int M2 = 0;
        int M3 = N * N;
        rep(i, N) {
            int num_containers = dispatched_containers[i].size();
            M3 -= num_containers;
            rep(j, num_containers) {
                if (dispatched_containers[i][j] < N * i || N * (i + 1) <= dispatched_containers[i][j]) {
                    M2++;
                } else {
                    rep(k, j + 1, num_containers) {
                        if (dispatched_containers[i][k] < N * i || N * (i + 1) <= dispatched_containers[i][k]) continue;
                        if (dispatched_containers[i][j] > dispatched_containers[i][k]) { M1++; }
                    }
                }
            }
        }
        return M0 + M1 * 1e2 + M2 * 1e4 + M3 * 1e6;
    }
};

int main(int argc, char *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);
    cout << setprecision(30) << fixed;
    cerr << setprecision(30) << fixed;

    scan(N);
    A = mat<int>(N, vi(N));
    scan(A);

    vector<string> S = {"PRDDDDRRRQLLLUUPRRRUQ", "B", "PRQB", "PRRRRUUQB", "PRRRRQB"};

    Terminal terminal;
    int num_turn = 0;
    rep(i, N) { chmax(num_turn, SZ(S[i])); }
    rep(turn, num_turn) {
        string actions;
        rep(i, N) {
            if (turn < SZ(S[i])) {
                actions.pb(S[i][turn]);
            } else {
                actions.pb('.');
            }
        }
        terminal.step(actions);
    }

    rep(i, N) { cout << S[i] << endl; }

    cerr << "Score = " << terminal.calc_score(num_turn) << endl;
}