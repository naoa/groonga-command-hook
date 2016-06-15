# Groonga hook management plugin

GroongaのHookの数の確認、削除を行うコマンドを追加します

インデックスのHookのみがカラムに残留し、カラムの削除、テーブルのtruncate、テーブルの削除ができない場合の回避策として使えるかもしれません。

* hook_status
* hook_delete

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
