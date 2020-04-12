/*
 * 日期：2020年4月12日
 * 作者：fr1gga@outlook.com
 * 功能：尝试《构建之法》中的练习：生成四则运算式
 */

#include <cstdio>
#include <cstdlib>

// 所有数值都可以表示为分数
// type: 0-整数（此时denominator无效）, 1-分数（分子可以是负数，分母必须是正数）
// denominator 有可能被赋值为零，所以使用时必须予以检查！
struct value_t
{
	unsigned char type;
	unsigned int denominator;
	int numerator;
};
struct exp_t					// 表达式
{
	struct exp_t *expLeft;
	struct exp_t *expRight;
	unsigned char opType;		// 0.值  1.加  2.减
	struct value_t value;
};
// 对分数进行约分
void Simplify(struct value_t &val)
{
	if (val.type == 0)			// 对整数无效
		return;
	int a = val.numerator, b = val.denominator, c;
	if (a < 0)
		a = -a;
	while (a > 0)				// 找到最大公因数
	{
		c = b % a;
		b = a;
		a = c;
	}
	val.type = 1;
	if (val.denominator == 1)	// 分母为一则退化为整数
		val.type = 0;
	val.denominator /= b;
	val.numerator /= b;
}
/*
 * 整数范围：[0, randomUpperBound]。
 * 如果生成了真分数，则分母不会大于randomUpperBound；
 * 注意：randomUpperBound <= 1时不生成分数。
 */
value_t GenerateValue(int randomUpperBound)
{
	value_t val;
	val.type = rand() % 2;		// 0-整数  1-（真）分数
	if (val.type == 0 || randomUpperBound <= 1)
	{
		val.denominator = 1;
		val.numerator = rand() % (randomUpperBound + 1);
	}
	else
	{
		randomUpperBound--;				// 分母不能为零，待选区间长度减一但至少是一。
		if (randomUpperBound < 1)
			randomUpperBound = 1;
		val.denominator = rand() % randomUpperBound + 2;// 分母: [2, randomUpperBound]
		val.numerator = rand() % val.denominator + 1;	// 分子: [1,val.denominator-1]
		Simplify(val);									// 保持最简形式
	}
	if (rand() % 2 == 0)
		val.numerator = -(val.numerator);				// 也有可能是负数
	return val;
}
// 构建表达式结构，最高为depth层（别大于5）
void BuildTree(struct exp_t * &exp, unsigned int depth)
{
	exp = (struct exp_t *)malloc(sizeof(struct exp_t));
	if (exp == NULL)
	{
		printf_s("Malloc：失败\n");
		exit(-1);
	}
	exp->expLeft = NULL;
	exp->expRight = NULL;
	if (depth == 0)					// 叶节点
	{
		exp->opType = 0;			// 0-整数  1-分数
		exp->value = GenerateValue(50);
		return;
	}
	exp->opType = rand() % 2 + 1;	// 1.加  2.减
	BuildTree(exp->expLeft, rand() % depth);
	BuildTree(exp->expRight, rand() % depth);
}

struct value_t CalculateAdd(struct value_t left, struct value_t right)
{
	struct value_t answer;
	answer.denominator = left.denominator * right.denominator;
	answer.numerator = left.denominator * right.numerator + left.numerator * right.denominator;
	Simplify(answer);
	return answer;
}
struct value_t CalculateSub(struct value_t left, struct value_t right)
{
	struct value_t answer;
	answer.denominator = left.denominator * right.denominator;
	answer.numerator = left.numerator * right.denominator - left.denominator * right.numerator;
	Simplify(answer);
	return answer;
}

// 用来显示的算符
const char *gOperatorWords[] = { "#", "+", "-", "#" };
// 用来计算表达式结果的函数们（只包含二元运算，将来添加阶乘运算）
struct value_t (*gCalculators[])(struct value_t, struct value_t) = { NULL, CalculateAdd, CalculateSub, NULL };

void Calculate(struct exp_t *exp)
{
	if (exp->opType == 0)				// 叶节点，不需要计算值
		return;
	Calculate(exp->expLeft);
	Calculate(exp->expRight);
	exp->value = gCalculators[exp->opType](exp->expLeft->value, exp->expRight->value);
}
// 显示指定节点的Value域
void ShowValue(struct exp_t *exp)
{
	if (exp == NULL)
		return;
	if (exp->value.numerator < 0)
		printf_s("(");
	printf_s("%d", exp->value.numerator);
	if (exp->value.type == 1)					// 0-整数  1-分数
		printf_s("/%d", exp->value.denominator);
	if (exp->value.numerator < 0)
		printf_s(")");
}
void ShowTree(struct exp_t *exp)
{
	if (exp == NULL)
		return;
	ShowTree(exp->expLeft);
	switch (exp->opType)
	{
	case 0:
		ShowValue(exp);
		break;
	default:
		printf_s(gOperatorWords[exp->opType]);
		break;
	}
	ShowTree(exp->expRight);
}
void DestroyTree(struct exp_t *exp)
{
	if (exp != NULL)
	{
		DestroyTree(exp->expLeft);
		DestroyTree(exp->expRight);
		free(exp);
		exp = NULL;
	}
}
int main()
{
	// 指定数量
	unsigned int expN = -1;
	char *pStr, buff[14];
	scanf_s("%s", buff, 14);
	expN = strtol(buff, &pStr, 10);
	if (pStr == buff)
		return -1;
	// 构建
	srand((unsigned int)pStr);
	while (expN > 0)
	{
		struct exp_t *exp;
		BuildTree(exp, 2);
		Calculate(exp);
		ShowTree(exp);
		printf_s(" = ");
		ShowValue(exp);
		printf_s("\n");
		DestroyTree(exp);
		expN--;
	}
	return 0;
}
