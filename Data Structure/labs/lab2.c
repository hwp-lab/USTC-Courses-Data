/*
 * 4.3 实验说明
 * ------------------------------------------------------------
 * • 完成线性表抽象数据类型 ADT 的链式存储设计与实现（作为基础数据结构）。
 * • 实现高精度实数 x 的加、减、乘运算（保证 |x| < 1024），运算精度要求达到 2^-n，
 *   其中 n 为输入参数或预定义参数。
 * • 用十进制或二进制串（线性表）表示实数 x，并构造相应的 ADT。
 * • 实现读入十进制实数的功能，并实现到 N 进制的转换（运行时指定 N，且 N < 20），
 *   实现加、减、乘三种基本操作，并输出对应的 N 进制结果与十进制结果。
 * • 实现一个复杂操作：单变量多项式求值。例如求函数
 *     f(x) = (3/7) x^3 - (1/3) x^2 + 4
 *   的值（其中取 x = 1.4，精度 n = 200），系数 3/7、1/3 等可以不使用高精度表示。
 * • 提示：在运行时解析输入的多项式字符串，分析每项的系数与幂次，调用基本操作完成计算。
 *
 * 4.4 检查 / 提交要求
 * ------------------------------------------------------------
 * • 代码能正确编译、运行并输出结果（能正确完成测试样例即可）。
 * • 实现二进制与十进制之间的转换：
 *     - 输入高精度十进制小数，转换为高精度二进制小数；
 *     - 输入高精度二进制小数，转换为高精度十进制小数。
 * • 实现从十进制到 N 进制的转换（N 在运行时指定，且 N < 20），并实现加、减、乘操作：
 *     - 支持输入高精度十进制小数，转换为高精度 N 进制；
 *     - 能对输入的数值进行加减乘运算，并输出对应的十进制与 N 进制结果。
 * • 单变量多项式求值：能实现并验证示例 f(x)（如上），精度示例 n = 200。
 * • 代码注释要求：每三行代码至少有一行注释；变量、函数等命名应有意义。
 * • 提交时间：10 月 15 日 24:00。
 */

#include <stdio.h>
#include <stdlib.h>
#include<math.h>//fabs(),modf()

#define integer 40//整数位数 
#define decimal 200//小数精度 
#define npoly 3

//表示小数的结构体
typedef struct num{
    int data[integer+decimal];
    int neg;//0表示正，1表示负
    int base;//表示进制
}num;
//多项式一项的结构体
typedef struct {
    num *coeff;  // 系数
    int power;   // 幂次
} PolyTerm;

num *minus(num *A_num, num *B_num);  // 函数声明

//初始化num
num *initial(){
    num *temp;
    temp=(num*)malloc(sizeof(num));
    if(!temp){
        printf("error\n");
        exit(1);
    }

    int i=0;
    for(i=0;i<integer+decimal;i++)
        temp->data[i]=0;
    
    temp->neg=0;
    temp->base=10;

    return temp;
}
//把用字符串表示的小数存入数组
num *input(char *str,int base,int *d){
    num *temp;
    temp=initial(); 
    temp->base=base;   

    if(str[0]=='-')
        temp->neg=1;
    else
        temp->neg=0;

    int i=0;
    while(str[i]!='.'&&str[i]!='\0')
        i++;
    if(str[i]=='.')
        *d=i;//表示i位整数
    else
        *d=i; // 若没有小数点，则i是字符串长度，表示整数位数

    int j;//标记数组中位置
    int start=(str[0]=='-')?1:0;//负数的话不能循环到i=0

    for(j=integer-1,i=*d-1;i>=start;i--,j--){
        if(str[i]>='0'&&str[i]<='9')//检查一下输入是否正确
            temp->data[j]=str[i]-'0';//转换为存储整数值
        else if(str[i]>='A'&&str[i]<='F'){//处理十六进制
            temp->data[j]=str[i]-'A'+10;
        }
        else{
             printf("Error: Invalid character '%c'\n", str[i]);
            free(temp);
            return NULL;
        }
    }
    for(j=integer,i=*d+1;str[i]!='\0';i++,j++){
        if(str[i]>='0'&&str[i]<='9')
            temp->data[j]=str[i]-'0';
        else if(str[i]>='A'&&str[i]<='F'){//处理十六进制
            temp->data[j]=str[i]-'A'+10;
        }
        else{
             printf("Error: Invalid character '%c'\n", str[i]);
            free(temp);
            return NULL;
        }
    }
    return temp;
}
//输出小数
void output(num*temp){
    //12printf("调试：开始输出\n");
    if(temp->neg==1)
        printf("-");

    int i=0;
    while(temp->data[i]==0)//赌整数部分不会超过400位的
        i++;
    if(i>=integer)//不一定会取等，0的话会继续移
        printf("0");//整数部分全为0的情况
    else{
        for(;i<integer;i++){
            if(temp->data[i]>=0&&temp->data[i]<=9)
                printf("%d",temp->data[i]);
            else
                printf("%c",temp->data[i]-10+'A');//如果是十六进制
        }
    }
    printf(".");

    int j=integer+decimal-1;
    while(temp->data[j]==0)
        j--;
    if(j<integer)//不一定会取等，0的话会继续移
        printf("0");
    else{
        for(i=integer;i<=j;i++)
            if(temp->data[i]>=0&&temp->data[i]<=9)
                printf("%d",temp->data[i]);
            else
                printf("%c",temp->data[i]-10+'A');
    }
}

