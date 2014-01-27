#ifndef XMALLOC_H_
#define XMALLOC_H_

extern void *xmalloc(size_t size);

extern void xfree(void *p);

extern char *xstrdup(char *str);



#endif /* XMALLOC_H_ */
