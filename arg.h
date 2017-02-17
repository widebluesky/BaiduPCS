#ifndef _PCS_SHELL_ARG_H_
#define _PCS_SHELL_ARG_H_

#include "hashtable.h"

/*参数对象*/
struct args
{
	char		*name;
	char		*cmd;
	Hashtable	*opts;
	int			optc;
	char		**argv;
	int			argc;
};

/*解析参数*/
int parse_args(struct args *arg, int argc, char *argv[]);

void free_args(struct args *arg);

/*判断是否存在 opt 选项
如果存在的话，返回 1, 否则返回 0 */
int has_opt(struct args *arg, const char *opt);

/*判断是否存在 opt 选项，如果存在的话，把其值填入 （*pValue）中，并返回 1，
如果不存在的话，则设置 (*pValue) = NULL, 并返回 0*/
int has_optEx(struct args *arg, const char *opt, char **pValue);

/* See int has_opts(struct args *arg, ...); */
int has_vopts(struct args *arg, va_list ap);

/*
判断是否存在指定的 opt 选项
  ap - 待查询的所有可选项，以 NULL 结束
如果全部不存在则返回 0，否则返回存在的选项数量。
例：
  if (has_vopt(arg, "d", "e", "r", "config", NULL)) {
    printf("Exists one or more options.\n");
  } else {
    printf("No options exists.\n");
  }
*/
int has_opts(struct args *arg, ...);

/*
测试可选项是否匹配。
  arg     -
  ...     - 匹配所有可选项，以NULL结束
如果可选项完全匹配的话，则返回 0。
如果传入的可选项比需要的多，返回 > 0 的值。
如果传入的可选项比需要的少，返回 < 0 的值。
例：
  if (test_opts(arg, "d", "e", "r", "config", NULL)) {
    printf("Wrong Arguments\n");
    return -1;
  }
*/
int test_opts(struct args *arg, ...);

/* See int test_arg(struct args *arg, int minArgc, int maxArgc, ...); */
int test_varg(struct args *arg, int minArgc, int maxArgc, va_list ap);

/*
测试参数是否正确。
arg     -
minArgc - 允许的最少参数
maxArgc - 允许的最多参数
ap      - 支持的所有可选项，以 NULL 结束
如果测试通过则返回 0，否则返回非零值。
例：
  if (test_arg(arg, 0, 1, "d", "e", "r", "config", NULL)) {
    printf("Wrong Arguments\n");
    return -1;
  }
*/
int test_arg(struct args *arg, int minArgc, int maxArgc, ...);

/*
删除一个选项。
删除成功返回 0，否则返回非 0 值。
执行成功，且 pValue 不为 NULL，则把被删除项的值写入 (*pValue)。
*/
int remove_opt(struct args *arg, const char *opt, char **pValue);


#endif
