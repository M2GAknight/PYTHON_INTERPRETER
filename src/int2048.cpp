#include "int2048.h"
#include <sstream>
#include <string>
namespace sjtu
{
    const int Base = 10000;
    const double PI = 3.14159265358979323;
    std::vector<std::complex<double>> fft(std::vector<std::complex<double>> a, bool pd)
    {
        int n = a.size();
        if (n == 1)
            return a;
        std::vector<std::complex<double>> ji, ou;
        for (int i = 0; i < n; i++)//先把奇偶分组
        {
            if (i % 2 == 0)
                ou.push_back(a[i]);
            else
                ji.push_back(a[i]);
        }
        std::vector<std::complex<double>> y0 = fft(ou, pd), y1 = fft(ji, pd);
        double theta = 2 * PI / n;
        if (pd)
            theta = -theta;
        std::vector<std::complex<double>> res(n);
        std::vector<std::complex<double>> wtab(n / 2);
        for (int k = 0; k < n / 2; k++)
            wtab[k] = std::polar(1.0, theta * k);
        for (int k = 0; k < n / 2; k++)
        {
            std::complex<double> t = wtab[k] * y1[k];
            res[k] = y0[k] + t;
            res[k + n / 2] = y0[k] - t;
        }
        return res;
    }
    std::vector<int> fftCheng(const std::vector<int> &a, const std::vector<int> &b)
    {
        std::vector<int> ans;
        ans.push_back(0);
        if (a == ans || b == ans)
            return ans;
        int need = (int)a.size() + (int)b.size() - 1;
        int N = 1;
        while (N < need)
            N *= 2;
        std::vector<std::complex<double>> x(N), y(N);
        for (int i = 0; i < a.size(); i++)
            x[i] = (double)(a[i]);
        for (int i = 0; i < b.size(); i++)
            y[i] = (double)b[i];
        x = fft(x, 0);
        y = fft(y, 0);
        for (int i = 0; i < N; i++)
            x[i] = x[i] * y[i];
        x = fft(x, 1);
        for (int i = 0; i < N; i++)
            x[i] /= N;
        std::vector<long long> tmp(N);
        for (int i = 0; i < N; i++)
        {
            double p = x[i].real();
            if (p > -0.5 && p < 0)
                p = 0;
            tmp[i] = (long long)(p + 0.5);
        }
        std::vector<int> res;
        long long carry = 0;
        for (int i = 0; i < N; i++)
        {
            long long cur = tmp[i] + carry;
            res.push_back((int)(cur % 10000));
            carry = cur / 10000;
        }
        while (carry > 0)
        {
            res.push_back((int)(carry % 10000));
            carry /= 10000;
        }
        while (res.size() > 1 && res.back() == 0)
            res.pop_back();
        if (res.empty())
            res.push_back(0);
        return res;
    }
    int cmpAbs(const std::vector<int> &a, const std::vector<int> &b)
    {
        if (a.size() != b.size())
        {
            if (a.size() > b.size())
                return 1;
            else
                return -1;
        }

        for (int i = a.size() - 1; i >= 0; i--)
        {
            if (a[i] != b[i])
            {
                if (a[i] > b[i])
                    return 1;
                else
                    return -1;
            }
        }
        return 0;
    }
    std::vector<int> subInplace(std::vector<int> a, std::vector<int> b)
    {
        long long borrow = 0;
        int n = (int)a.size();
        int m = (int)b.size();

        for (int i = 0; i < n; ++i)
        {
            long long cur = (long long)a[i] - borrow;
            if (i < m)
                cur -= b[i];

            if (cur < 0)
            {
                cur += Base;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }
            a[i] = (int)cur;
        }

        while (a.size() > 1 && a.back() == 0)
            a.pop_back();
        if (a.empty())
            a.push_back(0);

        return a;
    }
    std::vector<int> Muld(const std::vector<int> &a, int d)
    {
        std::vector<int> ans;
        ans.push_back(0);
        if (d == 0)
            return ans;
        ans.pop_back();
        int cur = 0, carry = 0;
        for (int i = 0; i < a.size(); i++)
        {
            cur = a[i] * d + carry;
            ans.push_back(cur % Base);
            carry = cur / Base;
        }
        while (carry)
        {
            ans.push_back(carry % Base);
            carry /= Base;
        }
        return ans;
    }
    struct Node
    {
        std::vector<int> q, r;
    };
    Node Dandm(std::vector<int> a, std::vector<int> b)
    {
        std::vector<int> zero;
        Node ans;
        ans.q.assign(a.size(), 0);
        zero.push_back(0);
        if (cmpAbs(a, b) == -1)//如果a<b
        {
            ans.q = zero;
            ans.r = a;
            return ans;
        }
        if (b.size() == 1)//只有一位，从高位往下除就行
        {
            ans.q.assign(a.size(), 0);
            long long rem = 0;
            for (int i = (int)a.size() - 1; i >= 0; --i)
            {
                rem = rem * Base + a[i];
                ans.q[i] = (int)(rem / b[0]);
                rem %= b[0];
            }
            while (ans.q.size() > 1 && ans.q.back() == 0)
                ans.q.pop_back();
            ans.r.clear();
            ans.r.push_back((int)rem);
            return ans;
        }
        std::vector<int> nq, rem;
        for (int i = a.size() - 1; i >= 0; i--)
        {
            rem.insert(rem.begin(), a[i]);
            while (rem.size() > 1 && rem.back() == 0)
                rem.pop_back();

            int rs = (int)rem.size(), bs = (int)b.size();
            int rr = Base - 1;
            int ll = 0;
            while (ll < rr)
            {
                int mid = (ll + rr + 1) / 2;
                std::vector<int> t = Muld(b, mid);
                if (cmpAbs(t, rem) <= 0)
                    ll = mid;
                else
                    rr = mid - 1;
            }
            int d = ll;
            ans.q[i] = d;
            rem = subInplace(rem, Muld(b, d));
        }
        while (ans.q.size() > 1 && ans.q.back() == 0)
            ans.q.pop_back();
        while (rem.size() > 1 && rem.back() == 0)
            rem.pop_back();
        ans.r = rem;
        return ans;
    }
    int2048::int2048()
    {
        num.clear();
        num.push_back(0);
        sign = 0;
    }
    int2048::int2048(long long x)//先判符号转化为绝对值.
    {
        num.clear();
        sign = 0;
        unsigned long long ux;
        if (x < 0)
        {
            ux = (unsigned long long)0 - x;
            sign = 1;
        }
        else
            ux = x;
        if (ux == 0)
        {
            sign = 0;
            num.push_back(0);
            return;
        }
        while (ux > 0)
        {
            num.push_back(ux % Base);
            ux /= Base;
        }
    }
    int2048::int2048(const std::string &s)
    {
        read(s);
    }
    void int2048::Solvezero()//弹出前导零，注意如果只剩一个0不要谈
    {
        while (num.size() > 1 && num.back() == 0)
            num.pop_back();
        if (num.empty())
            num.push_back(0);
        if (num.size() == 1 && num[0] == 0)//这里第一次写没考虑到
            sign = 0;
    }

