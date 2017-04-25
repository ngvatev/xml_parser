#ifndef xml_element_H
#define xml_element_H

#include <iostream>
#include <string>
#include <list>
#include <map>

class xml_parser;

#include "xml_parser.h"
#include "xpath_data.h"
#include "tokenizer.h"


class xml_element
{
private: // data
  typedef std::string string;
  string name;
  typedef std::map<string, string> xml_attributes;
  xml_attributes attributes;
  typedef std::vector<xml_element*> xml_elements;
  xml_elements inner_xml_elements;
  string text;


private: //methods
  xml_element ();

  // check if the current xml satisfies the requirements
  bool is_result (const xml_requirements &requirements);
  // check if the attributes satisfy the requirements
  bool verify_attributes (const std::map<string, string> &requirements);
  // check if the inner xml elements satisfy the requirements
  bool verify_sub_xmls (const std::map<string, string> &requirements);

public: // methods
  xml_element (std::istream &i);
  xml_element (const string &xml);
  xml_element (const xml_element &xe);
  
  xml_element& operator= (const xml_element &xe);

  // add an attribute to the xml_element
  void add_attribute (const string &key, const string &value);
  // add an inner xml_element pos is in [0 : inner xml size]
  void add_xml_element (const string &str, size_t pos);

  // remove an attribute
  void remove_attribute (const string &key);
  // remove an inner xml_element pos is in [0 : inner xml size)
  void remove_xml_element (size_t pos);

  // get name of the xml_element
  const string& get_name () const;
  // get all the attributes
  const std::map<string, string>& get_attributes () const; 
  // check if an attribute with key exists
  bool find_attribute (const string &key) const;
  // get the value of attribute with key key
  const string& get_attribute (const string &key) const;
  // get the number of inner xml elements
  size_t inner_xml_elements_size () const;
  const xml_element* get_inner_xml_element (size_t pos) const;
  // get text
  const string& get_text () const;

  // execute xpath in current xml
  xml_elements& xpath (const string &xpath_str,
                       xml_elements &rl);

  void clear();
  ~xml_element ();

  //outputs xml element and its subelements
  friend std::ostream& operator<< (std::ostream &o, const xml_element &e);
  friend xml_parser;
};

std::ostream& operator<< (std::ostream &o, const xml_element &e);
#endif
