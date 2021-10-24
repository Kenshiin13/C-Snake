![image](https://user-images.githubusercontent.com/63159154/138593175-8b52f546-1c03-4aa3-8a15-0c04be8fc0e7.png)


![image](https://user-images.githubusercontent.com/63159154/138593164-46411e2d-c00e-4795-8642-fc66728fed28.png)


Run: gcc snake.c -o snake

CLI Arguments: snake inputStream outputStream levelFile

To stick to the stdin and stdout streams but change the level file you may use "-" to pass an empty argument. 

snake - - level/2.txt <- Runs the second level file in the stdin and stdout streams.



You may construct your own level files as you please. 

O: Snake head start position

X: Wall

1-9: Food by growth rate

