# 個人用競技プログラミング用の環境
## 使い方
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

## 環境構築
### コマンドラインツール
- atcoder-cli
- online-judge-tools
  - 自動サブミット時にコンテスト名の入力を求められないようにpython3.7/site-packages/onlinejudge_command/subcommand/submit.pyの一部をコメントアウトする必要がある
### vscodeの設定
- Debug: Start Debugging（デバッグ開始）のキーボードショートカットを設定しておく
- inputに入力を入れておくことでデバッグが可能 (see .vscode/launch.json)