    void int2048::Uadd(const int2048 &x)//两个无符号的数直接相加
    {
        std::vector<int> b = x.num;
        int n = (int)num.size();
        int m = (int)b.size();
        int len = n > m ? n : m;//选两个加数中大的一个先假设为长度

        num.resize(len, 0); // 短的补 0，保证下标不越界

        long long carry = 0;//进位
        for (int i = 0; i < len; ++i)
        {
            long long sum = (long long)num[i] + carry;
            if (i < m)
                sum += b[i];
            num[i] = (int)(sum % Base);
            carry = sum / Base;
        }

        // 最高进位
        while (carry > 0)
        {
            num.push_back((int)(carry % Base));
            carry /= Base;
        }
    }
    int int2048::cmpAbs(const int2048 &x) const
    {
        int s1 = num.size(), s2 = x.num.size();
        if (s1 != s2)
        {
            if (s1 > s2)
                return 1;
            else
                return -1;
        }
        for (int i = s1 - 1; i >= 0; i--)
        {
            if (num[i] != x.num[i])
            {
                if (num[i] > x.num[i])
                    return 1;
                else
                    return -1;
            }
        }
        return 0;
    }
    void int2048::Output(std::ostream &os) const//这里任意一个ostream变量都可以当类似cin一样的东西
    {
        std::string s = "";
        if (sign == 1)
            os << '-';
        os << num.back();
        for (int j = (int)num.size() - 2; j >= 0; j--)
        {
            s += Tostring(num[j]);
        }
        os << s;
    }
    void int2048::Uminus(const int2048 &x)
    {
        int n = (int)num.size();
        int m = (int)x.num.size();

        long long borrow = 0;
        for (int i = 0; i < n; ++i)
        {
            long long cur = (long long)num[i] - borrow;
            if (i < m)
                cur -= x.num[i];

            if (cur < 0)
            {
                cur += Base;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }
            num[i] = (int)cur;
        }
        while (num.size() > 1 && num.back() == 0)
            num.pop_back();
    }

