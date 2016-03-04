class CheckPoints
{
 private:
 unsigned int t;
 unsigned int current1;
 unsigned int current2;
 unsigned int current3;

 public:
 CheckPoints(){current1 = current2 = current3 = 0;}
 inline unsigned int IncCheckPoint1()
 {t=current1; current1++; return t;}
 inline unsigned int IncCheckPoint2()
 {t=current2; current2++; return t;}
 inline unsigned int IncCheckPoint3()
 {t=current3; current3++; return t;}
};

CheckPoints g_cp;

 