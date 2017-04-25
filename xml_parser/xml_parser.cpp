#include "xml_element.h"
#include "xml_parser.h"

#include <cctype>
#include <iterator>

const std::string xml_parser::delimiters_str = "<>=/";


xml_parser::xml_parser () :
  xml_tokenizer (delimiters_str)
{
}

bool
xml_parser::parse (xml_element &return_xml,
                   const string &xml_text)
{
  this->xml_tokenizer.set_text (xml_text);
  this->elements_in_construction.push (&return_xml);
  if (this->parse () == false)
    return false; // some xml syntax error
  return true;
}

bool
xml_parser::parse ()
{
  // parse the main xml tag
  if (this->xml_tokenizer.next () == false ||
      this->xml_tokenizer.token ().compare ("<") != 0 ||
      this->xml_tokenizer.next () == false ||
      this->parse_opening_tag_content () == false ||
      this->xml_tokenizer.token ().compare (">") != 0 ||
      this->xml_tokenizer.next () == false)
    return false;

  // parse until we find the closing tag for the main xml tag
  while (!this->elements_in_construction.empty ())
    {

      if (this->xml_tokenizer.token ().compare ("<") == 0)
        { // we have xml tag
          if (this->parse_xml_tag() == false)
            return false;
        }
      else if (this->xml_tokenizer.isdelimiter () == false)
        {
          // we have text
          if (this->parse_text () == false)
            return false;
        }
      else // we have bad xml syntax
        return false;
    }
  return true;
}

bool
xml_parser::parse_xml_tag ()
{
  // expectin <xml_name key1=value1 ... key_n=value_n >
  if (this->xml_tokenizer.token ().compare("<") != 0 ||
      this->xml_tokenizer.next() == false)
    return false;

  if (this->xml_tokenizer.token ().compare ("/") == 0)
    {
      // we have closing tag </...>
      if (this->parse_closing_tag_content() == false)
        return false;
      this->elements_in_construction.pop ();
    }
  else
    {
      // we have opening tag <...>
      xml_element* new_xml_element = new xml_element;
      this->elements_in_construction.top ()->
          inner_xml_elements.push_back (new_xml_element);
      this->elements_in_construction.push (new_xml_element);
      if (this->parse_opening_tag_content () == false)
        return false;
    }
  if (this->xml_tokenizer.token ().compare (">") != 0)
    return false;
  this->xml_tokenizer.next ();
  return true;
}

bool
xml_parser::parse_text ()
{
  if (this->xml_tokenizer.isdelimiter ())
    return false; // the expected text is not delimiter

  string &text = this->elements_in_construction.top ()->text;
  if (text.empty())
    { // the first word doesnt have " " in front
      text.assign (this->xml_tokenizer.token ());
      if (this->xml_tokenizer.next() == false)
        return false;
    }

  while (!this->xml_tokenizer.isdelimiter ())
    { // read and append words
      text.append(" ");
      text.append (this->xml_tokenizer.token ());
      if (this->xml_tokenizer.next() == false)
        return false;
    }

  return true;

}

bool
xml_parser::parse_opening_tag_content ()
{
  // expecting <xml_name key1=value1 key_n=value_n >
  if (this->xml_tokenizer.isdelimiter ())
    return false; // the name can not be a delimiter
  xml_element &current_xml = *this->elements_in_construction.top ();
  current_xml.name.assign(this->xml_tokenizer.token ());

  if (this->xml_tokenizer.next () == false)
    return false; // we are expecting closing token >
  while (!this->xml_tokenizer.isdelimiter ())
    {
      // read key value sequences
      string key, value;
      key.assign (this->xml_tokenizer.token ());
      if (this->xml_tokenizer.next () == false ||
          this->xml_tokenizer.token ().compare ("=") != 0 ||
          this->xml_tokenizer.next () == false ||
          this->xml_tokenizer.isdelimiter ())
        return false;
      value.assign (this->xml_tokenizer.token ());
      current_xml.add_attribute (key, value);
      if (this->xml_tokenizer.next () == false)
        return false; // expecting key=value or >
    }
  return true;
}

bool
xml_parser::parse_closing_tag_content ()
{
  // expecting /some_xml_name
  if (this->xml_tokenizer.token ().compare ("/") != 0 ||
      this->xml_tokenizer.next () == false)
    return false;

  // compare opening and closing names
  xml_element &current_xml = *this->elements_in_construction.top ();
  string current_tag_name = current_xml.name;
  if (this->xml_tokenizer.token ().compare (current_tag_name) != 0)
    return false; // opening and closing names dont match
  if (this->xml_tokenizer.next () == false)
    return false; // we are expecting closing >
  return true;
}





