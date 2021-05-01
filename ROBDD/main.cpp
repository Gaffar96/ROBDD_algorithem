#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>


using namespace std;
//class ROBDD;
class Func
{
    friend class ROBDD;

public:
    ~Func()
    {
    }
    Func(bool b)
    {
        m_ciVar = b ? TRUE : FALSE;
        m_pElse = nullptr;
        m_pThen = nullptr;
    }

    Func(int iVar, Func *t, Func *e) : m_ciVar(iVar)
    {
        m_pThen = t;
        m_pElse = e;
    }

    Func *getThen(int iVar)
    {
        return iVar == m_ciVar ? m_pThen : this;
    }

    Func *getElse(int iVar)
    {
        return iVar == m_ciVar ? m_pElse : this;
    }
    bool operator==(const Func &f) const
    {
        return (f.m_ciVar == m_ciVar && f.m_pThen == m_pThen && f.m_pElse == m_pElse);
    }
    bool operator<(const Func &d) const
    {
        if (m_ciVar < d.m_ciVar)
        {

            return true;
        }
        else if (m_ciVar == d.m_ciVar)
        {
            if (m_pThen < d.m_pThen)
            {
                return true;
            }
            else if (m_pThen == d.m_pThen)
            {
                if (m_pElse < d.m_pElse)
                {

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {

            return false;
        }
    }

    int getVar() { return m_ciVar; }

    bool isTrue() { return m_ciVar == TRUE; }
    bool isFalse() { return m_ciVar == FALSE; }
    bool isConstant() { return isTrue() || isFalse(); }

private:
    const int TRUE = 0x7fffffff;
    const int FALSE = TRUE - 1;
    int m_ciVar = 0;

    Func *m_pThen;
    Func *m_pElse;
};

class Triple
{

public:
    friend class ROBDD;
    Triple(int iVar, Func *fThen, Func *fElse)
    {
        m_ciVar = iVar;
        m_pThen = fThen;
        m_pElse = fElse;
    }

    bool operator==(const Triple &obj)
    {
        return obj.m_ciVar == m_ciVar &&
               obj.m_pThen == m_pThen &&
               obj.m_pElse == m_pElse;
    }

    bool operator<(const Triple &d) const
    {
        if (m_ciVar < d.m_ciVar)
        {

            return true;
        }
        else if (m_ciVar == d.m_ciVar)
        {
            if (m_pThen < d.m_pThen)
            {
                return true;
            }
            else if (m_pThen == d.m_pThen)
            {
                if (m_pElse < d.m_pElse)
                {

                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {

            return false;
        }
    }

private:
    int m_ciVar;
    Func *m_pThen;
    Func *m_pElse;
};

class FuncTriple
{

public:
    FuncTriple(Func *iVar, Func *fThen, Func *fElse)
    {
        m_ciVa = iVar;
        m_pThen = fThen;
        m_pElse = fElse;
    }

    bool operator==(const FuncTriple &obj)
    {
        return obj.m_ciVa == m_ciVa &&
               obj.m_pThen == m_pThen &&
               obj.m_pElse == m_pElse;
    }

    bool operator<(const FuncTriple &dh) const
    {

        if (m_ciVa < dh.m_ciVa)
        {

            return true;
        }
        else if (m_ciVa == dh.m_ciVa)
        {
            if (m_pThen < dh.m_pThen)
            {
                return true;
            }
            else if (m_pThen == dh.m_pThen)
            {
                if (m_pElse < dh.m_pElse)
                {

                    return true;
                }
                else
                {
                    return false;

                }
            }
            else
            {
                return false;
            }
        }
        else
        {

            return false;
        }
    }

private:
    Func *m_ciVa;
    Func *m_pThen;
    Func *m_pElse;
};

class ROBDD
{

public:
    ~ROBDD()
    {
        delete m_pTrue;
        delete m_pFalse;
        for (auto i = out2.begin(); i != out2.end(); ++i)
        {
            if (i->second != nullptr)
            {
                delete i->second;
            }
        }

        for (auto i = m_Unique.begin(); i != m_Unique.end(); ++i)
        {
            if (i->second != nullptr)
            {
                delete i->second;
            }
        }
    }

    ROBDD()
    {
        m_pTrue = new Func(true);
        m_pFalse = new Func(false);
    }

    Func *genTrue() { return m_pTrue; }
    Func *genFalse() { return m_pFalse; }

    Func *ite(Func *i, Func *t, Func *e)
    {
        if (i->isTrue())
        {
            return t;
        }
        else if (i->isFalse())
        {
            return e;
        }
        else if (t->isTrue() && e->isFalse())
        {
            return i;
        }
        else
        {
            FuncTriple args(i, t, e);
            auto re = m_Computed.find(args);
            if (re != m_Computed.end())
            {
                return re->second;
            }

            const int ciVar = min(min(i->getVar(), t->getVar()), e->getVar());
            Func *T = ite(i->getThen(ciVar), t->getThen(ciVar), e->getThen(ciVar));
            Func *E = ite(i->getElse(ciVar), t->getElse(ciVar), e->getElse(ciVar));
            if (T == E)
            {

                return T;
            }

            Triple entry(ciVar, T, E);
            auto res = m_Unique.find(entry);
            if (res == m_Unique.end())
            {
                res->second = new Func(ciVar, T, E);
                m_Unique.insert({entry, res->second});
            }
            m_Computed.insert({args, res->second});
            return res->second;
        }
    }

    Func *genVar(int i)
    {

        Triple entry(i, genTrue(), genFalse());
        auto res = m_Unique.find(entry);
        if (res == m_Unique.end())
        {

            res->second = new Func(i, genTrue(), genFalse());
            m_Unique.insert({entry, res->second});
        }

        return res->second;
    }

    Func *AND(Func *x, Func *y)
    {
        return ite(x, y, genFalse());
    }

    Func *AND(vector<Func *> &vec)
    {

        Func *Function = AND(vec[0], vec[1]);

        for (unsigned ui = 2; ui < vec.size(); ++ui)
            Function = AND(Function, vec[ui]);
        return Function;
    }

    Func *NOT(Func *x)
    {
        return ite(x, genFalse(), genTrue());
    }

    Func *NAND(Func *x, Func *y)
    {
        return ite(x, NOT(y), genTrue());
    }
    Func *NAND(vector<Func *> &vec)
    {
        return NOT(AND(vec));
    }
    Func *OR(Func *x, Func *y)
    {
        return ite(x, genTrue(), y);
    }
    Func *OR(vector<Func *> &vec)
    {
        Func *f = OR(vec[0], vec[1]);
        for (unsigned ui = 2; ui < vec.size(); ++ui)
            f = OR(f, vec[ui]);
        return f;
    }
    Func *XOR(Func *x, Func *y)
    {

        return ite(x, NOT(y), y);
    }
    Func *XOR(vector<Func *> &vec)
    {
        Func *f = XOR(vec[0], vec[1]);
        for (unsigned ui = 2; ui < vec.size(); ++ui)
            f = XOR(f, vec[ui]);
        return f;
    }
    Func *NOR(Func *x, Func *y)
    {
        return ite(x, genFalse(), NOT(y));
    }
    Func *NOR(vector<Func *> &vec)
    {
        return NOT(OR(vec));
    }

    void INPUT1(string s)
    {

        bool b = true;
        string s2 = "ldkf";
        for (int i = 0; i < s.length(); ++i)
        {
            if (b && s[i] != ' ' && s[i] != '=' && s[i] != ')' && s[i] != '(' && s[i] != ',' && s[i] != 'I' && s[i] != 'N' && s[i] != 'P' && s[i] != 'U' && s[i] != 'T')
            {
                int point = i;
                string testt = "";
                while (point < s.length() && s[point] != ' ' && s[point] != '=' && s[point] != ')' && s[point] != '(' && s[point] != ',')
                {
                    testt += s[point];
                    ++point;
                }
                s2 = string(testt);
                b = false;
            }        }
        int number = stoi(s2.substr(1));
        funcs.push_back(s2);
        Func *ke = genVar(number);

        inp.insert({s2, ke});
    }

    void OUTPUT1(string s)
    {

        bool b = true;
        string s2 = "ldkf";
        for (int i = 0; i < s.length(); ++i)
        {
            if (b && s[i] != ' ' && s[i] != '=' && s[i] != ')' && s[i] != '(' && s[i] != ',' && s[i] != 'T' && s[i] != 'O' && s[i] != 'P' && s[i] != 'U' && s[i] != 'T')
            {
                int point = i;
                string testt = "";
                while (point < s.length() && s[point] != ' ' && s[point] != '=' && s[point] != ')' && s[point] != '(' && s[point] != ',')
                {
                    testt += s[point];
                    ++point;
                }
                s2 = string(testt);
                b = false;
            }
        }
        ou.push_back(s2);
    }

    void NOT1(string s)
    {

        bool b = true;
        string s2 = "ldkf";
        for (int i = 0; i < s.length(); ++i)
        {
            if (b && s[i] != ' ' && s[i] != '=' && s[i] != ')' && s[i] != '(' && s[i] != ',' && s[i] != 'T' && s[i] != 'O' && s[i] != 'N' && s[i] != 'U' && s[i] != 'T')
            {
                int point = i;
                string testt = "";
                while (point < s.length() && s[point] != ' ' && s[point] != '=' && s[point] != ')' && s[point] != '(' && s[point] != ',')
                {
                    testt += s[point];
                    ++point;
                }
                s2 = string(testt);
                b = false;
            }
        }
        Func *i = NOT(vecs[0]);
        funcs.push_back(s2);
        inp.insert({s2, i});
        vecs.push_back(i);
    }

    void nand1(string s)
    {
        Func *i = NAND(vecs);
        funcs.push_back(s);
        inp.insert({s, i});
        vecs.push_back(i);
    }
    void and1(string s)
    {

        Func *i = AND(vecs);
        funcs.push_back(s);
        inp.insert({s, i});
        vecs.push_back(i);
    }
    void xnor1(string s, int i)
    {
        if (i == 1)
        {
            Func *i = XOR(vecs);
        }
        else
        {
            Func *i = NOR(vecs);
        }
    }
    void or1(string s)
    {
        Func *i = OR(vecs);
        funcs.push_back(s);
        inp.insert({s, i});
        vecs.push_back(i);
    }
    void doit(const string path)
    {

        if (true)
        {

            ifstream file(path);
            if (file)
            {
                string s, s2;
                string s3 = "";
                string s4 = "";
                string s5;
                unsigned i1, i2, i3, p4;
                bool ttt = false;

                while (getline(file, s))
                {
                    if (s != "" && s.find("#") == -1)
                    {
                        //hier ist such nach INPUT
                        for (int z = 0; z < s.length() - 4; ++z)
                        {

                            if (s[z] == 'I' && s[z + 1] == 'N' && s[z + 2] == 'P' && s[z + 3] == 'U' && s[z + 4] == 'T')
                            {
                                INPUT1(s);
                                ttt = true;
                            }
                        }

                        //output
                        if (s.find("OUTPUT") == 0)
                        {
                            // hier ist suche  nach OUTPUT
                            for (int z = 0; z < s.length() - 5; ++z)
                            {
                                if (s[z] == 'O' && s[z + 1] == 'U' && s[z + 2] == 'T' && s[z + 3] == 'P' && s[z + 4] == 'U' && s[z + 5] == 'T')
                                {

                                    OUTPUT1(s);
                                    ttt = true;
                                }
                            }
                        }

                        if (ttt)
                        {

                            i1 = s.find("=");
                            // for(int z = 0;z<s.length();++z){
                            //     if(s[z]=='=')
                            //     i1=z+1;
                            //     if(s[z]=='(')
                            //     i2=z+1;
                            //     if(s[z]==')')
                            //     i3=z+1;
                            // }
                            i2 = s.find("(");
                            i3 = s.find(")");

                            string before = s.substr(0, i1 - 1);
                            s5 = before;
                            before += "?";
                            before = before.substr(1, before.find("?") - 1);
                            auto serch = inp.find(s5);

                            string Functions = s.substr(i1 + 2, i2 - (i1 + 2));
                            string gs = s.substr(i2 + 1, i3 - (i2 + 1));

                            gs += "?";

                            if (serch == inp.end())
                            {

                                for (auto x : gs)
                                {
                                    if (x == ',' || x == '?')
                                    {

                                        s2 = s4.substr(0, s4.find(",") - 1);
                                        s3 = s2;
                                        s2 = s4.substr(1, s4.find(",") - 1);

                                        auto z1 = inp.find(s3);
                                        if (z1 != inp.end())
                                        {
                                            vecs.push_back(z1->second);
                                        }

                                        s4 = "";
                                    }
                                    else
                                    {
                                        s4 = s4 + x;
                                    }
                                }
                                // switch (std::Functions)
                                // {
                                // case  std::string("AND"):
                                //     and1(s5);

                                // case std::string("NAND"):
                                //     or1(s5);

                                // case std::string("OR"):
                                // or1(s5);

                                // case std::string("XOR"):
                                //  xnor1(s5,1);
                                //  case std::string("NOR"):
                                //  xnor1(s5,1);
                                // default:
                                //   cout<< "fast fertig "<<endl;

                                // }

                                if (Functions == "AND"){ and1(s5);}

                                if (Functions == "NAND"){nand1(s5);}
                                if (Functions == "OR"){or1(s5);}
                                if (Functions == "XOR"){xnor1(s5, 1);}
                                if (Functions == "NOR"){xnor1(s5, 2);}

                                for (int z = 0; z < s.length() - 2; ++z)
                                {
                                    if (s[z] == 'N' && s[z + 1] == 'O' && s[z + 2] == 'T')
                                    {
                                        NOT1(s);
                                    }
                                }

                                //}

                                vecs.erase(vecs.begin(), vecs.end());
                            }
                        }
                    }
                }
                file.close();
                ofstream file23;
                file23.open("./newGraph.udg");

                m(file23, 1);
                file23.close();
            }
        }
    }
    void paint(Func &f, int z, ofstream &file)
    {
        string str;

        file << "l(\"" << &f << "\",n(\"Module\",[a(\"OBJECT\",\""
             << "G" << z << "\")],[" << endl;

        if (f.m_pThen->isTrue() || f.m_pThen->isFalse())
        {
            if (f.m_pThen->isTrue())
                str = "True";
            else
                str = "False";
            file << "e(\"" << f.m_ciVar << "->" << f.m_pThen->m_ciVar << "\",[a(\"OBJECT\",\"1\")],r(\"" << str << "\")" << endl;
        }
        else
            file << "e(\"" << f.m_ciVar << "->" << f.m_pThen->m_ciVar << "\",[a(\"OBJECT\",\"1\")],r(\"" << f.m_pThen << "\")" << endl;
        file << ")," << endl;
        if (f.m_pElse->isTrue() || f.m_pElse->isFalse())
        {
            if (f.m_pElse->isTrue())
                str = "True";
            else
                str = "False";
            file << "e(\"" << f.m_ciVar << "->" << f.m_pElse->m_ciVar << "\",[a(\"OBJECT\",\"0\")],r(\"" << str << "\")" << endl;
        }
        else
            file << "e(\"" << f.m_ciVar << "->" << f.m_pElse->m_ciVar << "\",[a(\"OBJECT\",\"0\")],r(\"" << f.m_pElse << "\")" << endl;
        file << ")]))," << endl;
    }

    void create(Func &f, ofstream &file)
    {
        if (f.isTrue())
            return;
        if (f.isFalse())
            return;
        auto check = out2.find(f);
        if (check == out2.end())
        {
            check->second = new bool(true);
            out2.insert({f, check->second});
            paint(f, f.m_ciVar, file);
        }
        else
        {
        }
        create(*f.m_pThen, file);
        create(*f.m_pElse, file);
    }

    void m(ofstream &file, bool b)
    {
        if (b)
        {
            file << "[" << endl;
            b = 0;
        }
        int x = 0;

        for (int i = 0; i < ou.size(); ++i)
        {
            auto fun = inp.find(ou[i]);
            if (fun != inp.end())
            {
                Func &Functions = *fun->second;
                file << "l(\"" << ou[i] << "\",n(\"Module\",[a(\"OBJECT\",\"" << ou[i] << "\")],[" << endl;
                file << "e(\"" << ou[i] << "->" << Functions.m_ciVar << "\",[a(\"OBJECT\",\"1\")],r(\"" << &Functions << "\")" << endl;
                file << ")]))," << endl;
                create(*fun->second, file);
            }
        }

        file << "l(\"True\",n(\"Module\",[a(\"OBJECT\",\"True\")],[]))," << endl;
        file << "l(\"False\",n(\"Module\",[a(\"OBJECT\",\"False\")],[]))" << endl;
        file << "]";
        file.close();
    }

private:
    map<Triple, Func *> m_Unique;
    Func *m_pTrue = new Func(true);
    Func *m_pFalse = new Func(false);
    map<string, Func *> inp;
    vector<Func *> vecs;
    vector<string> funcs;
    vector<string> ou; //
    map<FuncTriple, Func *> m_Computed;
    map<Func, bool *> out2;
};

string *data()
{
    string *datas = new string[3];
    datas[0] = "c432.bench.txt";
    datas[1] = "c432.bench.txt";

    return datas;
}

void createfile1()
{
    ROBDD r;
    string path = "c432.bench.txt";
    ofstream file;
    r.doit(path);
}
void createfile2()
{
    ROBDD r;
    string path = "c432.bench.txt";
    ofstream file;
    r.doit(path);
}
void createfile3()
{
    ROBDD r;
    string path = "c432.bench.txt";
    ofstream file;
    r.doit(path);
}
void creatfiles()
{
    createfile1();
    createfile2();
    createfile3();
}

int main()
{
    string path1, path2, path3, path4, path5;

    ROBDD r;
    path1 = "c17.brench.txt";
    path2 = "c432.bench.txt";
    path3 = "c499.bench.txt";
    path4 = "c880.bench.txt";
    cout << "wählen Sie eine datei aus oder geben Sie die  nummer 0 um ihre path einzugeben" << endl;
    cout << "Sie können von 1 bis 4 whälen " << endl;
    int i;

    cin >> i;
    if (i == 1)
    {

        r.doit(path1);
    }
    if (i == 2)
    {

        r.doit(path2);
    }
    if (i == 3)
    {

        r.doit(path3);
    }
    if (i == 4)
    {

        r.doit(path4);
    }

    if (i == 0)
    {
        cout << "bitte geben Sie ihre path mit der endung .txt" << endl;
        cin >> path5;
        r.doit(path5);
    }

    return 0;
}
