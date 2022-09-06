class TermHelper {
  public:
    TermHelper();
    ~TermHelper();
    static bool generateTerm();
    static int getSolution();
    static String getTerm();

  private:
    static signed int TermSolution;
    static String TermString;
};