    void int2048::read(const std::string &s)//倒着录入（tot记录录了几次），最后通过s[0]判断正负，最后如果还有没录入的就全部最后pushback。
    {
        num.clear();
        sign = 0;
        if (s.empty())
        {
            num.push_back(0);
            return;
        }
        int r = 0, tot = 0, w = 1;
        for (int i = s.size() - 1; i >= 0; i--)
        {
            if (i == 0)
            {
                if (s[0] == '-')
                {
                    sign = 1;
                    break;
                }
                else if (s[0] == '+')
                {
                    break;
                }
            }
            int k = (int)(s[i] - '0');
            r = r + k * w;
            w *= 10;
            tot++;
            if (tot == 4)
            {
                tot = 0;
                num.push_back(r);
                r = 0;
                w = 1;
            }
        }
        if (tot > 0)//这里当初没有考虑到
            num.push_back(r);
        Solvezero();
    }
    std::string int2048::Tostring(int x) const//对每个四位进行拆解成string
    {
        std::string s = "";
        for (int k = 0; k <= 3; k++)
        {
            s = char('0' + x % 10) + s;
            x /= 10;
        }
        return s;
    }
    int2048::int2048(const int2048 &x)
    {
        sign = x.sign;
        num = x.num;
    }
    void int2048::print()//先转换成一个字符串再输出
    {
        std::string s = "";
        if (sign == 1)
            printf("-");
        printf("%d", num.back());
        for (int j = (int)num.size() - 2; j >= 0; j--)
        {
            s += Tostring(num[j]);
        }
        std::cout << s;
    }
    std::string itos(const int2048 x)
    {
        std::stringstream os;
        os<<x;
        return os.str();
    }
    double itold(const int2048 x)
    {
        return (double)std::stold(itos(x));
    }
    long long itoll(const int2048 x)
    {
        return std::stoll(itos(x));
    }
    int2048 &int2048::add(const int2048 &x)
    {
        if (sign == x.sign)
        {
            Uadd(x);
            Solvezero();
            return *this;
        }
        else
        {
            int k = cmpAbs(x);
            if (k == 0)
            {
                num.clear();
                num.push_back(0);
                sign = 0;
                Solvezero();
                return *this;
            }
            else if (k == 1)
            {
                Uminus(x);
                Solvezero();
                return *this;
            }
            else
            {
                int2048 p(x);//注意不要直接用x去减，因为参数给的是引用，刚开始一直编译不通过
                p.Uminus(*this);
                this->num = p.num;
                this->sign = p.sign;
                Solvezero();
                return *this;
            }
        }
        // TODO: 在此处插入 return 语句
    }
    int2048 &int2048::minus(const int2048 &x)//相当于+（-x)
    {
        int2048 m(x);
        m.sign = (!x.sign);
        add(m);
        return *this;
        // TODO: 在此处插入 return 语句
    }
    int2048 int2048::operator+() const //这个意思只是把一个数前面加上一个正号…………一开始没看懂为啥有个没有参数的加号
    {
        return *this;
    }
    int2048 int2048::operator-() const//注意-0还是要变成0
    {
        int2048 r(*this);
        if (!(r.num.size() == 1 && r.num[0] == 0))
            r.sign = (!r.sign);
        return r;
    }
    int2048 &int2048::operator=(const int2048 &x)
    {
        this->num = x.num;
        this->sign = x.sign;
        return *this;
        // TODO: 在此处插入 return 语句
    }
    int2048 &int2048::operator+=(const int2048 &x)
    {
        add(x);
        return *this;
        // TODO: 在此处插入 return 语句
    }
    int2048 &int2048::operator-=(const int2048 &x)
    {
        minus(x);
        return *this;
        // TODO: 在此处插入 return 语句
    }
    int2048 &int2048::operator*=(const int2048 &x)
    {
        *this = *this * x;
        return *this;
        // TODO: 在此处插入 return 语句
    }
    int2048 &int2048::operator/=(const int2048 &x)
    {
        *this = *this / x;
        return *this;
    }
    int2048 &int2048::operator%=(const int2048 &x)
    {
        *this = *this % x;
        return *this;
    }
    int2048 add(int2048 a, const int2048 &x)
    {
        return a.add(x);
    }
    int2048 minus(int2048 a, const int2048 &x)
    {
        return a.minus(x);
    }
    int2048 operator+(int2048 a, const int2048 &b)
    {
        return a.add(b);
    }

