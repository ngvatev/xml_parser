#ifndef XPATH_DATA_H_
#define XPATH_DATA_H_

#include <string>
#include <queue>
#include <map>
#include "tokenizer.h"

class xml_requirements
{
private:
  typedef std::string string;
public:
   xml_requirements (string name):name(name),
                            index(-1){}
  // name of an xml_element
  string name;

  // the attributes that that xml_element should have
  // ex. person(@id=2)/name
  std::map<string, string> attribute;

  // the text that sub xml_elements of the xml_element should have
  // ex. person(address="USA")/name
  std::map<string, string> subxml;

  // -1 if no index is specified
  int index;
};

class xpath_data
{
private:
  typedef std::string string;

private: // data
  static const string delimeters_str;

private: // methods
  bool parse_square_brakets (tokenizer &xpath_tokenizer);
  bool parse_attribute_requirement (tokenizer &xpath_tokenizer);
  bool parse_subxml_elemen_trequirement (tokenizer &xpath_tokenizer);
  bool parse_requirements (tokenizer &xpath_tokenizer);
public: // methods
  // parse xpath
  bool parse_xpath (const string &xpath);

  // a path describing xml elements
  std::queue<xml_requirements> xml_elements_path;
};

#endif /* XPATHREADABLE_PARSER_H_ */
