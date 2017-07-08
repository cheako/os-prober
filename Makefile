CFLAGS := -Os -g -Wall

all: newns plist2strings

newns: newns.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

plist2strings: plist2strings.c
	$(CC) $(CFLAGS) $(LDFLAGS) -lexpat $^ -o $@

clean:
	rm -f newns plist2strings
