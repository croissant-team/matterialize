JNI_DIR = $(JAVA_HOME)/include/
JNI_MD_DIR = $(JAVA_HOME)/include/linux

all: libfakecam.so

install: libfakecam.so
	sudo cp -f libfakecam.so /usr/lib/libfakecam.so

clean:
	rm -f *.so

libfakecam.so: FakeCam.c FakeCam.h
	gcc -I$(JNI_DIR) -I$(JNI_MD_DIR) -lrt -lpthread -fPIC -shared -o libfakecam.so FakeCam.c