    int2048 operator-(int2048 a, const int2048 &b)
    {
        return a.minus(b);
    }
    int2048 operator*(int2048 a, const int2048 &b)
    {
        int2048 ans;
        ans.sign = a.sign ^ b.sign;
        ans.num = fftCheng(a.num, b.num);
        ans.Solvezero();
        return ans;
    }
    int2048 operator/(int2048 a, const int2048 &b)
    {
        Node x = Dandm(a.num, b.num);
        int2048 ans;
        ans.num = x.q;
        if (a.sign == b.sign)
            ans.sign = 0;
        else
        {
            if (x.r.size() == 1 && x.r[0] == 0)
            {
                ans.sign = 1;
            }
            else
            {
                ans = ans + 1;
                ans.sign = 1;
            }
        }
        ans.Solvezero();
        return ans;
    }
    int2048 operator%(int2048 a, const int2048 &b)
    {
        Node x = Dandm(a.num, b.num);
        int2048 ans;
        ans.num = x.r;
        if (x.r.size() == 1 && x.r[0] == 0)
        {
            ans.sign = 0;
            return ans;
        }
        if (a.sign == b.sign)
        {
            ans.sign = b.sign;
            return ans;
        }
        else
        {
            ans.num = subInplace(b.num, x.r);
            ans.sign = b.sign;
        }
        ans.Solvezero();
        return ans;
    }
    bool operator==(const int2048 &a, const int2048 &b)
    {
        if (a.sign != b.sign)
            return 0;
        if (a.num != b.num)
            return 0;
        return 1;
    }
    bool operator!=(const int2048 &a, const int2048 &b)
    {
        return !(a == b);
    }
    bool operator<(const int2048 &a, const int2048 &b)
    {
        if (a == b)
            return false;
        if (a.sign != b.sign)
            return (a.sign);
        if (a.sign == 0)
            if (a.cmpAbs(b) == 1)
                return 0;
            else
                return 1;
        else
        {
            if (a.cmpAbs(b) == -1)
                return 0;
            else
                return 1;
        }
    }
    bool operator>(const int2048 &a, const int2048 &b)
    {
        return (b < a);
    }

    bool operator<=(const int2048 &a, const int2048 &b)
    {
        return !(a > b);
    }

    bool operator>=(const int2048 &a, const int2048 &b)
    {
        return !(a < b);
    }
    std::ostream &operator<<(std::ostream &os, const int2048 &x)
    {
        x.Output(os);
        return os;
    }
    std::istream &operator>>(std::istream &is, int2048 &x)
    {
        std::string s;
        is >> s;
        x.read(s);
        return is;
    }
//一个调了一段时间的bug：fft旋转因子只用double的话由于精度问题不能沿用longdouble的迭代乘法，只能刚开始就预处理，不然精度炸了
} // namespace sjtu
