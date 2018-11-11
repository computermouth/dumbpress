
## dumbpress

It's small, it's fast, it _stinks_!

##### `dumbpress` is just a quick experiment to make a small and bad compression method. 

It has however resulted in some interesting properties:

1. It's pretty dang fast (because it's barely doing anything)
2. It works best on very very random data (because you can barely do anything with that)

Check out this gnarly unscientific benchmark of compressing a 1.1GB iso:


| Methods    | Time     |  Output      | Size       |
| ---------- | --------:| :----------- | ----------:|
| no comp    |      N/A | testfile     | 1127481344 |
| dumbpress  |  18.084s | testfile.dp  | 1122264411 |
| zip        |  34.259s | testfile.zip | 1116107723 |
| gzip       |  38.362s | testfile.gz  | 1116107584 |
| bzip2      | 153.206s | testfile.bz2 | 1120221876 |

Good news, it's dependency-less! Just `cc main.c -o dumbpress` and you're ready to hardly do anything!
