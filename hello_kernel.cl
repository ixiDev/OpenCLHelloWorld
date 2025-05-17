
kernel void say_hello( global char *str){
     const char message[] = "Hello World";

     for (int i = 0; i < sizeof(message); i++) {
          str[i] = message[i];
     }
}