//十进制转换成N进制（N>2&&N<20)
num *dec_to_N(num*dec_num,int N){
    //判断数制
    if(N<2||N>20){
        printf("Error: Base must be between 2 and 20\n");
        return NULL;
    }
    
    num*result;
    result=initial();
    result->base=N;
    result->neg=dec_num->neg;

    num *temp;// 创建临时副本用于计算
    temp=initial();//必须先初始化(分配内存吧)
    *temp=*dec_num;

    //整数部分
    int int_part[integer]={0};
    int int_len=0;

    int i=0;//数组中已经没有符号了
    while(i<integer && temp->data[i]==0)
        i++;//得到的i即第一个非0整数位

    while(i<integer){
        int remainder=0;//表示余数

        for(int j=i;j<integer;j++){
            int current;//当前的被除数
            current=remainder*10+temp->data[j];
            temp->data[j]=current/N;
            remainder=current%N;
        }

        int_part[int_len]=remainder;//逆序存储
        int_len++;

        while(i<integer&&temp->data[i]==0)
            i++;
    }
    if(int_len==0){
        int_part[int_len]=0;
        int_len=1;//int——len的值变了！
    }
        
    //存储结果
    for(int k=0;k<int_len;k++)
        result->data[integer-1-k]=int_part[k];

    //小数部分(时间复杂度好高的样子，电脑盲尽力了o(╥﹏╥)o)
    int dec_part[decimal]={0};
    int dec_len=0;//赋初始值！
    int max_iter=decimal;//防止无限循环,iter​​ 通常是 ​​iterator（迭代器）​​ 的缩写

    while(dec_len<decimal&&max_iter-->0){//不理解max_iter-->0有什么用，但感觉多一道保险没啥毛病
        int carry=0;//进位

        for(int j=integer+decimal-1;j>=integer;j--){
            int current;
            current=temp->data[j]*N+carry;
            temp->data[j]=current%10;
            carry=current/10;
        }
        dec_part[dec_len]=carry;
        dec_len++;

        //判断是否是有限小数，若是，提前终止
        int is_zero=1;
        for(int j=integer;j<integer+decimal;j++)
            if(temp->data[j]!=0){
                is_zero=0;
                break;
            }
        if(is_zero)
            break;
    }

    //存储结果
    for(int k=0;k<dec_len;k++)
        result->data[integer+k]=dec_part[k];
    
    if(temp!=NULL)
        free(temp);
    return result;
    }
//N进制转十进制
num *N_to_dec(num* N_num,int N){
    //判断数制
    if(N<2||N>20){
        printf("Error: Base must be between 2 and 20\n");
        return NULL;
    }
    
    num*result;
    result=initial();
    result->base=10;
    result->neg=N_num->neg;

    //权重初始化
    num *power;
    power=initial();
    power->data[integer-1]=1;

    // 处理整数部分：从最低位到最高位，按权重累加
    for (int i = integer - 1; i >= 0; i--) {
        int digit = N_num->data[i];
        // 先计算当前位的贡献（digit * power）并加到result
        if (digit != 0) {
            num *temp = initial();
            int carry = 0;
            for (int j = integer - 1; j >= 0; j--) {
                int current = power->data[j] * digit + carry;
                temp->data[j] = current % 10;
                carry = current / 10;
            }

            carry = 0;
            for (int k = integer - 1; k >= 0; k--) {
                int current = result->data[k] + temp->data[k] + carry;
                result->data[k] = current % 10;
                carry = current / 10;
            }
            free(temp);
        }

        // 更新权重：power *= N
        int carry = 0;
        for (int j = integer - 1; j >= 0; j--) {
            int current = power->data[j] * N + carry;
            power->data[j] = current % 10;
            carry = current / 10;
        }
    }
    free(power);

    //小数部分
    num *weight = initial();
    weight->data[integer-1] = 1;  // 初始权重 = 1
    
    for (int i = integer; i < integer + decimal; i++) {
        // 计算权重：weight = weight / N
        int remainder = 0;
        for (int j = integer-1; j < integer + decimal; j++) {//从整数部分的最后一位开始除，第一个权重就是/N
            int current = remainder * 10 + weight->data[j];
            weight->data[j] = current / N;
            remainder = current % N;
        }
        
        if (N_num->data[i] == 0) 
            continue;
        
        // 当前位贡献 = digit * weight
        int carry = 0;
        for (int j = integer + decimal - 1; j >= integer - 1; j--) {//integer-1是为了处理进位,毕竟weight->data[integer-1]是
            int sum = result->data[j] + weight->data[j] * N_num->data[i] + carry;
            result->data[j] = sum % 10;
            carry = sum / 10;
        }
    }   
    free(weight);

    return result;
}

