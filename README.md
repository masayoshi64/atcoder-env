# 個人用競技プログラミング用の環境
## Installation
### [atcoder-cli](https://github.com/Tatamo/atcoder-cli)
```bash
brew install nvm
# 指示に従ってパスを通す
nvm install node
npm install -g npm
npm install -g atcoder-cli
```
### [online-judge-tools](https://github.com/online-judge-tools/oj)
```bash
pip install online-judge-tools
```

## Settings
### atcoder-cli
```bash
acc login # ログイン
acc config-dir # configの場所を確認
# ojと連携するためテストディレクトリの名前を変更
acc config default-test-dirname-format test
# テンプレートの作成
cp -r templates/cpp `acc config-dir`
acc config default-template cpp
```
## 使い方
### アルゴリズム系コンテスト
```
acc new abc100 # abc100用のディレクトリを作成
cd abc100/a # a問題のディレクトリに移動
code main.cpp # main.cppを開く
########################
# コーディング
########################
oj t # 自動テストを実行
acc s # 自動submit
```
### ヒューリスティックコンテスト
1. 以下を実行
```
python -m venv .venv
pip install psytester
psytester config --load atcoder (最新のtester.clgが欲しい時)
```
2. cmd_testerを書き換える（特にインタラクティブな場合）
3. スコアをScore = 100のような形で標準エラー出力に出力
3. 以下のコマンドでまとめて実行（seed=0~9）
```
./run.sh # コンパイル
psytester r -t 0-9
```

#### ハイパラ最適化
1. main.cppに以下を追加
```
if(argc == 2){
    x = stoi(argv[1]); // ハイパラの数，型に応じて変更
}
```
2. optimize.pyを以下のように書き換え
```
x = trial.suggest_int("x", 1, 1000)
args = f"{x}"
```
3. `python optimize.py`で最適化

## 環境構築
### コマンドラインツール
- atcoder-cli
- online-judge-tools
  - 自動サブミット時にコンテスト名の入力を求められないようにpython3.7/site-packages/onlinejudge_command/subcommand/submit.pyの一部をコメントアウトする必要がある
- [psytester](https://github.com/FakePsyho/psytester)
### vscodeの設定
- Debug: Start Debugging（デバッグ開始）のキーボードショートカットを設定しておく
- inputに入力を入れておくことでデバッグが可能 (see .vscode/launch.json)
