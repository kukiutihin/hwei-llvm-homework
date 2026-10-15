### HW1. Graphical app 

#### Build and run 
```bash
cd ./sdl
make app-opt 
./app-opt
```

#### Example

<img width="388" height="220" alt="example" src="https://github.com/user-attachments/assets/0b74b4d6-d277-473a-82a3-2ab7ac0675ab" />


### HW2. Logging pass 

#### Build and run 


```bash
cd ./sdl
make app-log 
./app-log > log.txt
make statistics
```
And you will get `statistics.txt`

#### Example

```
5 most frequent instruction subsequent with window size 5:
	2479338:
		fdiv <- fpext
		fadd <- fpext
		fdiv <- fadd
		call <- fdiv
		fptrunc <- fdiv

	2479338:
		uitofp <- and
		fdiv <- uitofp
		fpext <- fdiv
		fcmp <- fpext
		br <- fcmp

	2479338:
		fcmp <- fpext
		br <- fcmp
		fdiv <- fpext
		fadd <- fpext
		fdiv <- fadd

	2479338:
		br <- fcmp
		fdiv <- fpext
		fadd <- fpext
		fdiv <- fadd
		call <- fdiv

	2479338:
		fadd <- fpext
		fdiv <- fadd
		call <- fdiv
		fptrunc <- fdiv
		fptrunc <- call
```
