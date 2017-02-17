#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pcs/pcs_mem.h"
#include "arg.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/*找到str第一次出现ch的位置*/
static inline char *findchar(char *str, int ch)
{
	char *p = str;
	while (*p && ((int)((unsigned int)(*p))) != ch) p++;
	return p;
}

/*从程序路径中找到文件名开始的位置，返回开始位置的指针*/
static inline char *filename(char *path)
{
	char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

#pragma region 解析参数

/*解析参数*/
int parse_args(struct args *arg, int argc, char *argv[])
{
	int i, j = 0;
	char *p, *op, *val;
	arg->name = filename(argv[0]);
	for (i = 1; i < argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			arg->optc++;
		}
		else {
			arg->argc++;
		}
	}
	if (arg->argc > 0) arg->argc--;
	if (arg->optc > 0) arg->opts = ht_create((int)((float)arg->optc * HASH_EXTEND_MULTIPLIER), 0, NULL);
	if (arg->argc > 0) arg->argv = (char **)pcs_malloc(arg->argc * sizeof(char *));
	for (i = 1; i < argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			p++;
			if (*p == '-') {
				p++;
				val = findchar(p, '=');
				if (ht_has(arg->opts, p, val - p)) return -1; /*重复指定参数*/
				if (ht_add(arg->opts, p, val - p, (*val) == '=' ? val + 1 : NULL))
					return -1; /*添加到哈希表中失败*/
			}
			else {
				op = p;
				while (*op) {
					if (ht_has(arg->opts, op, 1)) return -1; /*重复指定参数*/
					if (ht_add(arg->opts, op, 1, NULL))
						return -1; /*添加到哈希表中失败*/
					op++;
				}
			}
		}
		else if (!arg->cmd) {
			arg->cmd = p;
		}
		else {
			arg->argv[j++] = p;
		}
	}
	return 0;
}

void free_args(struct args *arg)
{
	if (arg->opts) ht_destroy(arg->opts);
	if (arg->argv) {
		pcs_free(arg->argv);
	}
}

int has_opt(struct args *arg, const char *opt)
{
	if (arg->opts) {
		return ht_has(arg->opts, opt, -1);
	}
	return 0;
}

int has_optEx(struct args *arg, const char *opt, char **pValue)
{
	if (arg->opts) {
		HashtableNode *node = ht_get_node(arg->opts, opt, -1);
		if (node) {
			if (pValue) (*pValue) = node->value;
			return 1;
		}
	}
	if (pValue) (*pValue) = NULL;
	return 0;
}

int has_vopts(struct args *arg, va_list ap)
{
	int co = 0;
	const char *opt;
	if (!arg->opts) return 0;
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	return co;
}

int has_opts(struct args *arg, ...)
{
	int rc = 0;
	va_list ap;
	va_start(ap, arg);
	rc = has_vopts(arg, ap);
	va_end(ap);
	return rc;
}

int test_opts(struct args *arg, ...)
{
	int co = 0;
	const char *opt;
	va_list ap;
	if (!arg->opts) return 0;
	va_start(ap, arg);
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	va_end(ap);
	return arg->opts->count - co;
}

int test_varg(struct args *arg, int minArgc, int maxArgc, va_list ap)
{
	int co = 0;
	const char *opt;
	if (arg->argc < minArgc || arg->argc > maxArgc) return -1;
	if (!arg->opts) return 0;
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	return arg->opts->count - co;
}

int test_arg(struct args *arg, int minArgc, int maxArgc, ...)
{
	int rc = 0;
	va_list ap;
	va_start(ap, maxArgc);
	rc = test_varg(arg, minArgc, maxArgc, ap);
	va_end(ap);
	return rc;
}

int remove_opt(struct args *arg, const char *opt, char **pValue)
{
	if (!arg->opts) return -1;
	int rc = ht_remove(arg->opts, opt, -1, (void **)pValue);
	if (rc == 0) {
		arg->optc--;
	}
	return rc;
}

#pragma endregion
