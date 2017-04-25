#ifndef XML_PARSER_H_
#define XML_PARSER_H_

#include "tokenizer.h"

#include <string>
#include <stack>

class xml_element;

class xml_parser
{
private:
  typedef std::string string;

private: // data
  // xml elements that are being constructed
  // the top of the stack is the element we are currently constructing
  std::stack<xml_element *> elements_in_construction;
  tokenizer xml_tokenizer;
  static const string delimiters_str;

private: // methods
  // parses a whole xml text
  bool parse ();

  // parses <....>
  bool parse_xml_tag ();

  // parses text until a delimiter is reached and
  // the space between words are substituted with a single space
  bool parse_text ();

  // parses the content of opening xml tag
  bool parse_opening_tag_content ();

  // parses the content of closing xml tag
  bool parse_closing_tag_content ();

public: // methods

  // initializations
  xml_parser ();

  // return_xml will be filled with data according to xml_text
  // returns false if the xml has syntax error
  bool parse (xml_element &return_xml,
              const string &xml_text);
};
#endif /* XML_PARSER_H_ */
