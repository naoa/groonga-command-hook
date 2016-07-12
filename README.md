# Groonga hook management plugin

GroongaのHookの数の確認、削除を行うコマンドを追加します

インデックスのHookのみがカラムに残留し、カラムの削除、テーブルのtruncate、テーブルの削除ができない場合の回避策として使えるかもしれません。

* hook_status

```
hook_status --table Entries --column title
[[0,0.0,0.0],[1,[["Tokens.entries_title","(nil)"]]]]
```
1つ目はカラムが対象とされているインデックス名。２つ目はgrn_proc。
通常、grn_procが設定されることはありません。
インデックスカラムを削除してカラムにHookのみが残留している場合は、1つ目が(nil)になっています。


* hook_delete

```
hook_delete --table Entries --column title --no_target_only true
[[0,0.0,0.0],1]
```

デフォルトでは、ターゲットのインデックスが存在しない不正なhookのみ削除します。
--no_target_only falseにすると１つだけ削除します。

## Install

Install libgroonga-dev.

Build this command.

    % ./configure
    % make
    % sudo make install

## Usage

Register `commands/hook`:

    % groonga DB
    > register commands/hook


    > hook_status table column
    [
      [
        0,
        0.0,
        0.0
      ],
      [
        0
      ]
    ]

## License

Public domain. You can copy and modify this project freely.