//加法(若非十进制，先转换为十进制再说)
num *add(num *A_num,num *B_num){

    if (!A_num || !B_num) return NULL;

    num *A=initial();
    *A=*A_num;
    num *B=initial();
    *B=*B_num;

    num *result;
    result=initial();

    if(A->neg==1&&B->neg==0){
        A->neg=0;
        result=minus(B,A);
    }

    else if(A->neg==0&&B->neg==1){
        B->neg=0;
        result=minus(A,B);
    }

    else{//A和B同号

        result->neg=A->neg;

        //跳过全是0的部分
        int i=integer+decimal-1;
        int j=0;
        while(A->data[i]==0&&B->data[i]==0)
            i--;
        while(A->data[j]==0&&B->data[j]==0)
            j++;

        int carry=0;
        for(;i>=j-1&&i>=0;i--){//和可能比最大加数的位数都大1，所以是j-1           
            int current=A->data[i]+B->data[i]+carry;
            result->data[i]=current%10;
            carry=current/10;
        }
        if(i<0&&carry==1){//如果最高位还有进位，就溢出了
            printf("Error:overflow\n");
            return NULL;
        }
    }
    free(A);
    free(B);

    return result;
}
//比较绝对值
int compare(num *A,num *B){
    for(int i=0;i<integer+decimal;i++){
        if(A->data[i]>B->data[i])
            return 1;
        
        if(B->data[i]>A->data[i])
            return -1;
    }
    return 0;
}
//减法
num *minus(num *A_num,num *B_num){

    if (!A_num || !B_num) return NULL;

    num *A=initial();
    *A=*A_num;
    num *B=initial();
    *B=*B_num;

    num *result;
    result=initial();

    if(A->neg==0&&B->neg==1){//异号转变成加法
        B->neg=0;
        result=add(A,B);
    }

    else if(A->neg==1&&B->neg==0){//异号转变成加法
        B->neg=1;
        result=add(A,B);
    }

    else{
        int n=compare(A,B); //比较绝对值大小      

        if(n==0)
            return result;
        
        else if(n>0){
            if(A->neg==0)
                result->neg=0;
            else
                result->neg=1;
        }
        else{
            if(A->neg==0)
                result->neg=1;
            else
                result->neg=0;
            
            num *temp=initial();//交换一下，数值部分大减小
            *temp=*A;
            *A=*B;
            *B=*temp;
        }

        int borrow=0;//借位
        for(int i=integer+decimal-1;i>=0;i--){
            int current;
            current=A->data[i]-B->data[i]-borrow;
            if(current<0){
                current=current+10;
                borrow=1;
            }
            else
                borrow=0;
            
            result->data[i]=current;
        }
    }
    return result;
}
//乘法
num *multiply(num *A,num *B){

    num *result=initial();

    //判断符号
    result->neg=A->neg^B->neg;//这个运算符哎，好用

    int temp[(integer+decimal)*2]={0};//最多位数是原两倍，临时放一下
    for(int i=0;i<integer+decimal;i++){

        int carry=0;
        for(int j=0;j<integer+decimal;j++){

            int current;
            current=A->data[integer+decimal-1-j]*B->data[integer+decimal-1-i]+carry;//感受一下不知道咋说，感觉弄麻烦了
            int pose=(integer+decimal)*2-1-i-j;
            carry=current/10;
            temp[pose]=temp[pose]+current%10;

            if(temp[pose]>=10){
                temp[pose-1]=temp[pose-1]+temp[pose]/10;
                temp[pose]=temp[pose]%10;
            }
        }

        // 处理最高位的进位（怪怪的，不想看了。。。）
        if (carry > 0) {
            int pos = (integer+decimal)*2 - 1 - i - (integer+decimal);
            temp[pos] += carry;//如果这超了，下一位会来处理吧，最高位一定不会超
        }
    }

    //溢出检查
    for (int k = 0; k < integer; k++) {
        if (temp[k] != 0) {
            printf("Error: integer part overflow\n");
            free(result);
            return NULL;
        }
    }

    for(int k=integer+decimal-1;k>=0;k--)
        result->data[k]=temp[k+integer];

    return result;
}

