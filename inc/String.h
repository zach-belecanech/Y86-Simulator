//This is our own implementation of a String
//class.  We can create a String object out of a c-string
//or a std::string.

class String
{
   private:
      char * str;     //constructor dynamically allocates array of characters
      int32_t length; //length of array
    
      //method only used by other methods in this file
      bool badIndex(int32_t idx);
   public:
      String(std::string str);
      char * get_cstr();
      std::string get_stdstr();
      int32_t get_length();
      bool isSpaces(int32_t sIdx, int32_t eIdx, bool & error);
      uint32_t convert2Hex(int32_t sIdx, int32_t eIdx, bool & error);
      bool isChar(char what, int32_t idx, bool & error);
      bool isSubString(char * subStr, int32_t idx, bool & error);
      bool isSubString(std::string subStr, int32_t idx, bool & error);
};

