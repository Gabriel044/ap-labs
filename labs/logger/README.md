Lab - Logger Library
====================
Building & Running the Logger:
1. Compile the logger.c program: gcc -c logger.c -o logger.o 
2. Declare the following functions in the header of the code you are using:
    - int infof(const char *format, ...);
    - int warnf(const char *format, ...);
    - int errorf(const char *format, ...);
    - int panicf(const char *format, ...);
3. Use the respective functions throughout your code.
4. Compile your code with the following format: gcc -c ${APP_NAME}.c -o ${APP_NAME}.o
5. Finally merge both compilations in one with the following command: gcc ${APP_NAME}.o logger.o -o main.o
6. Run main: ./main.o

Implement a Logger library in C that will be used for printing the following types of messages.
- `INFO` General information.
- `WARN` Warnings.
- `ERROR` Errors.
- `PANIC` Panics. Non recoverable issues with core dump.

This `logger` library will be used instead of `printf` function calls in all new labs and challenges that we'll be doing in the course.
Below you can see the prototype for each function.

```
int infof(const char *format, ...)
int warnf(const char *format, ...)
int errorf(const char *format, ...)
int panicf(const char *format, ...)
```

General instructions
--------------------
- Use the `logger.c` file for your implementation.
- Update `README.md` with the proper steps for building and running your code.
- Use the `testLogger.c` file for testing all your logger functions.
- Use different font color for each of the log types. For colors references go to: https://www.linuxjournal.com/article/8603
- Don't forget to handle errors properly.
- Coding best practices implementation will be also considered.

How to submit your work
=======================
```
make submit
```
More details at: [Classify API](../../classify.md)
