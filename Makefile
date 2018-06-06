test:test.c leptjson.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f test
