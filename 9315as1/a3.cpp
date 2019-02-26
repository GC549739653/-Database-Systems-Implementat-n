#include <stdio.h>

 
// 升序数组src
void sort(int *src, int len)
{
    int tem;
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
            if (src[j] > src[j+1])
            {
                tem = src[j];
                src[j] = src[j+1];
                src[j+1] = tem;
            }
    }
}
 
int main()
{
    int num[10] = {0, 2, 5, 1, 1, 5, 4, 9, 2, 1}, d;
    int i = 0, cnt;
 
    sort(num, 10);  // 排序
    d = num[0];
    for (i = 1; i < 10; i++) // 遍历比较
    {
        if (num[i] != d)
        {
            printf("%d ", d);
            d = num[i];
        }
    }
    if (num[9] == d)
        printf("%d\n", d);
    return 0;
}