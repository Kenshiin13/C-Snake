Run: gcc snake.c -o snake

CLI Arguments: snake inputStream outputStream levelFile

To stick to the stdin and stdout streams but change the level file you may use "-" to pass an empty argument. 

snake - - level/2.txt <- Runs the second level file in the stdin and stdout streams.



You may construct your own level files as you please. 

O: Snake head start position

X: Wall

1-9: Food by growth rate

