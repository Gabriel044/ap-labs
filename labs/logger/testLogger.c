int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);


int main() {
    infof("This is an Info Log\n");
    warnf("This is a Warning Log\n");
    errorf("This is an error Log\n");
    panicf("This is a Panic Log\n");
    return 0;
}
