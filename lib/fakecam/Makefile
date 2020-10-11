JNI_DIR = /usr/lib/jvm/java-8-openjdk/include/
JNI_MD_DIR = /usr/lib/jvm/java-8-openjdk/include/linux

all: libfakecam.so

install: libfakecam.so
	sudo cp -f libfakecam.so /usr/lib/libfakecam.so

clean:
	rm -f *.so

libfakecam.so: FakeCam.c FakeCam.h
	gcc -I$(JNI_DIR) -I$(JNI_MD_DIR) -lrt -lpthread -fPIC -shared -o libfakecam.so FakeCam.c

