Multi masternode config
=======================

The multi masternode config allows you to control multiple masternodes from a single wallet. The wallet needs to have a valid collateral output of 1000 coins for each masternode. To use this, place a file named masternode.conf in the data directory of your install:
 * Windows: %APPDATA%\KFX\
 * Mac OS: ~/Library/Application Support/KFX/
 * Unix/Linux: ~/.kfx/

The new masternode.conf format consists of a space seperated text file. Each line consisting of an alias, IP address followed by port, masternode private key, collateral output transaction id, collateral output index, donation address and donation percentage (the latter two are optional and should be in format "address:percentage").

Example:
```
mn1 127.0.0.2:29929 3hcHUL2kyw9HRGGHXz3dM1pp5eSC2cCafGGYzjPNnUL1uJ2YhgP 46b1368c229899f55665ee80f4f197b66df60e42305e2d6ab984cbe38d1b1e7c 0
mn2 127.0.0.3:29929 3hjkmpLZc22zEDmQMDo8vTCdzcyaXCKYR6nt4Cx4tiVdU4445Yg 39324a27fa8c13c7cbf93a011d945e4a7138aa59082e83b82a1f33e572ab7523 0 KDEVVVBT3VA57vxXNVFk1jpkKQrJdQS44i:33
mn3 127.0.0.4:29929 3hYGpHQT5ETFjEoPMPJQ8VDdYaj9H4iwPrfXY6ghp5Pe7iP9A5N 1f60afa78cf40f2db96b1a725398b68584a7f2fac329dd49e84d20c84a6ac8e9 1 KDEVVVBT3VA57vxXNVFk1jpkKQrJdQS44i
```

In the example above:
* the collateral for mn1 consists of transaction 46b1368c229899f55665ee80f4f197b66df60e42305e2d6ab984cbe38d1b1e7c, output index 0 has amount 1000
* masternode 2 will donate 33% of its income
* masternode 3 will donate 100% of its income


The following new RPC commands are supported:
* list-conf: shows the parsed masternode.conf
* start-alias \<alias\>
* stop-alias \<alias\>
* start-many
* stop-many
* outputs: list available collateral output transaction ids and corresponding collateral output indexes

When using the multi masternode setup, it is advised to run the wallet with 'masternode=0' as it is not needed anymore.
