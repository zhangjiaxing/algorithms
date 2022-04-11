all: skiplist ranklist
.PHONY: all

skiplist: skiplist.c
	$(CC) $^ -o $@

ranklist: rank_skiplist.c
	$(CC) $^ -o $@ 

clean:
	rm -rf skiplist ranklist

.PHONY: clean
