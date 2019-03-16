
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039438942208>

/*==--floatTidf.c-工具uu floatTidf----------------------------------==
 *
 *LLVM编译器基础结构
 *
 *此文件是麻省理工学院和伊利诺伊大学公开赛的双重许可文件。
 *源许可证。有关详细信息，请参阅license.txt。
 *
 *==-------------------------------------------------------------------------------------==
 *
 *此文件实现编译器库的floattidf。
 *
 *==-------------------------------------------------------------------------------------==
 **/


/*返回：将A转换为双精度，四舍五入为偶数*/

/*假设：double是IEEE 64位浮点类型
 *ti_int是128位整数类型
 **/


/*参见eeee eeee mmmm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm mm*/

#include <float.h> //需要进行数据挖掘
#include "int_t.h"

double ___floattidf(__int128 a)
{
    if (a == 0)
        return 0.0;
    const unsigned N = sizeof(ti_int) * CHAR_BIT;
    const ti_int s = a >> (N-1);
    a = (a ^ s) - s;
    /*sd=n-uu clzti2（a）；/*有效位数*/
    int e=sd-1；/*指数*/

    if (sd > DBL_MANT_DIG)
    {
        /*开始：000000000000000000001XXXXXXXXXXXXXXXXXXPQXXXXXXXXXXXX
         *完成：000000000000000000000000000000000 1XXXXXXXXXXXXXXXXXXPQR
         *12345678901234567890123456
         * 1＝MSB 1位
         *P=1右边的位dbl_mant_dig-1
         *Q=1右边的位dbl_mant_dig位
         *r=“or”Q右边所有位的值
         **/

        switch (sd)
        {
        case DBL_MANT_DIG + 1:
            a <<= 1;
            break;
        case DBL_MANT_DIG + 2:
            break;
        default:
            a = ((tu_int)a >> (sd - (DBL_MANT_DIG+2))) |
                ((a & ((tu_int)(-1) >> ((N + DBL_MANT_DIG+2) - sd))) != 0);
        };
        /*完成：*/
        /*=（A和4）！=0；/*或p到r*/
        ++A；/*四舍五入-此步骤可能会增加一个有效位*/

        /*>=2；/*转储q和r*/
        /*A现在四舍五入为dbl_mant_dig或dbl_mant_dig+1位*/

        if (a & ((tu_int)1 << DBL_MANT_DIG))
        {
            a >>= 1;
            ++e;
        }
        /*A现在四舍五入为dbl-mant-dig位*/
    }
    else
    {
        a <<= (DBL_MANT_DIG - sd);
        /*A现在四舍五入为dbl-mant-dig位*/
    }
    double_bits fb;
    /*U.S.HIGH=（uint32_t）S&0x8000000）/*符号*/
                （（E+1023）<20）/*指数*/

                /*int32_t）（a>>32）&0x000ffff）；/*尾数高*/
    fb.u.s.low=（uint32_t）a；/*尾数低*/

    return fb.f;
}

