leptjson_test:leptjson_test.c leptjson.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f leptjson_test