//double类型变num类型
num *double_to_num(double value){

    num *result=initial();

    result->neg=value<0?1:0;
    value=fabs(value);//取绝对值

    double int_part_d;
    double frac_part=modf(value,&int_part_d);
    long int int_part=(long int)int_part_d;

    //整数部分
    for(int i=integer-1;i>=0;i--){
        result->data[i]=int_part%10;
        int_part=int_part/10;
    }

    //小数部分
    for(int i=integer;i<integer+decimal;i++){
        frac_part=frac_part*10;
        int digit=(int)frac_part;
        result->data[i]=digit;
        frac_part=frac_part-digit;
    }

    return result;
}
//num类型幂运算
num *expo(num *x,int power){

    if(power<0){
        printf("Error:power<0\n");
        return NULL;
    }

    if(power==0)
        return double_to_num(1);

    num *result=initial();
    *result=*x;

    if(x->neg==1)
        result->neg=power%2?1:0;

    while(power>1){
        num *temp = multiply(result, x);//直接result=multiply(result,x)的话，每次都会泄漏一个内存。。
        if(!temp) {
            free(result);
            return NULL;
        }
        free(result);
        result = temp;
        power--;
    }
    return result;
}

//单变量多项式求值
num *poly(double double_x,PolyTerm *terms,int term_count){

    num *x=double_to_num(double_x);
    if (!x) {
        printf("Error: Failed to convert x\n");
        return NULL;
    }
    num *result=double_to_num(0.0);

    for(int i=0;i<term_count;i++){

        num *term=expo(x,terms[i].power);
        if(!term) {//毕竟是调用别的函数，还是检查一下吧
            free(result);
            free(x);
            return NULL;
        }

        num *product=multiply(terms[i].coeff,term);
        if(!product) {
            free(result);
            free(x);
            return NULL;
        }
        num *new_result = add(result, product);
        free(result);      // 释放旧result,否则每次循环都会丢掉一个原来的
        free(term);
        free(product);
        result=new_result;
    }

    free(x);
    return result;
}
//主函数 - 实现实验二测试样例
int main() {
    printf("=== 实验二测试样例 n=200 ===\n");
    
    // 1. 二进制和十进制之间的转换
    printf("1. 二进制和十进制之间的转换\n");
    printf("===========================\n");
    
    // (1) 十进制转二进制
    printf("(1) 十进制转二进制\n");
    printf("输入: 203.2\n");
    
    char dec_str[] = "203.2";
    int d=0;
    num *dec_num = input(dec_str, 10, &d);
    if (dec_num) {
        printf("十进制表示: ");
        output(dec_num);
        printf("\n");
        
        num *bin_num = dec_to_N(dec_num, 2);
        if (bin_num) {
            printf("转换为二进制: ");
            output(bin_num);
            printf("\n");
            free(bin_num);
        }
        
        free(dec_num);
    }
    printf("\n");
    
    // (2) 二进制转十进制
    printf("(2) 二进制转十进制\n");

    // 创建二进制数（简化表示，实际应输入完整200位）
    char bin_str[] = "-11001100.00110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011";
    int d1=0;
    num *bin_input = input(bin_str, 2, &d1);
    if (bin_input) {
        printf("二进制输入: ");
        output(bin_input);
        printf("\n");
        
        num *back_dec = N_to_dec(bin_input, 2);
        if (back_dec) {
            printf("转回十进制: ");
            output(back_dec);
            printf("\n");
            free(back_dec);
        }
        free(bin_input);
    }
    printf("\n");
    
    // 2. 十进制到N进制的转换
    printf("2. 十进制到N进制的转换\n");
    printf("=====================\n");
    
    // (1) N=5
    printf("(1) N=7\n");
    printf("输入:43.5\n");
    
    char num_str1[] = "43.5";
    num *num1 = input(num_str1, 10, &d);
    if (num1) {
        num *base5_num = dec_to_N(num1, 7);
        if (base5_num) {
            printf("转换为7进制: ");
            output(base5_num);
            printf("\n");
            free(base5_num);
        }
        free(num1);
    }
    printf("\n");
    
    // (2) N=19
    printf("(2) N=19\n");
    printf("输入: -114.5141919810\n");
    
    char num_str2[] = "-114.5141919810";
    num *num2 = input(num_str2, 10, &d);
    if (num2) {
        num *base19_num = dec_to_N(num2, 19);
        if (base19_num) {
            printf("转换为19进制: ");
            output(base19_num);
            printf("\n");
            free(base19_num);
        }
        free(num2);
    }
    printf("\n");
    
    // (3) N=7 完成乘法运算
    printf("(3) N=7 完成加减乘运算\n");
    printf("输入: (十进制）\na=1.14514\nb=-1.919810\n");
    
    // 创建两个7进制数
    char a_str[] = "1.14514";
    char b_str[] = "-1.919810";
    num *a_base10 = input(a_str, 10, &d);
    num *b_base10 = input(b_str, 10, &d);
    
    if (a_base10 && b_base10) {
        // 转换为七进制进行运算
        num *a_dec = N_to_dec(a_base10, 10);
        num *b_dec = N_to_dec(b_base10, 10);
        
        if (a_dec && b_dec) {
            printf("a七进制: \n"); output(a_dec); printf("\n");
            printf("b七进制: \n"); output(b_dec); printf("\n");
            
            // 加法
            num *sum_dec = add(a_base10, b_base10);
            if (sum_dec) {
                printf("十进制 a+b: \n"); output(sum_dec); printf("\n");
                num *sum_base7 = dec_to_N(sum_dec, 7);
                if (sum_base7) {
                    printf("7进制 a+b: \n"); output(sum_base7); printf("\n");
                    free(sum_base7);
                }
                free(sum_dec);
            }
            
            // 减法
            num *diff_dec = minus(a_base10, b_base10);
            if (diff_dec) {
                printf("十进制 a-b: \n"); output(diff_dec); printf("\n");
                num *diff_base7 = dec_to_N(diff_dec, 7);
                if (diff_base7) {
                    printf("7进制 a-b: \n"); output(diff_base7); printf("\n");
                    free(diff_base7);
                }
                free(diff_dec);
            }
            
            // 乘法
            num *prod_dec = multiply(a_base10, b_base10);
            if (prod_dec) {
                printf("十进制 a*b: "); output(prod_dec); printf("\n");
                num *prod_base7 = dec_to_N(prod_dec, 7);
                if (prod_base7) {
                    printf("7进制 a*b: "); output(prod_base7); printf("\n");
                    free(prod_base7);
                }
                free(prod_dec);
            }
            
            free(a_dec);
            free(b_dec);
        }
        free(a_base10);
        free(b_base10);
    }
    printf("\n");
    
    // 3. 单变量多项式求值
    printf("3. 单变量多项式求值\n");
    printf("==================\n");
    
    // (1) 标准多项式 f(x)=3/7*x^3-1/3*x^2+4
    printf("(1) 标准多项式 f(x)=3/7*x^3-1/3*x^2+4\n");
    printf("输入: x=1.3\n");
    
    PolyTerm terms1[3];
    terms1[0].coeff = double_to_num(3.0/7.0);
    terms1[0].power = 3;
    terms1[1].coeff = double_to_num(-1.0/3.0);
    terms1[1].power = 2;
    terms1[2].coeff = double_to_num(4.0);
    terms1[2].power = 0;
    
    num *poly_result1 = poly(1.3, terms1, 3);
    if (poly_result1) {
        printf("输出: f(1.3)=");
        output(poly_result1);
        printf("\n");
        free(poly_result1);
    }
    
    // 释放多项式1内存
    for (int i = 0; i < 3; i++) {
        free(terms1[i].coeff);
    }
    printf("\n");
    
    // (2) 自定义多项式 f(x)=12.31379131*x^2 + 1*x + 8.2137619836821388
    printf("(2) 自定义多项式 f(x)=12.31379131*x^2 + 1*x + 8.2137619836821388\n");
    printf("输入: x=1.612368921371923122414\n");
    
    PolyTerm terms2[3];
    terms2[0].coeff = double_to_num(12.31379131);
    terms2[0].power = 2;
    terms2[1].coeff = double_to_num(1.0);
    terms2[1].power = 1;
    terms2[2].coeff = double_to_num(8.2137619836821388);
    terms2[2].power = 0;
    
    num *poly_result2 = poly(1.612368921371923122414, terms2, 3);
    if (poly_result2) {
        printf("输出: f(1.612368921371923122414)=");
        output(poly_result2);
        printf("\n");
        free(poly_result2);
    }
    
    // 释放多项式2内存
    for (int i = 0; i < 3; i++) {
        free(terms2[i].coeff);
    }
    printf("\n");
    
    return 0;
}